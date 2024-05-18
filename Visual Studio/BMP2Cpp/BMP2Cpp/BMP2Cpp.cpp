#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <iomanip>

#pragma pack(1)
#define RUNLENGTHENCODE

struct BitmapFileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BitmapInfoHeader {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};

#pragma pack()

struct Image {
    int width;
    int height;
    int padding;
    std::vector<unsigned char> data;
};

Image LoadBitmap(const std::string& filename) {
    Image image;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << filename << std::endl;
        return image;
    }

    BitmapFileHeader fileHeader;
    BitmapInfoHeader infoHeader;

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BitmapFileHeader));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BitmapInfoHeader));

    if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 8) {
        std::cerr << "Error: Unsupported bit count: " << infoHeader.biBitCount << std::endl;
        return image;
    }

    image.width = infoHeader.biWidth;
    image.height = infoHeader.biHeight;

    int padding = (4 - (image.width * (infoHeader.biBitCount / 8)) % 4) % 4;
    image.padding = padding;

    int dataSize = (image.width * (infoHeader.biBitCount / 8) + padding) * image.height;
    image.data.resize(dataSize);

    file.seekg(fileHeader.bfOffBits, std::ios::beg);
    file.read(reinterpret_cast<char*>(image.data.data()), dataSize);

    return image;
}

void ConvertToGrayscale(Image& image) {
    if (image.data.empty())
        return;

    int bytesPerPixel = image.data.size() / (image.width * image.height);
    if (bytesPerPixel == 3) { // 24-bit color
        for (size_t i = 0; i < image.width*image.height*bytesPerPixel; i += 3) {

            if (i + 2 + (i / image.width/ bytesPerPixel) * image.padding < image.data.size()) {
                unsigned char gray = static_cast<unsigned char>(0.2126 * image.data[i + 2 + (i / image.width/ bytesPerPixel) * image.padding] +
                    0.7152 * image.data[i + 1 + (i / image.width/ bytesPerPixel) * image.padding] +
                    0.0722 * image.data[i + (i / image.width/ bytesPerPixel) * image.padding]);
                image.data[i / 3] = gray;
            }
        }
        image.data.resize(image.width * image.height);
    }
    else {
        for (size_t i = 0; i < image.width * image.height; i++) {
            image.data[i] = image.data[i + (i / image.width) * image.padding];
        }
        image.data.resize(image.width * image.height);
    }
    // No conversion needed for 8-bit grayscale
    /*struct Image {
        int width;
        int height;
        int padding;
        std::vector<unsigned char> data;
    };*/
    //Flip vertically (bitmap is inverted)
    for (int w = 0; w < image.width; w++) {
        for (int h = 0; h < image.height/2; h++) {
            unsigned char temp = image.data[h * image.width + w];
            image.data[h * image.width + w] = image.data[((image.height-1) - h) * image.width + w];
            image.data[((image.height-1) - h) * image.width + w] = temp;
        }
    }


}

void SaveAsTxt(const Image& image, const std::string& filename) {
    std::ofstream file(filename + ".txt");
    if (!file.is_open()) {
        std::cerr << "Error: Unable to create file: " << filename << ".txt" << std::endl;
        return;
    }
    file << "struct Image {" << std::endl;
    file << "    int width;" << std::endl;
    file << "    int height;" << std::endl;
    file << "    int datalen;" << std::endl;
    file << "    char charVal;" << std::endl;
    file << "    unsigned char * data;" << std::endl;
    file << " };" << std::endl;
    int filenamesta = filename.rfind("\\") + 1;
    int filenameend = filename.find(".bmp") - filenamesta;
    std::string dataname = filename.substr(filenamesta, filenameend);
    //file << "Width: " << image.width << std::endl;
    //file << "Height: " << image.height << std::endl;
    file << "const unsigned char ImgData_" << dataname << " [] PROGMEM = {" << std::endl << "  ";
    //file << "Data: ";
    int lineupto=0;
    std::ios init(NULL);
    init.copyfmt(file);
    int RLE = 0;
    int Datalen = 0;
    bool SkipNext=false;
    unsigned char RLEVal = 0;
    for (size_t i = 0; i < image.data.size(); ++i) {
#ifdef RUNLENGTHENCODE       
        if (i < image.data.size() - 1) {
            if ((image.data[i] == image.data[i+1])&& (RLE!=255)) {
                RLE++;
                if (RLE == 1) {
                    RLEVal = image.data[i];
                    RLE++; //because first char is actually 2 chars
                }
            }
            else if (RLE != 0) {
                if ((RLE > 3)|| (RLEVal== 0x64)) {
                    file << "0x64, ";//Escape character
                    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLEVal << ", ";
                    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLE << ", ";
                    lineupto += 3;
                    Datalen += 3;
                }
                else {
                    //Number of chars is not enough to make RLE worth it,just print them
                    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLEVal << ", ";
                    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLEVal << ", ";
                    lineupto += 2;
                    Datalen += 2;
                    if (RLE == 3) {
                        file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLEVal << ", ";
                        lineupto++;
                        Datalen++;
                    }
                }
                SkipNext = true;
                RLE = 0;
            }
        }

        if (SkipNext) {
            SkipNext = false;
        }
        else {
            if (RLE == 0) {
                if (image.data[i] == 0x64) {
                    file << "0x64, 0x64, 0x01, "; //escape character
                    lineupto += 3;
                    Datalen += 3;
                }
                else {
                    file << "0x";
                    file << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)image.data[i];
                    if (i < image.data.size() - 1) {
                        file << ", ";
                    }
                    lineupto++;
                    Datalen++;
                }
            }
        }

        if ((i == image.data.size() - 1) && (RLE > 0)) {
            if (RLE != 0) {
                if ((RLE > 3) || (RLEVal == 0x64)) {
                    file << "0x64, ";//Escape character
                    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLEVal << ", ";
                    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLE;
                    Datalen += 3;
                }
                else {
                    //Number of chars is not enough to make RLE worth it,just print them
                    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLEVal << ", ";
                    file << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLEVal;
                    Datalen+=2;
                    if (RLE == 3) {
                        file << ", 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)RLEVal;
                        Datalen++;
                    }
                }
                RLE = 0;
            }
        }
        else {
            if ((lineupto > 25) && (i > 0)) {
                file << std::endl << "  ";
                lineupto = 0;
            }
        }
#else
        file << "0x";
        file << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)image.data[i];
        if (i < image.data.size() - 1) {
            file << ", ";
        }
        lineupto++;

        if ((lineupto >= image.width) && (i > 0)) {
            file << std::endl << "  ";
            lineupto = 0;
        }
#endif

    }
    file << std::endl << " };" << std::endl;
    file.copyfmt(init);
    file << "Image ImgData_" << dataname << "_Img = {" << std::endl;
    file << "  .width = " << image.width << ","<<std::endl;
    file << "  .height = " << image.height << "," << std::endl;
    file << "  .datalen = " << Datalen << "," << std::endl;
    file << "  .charVal = '" << dataname << "'," << std::endl;
    file << "  .data = (unsigned char*)&ImgData_" << dataname << std::endl;
    file << "};";

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <bitmap_file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    Image image = LoadBitmap(filename);
    if (image.data.empty()) {
        std::cerr << "Error: Unable to load bitmap: " << filename << std::endl;
        return 1;
    }

    ConvertToGrayscale(image);
    SaveAsTxt(image, filename);

    std::cout << "Conversion successful. Output saved as: " << filename << ".txt" << std::endl;

    return 0;
}
