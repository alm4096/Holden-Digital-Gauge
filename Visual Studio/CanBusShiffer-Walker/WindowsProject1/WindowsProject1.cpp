//------------------------
//OBDII HIGH SPEED SNIFFER
//------------------------
//Use MCP2515 and Arduino/ESP32 to capture packets, send back via serial at 1MBPs
//
//Send in format ID DATA\r\n  (i.e. do not send DTS/Length)
//E.g. F1 624403400000
//
//Save all data using Laptop with putty and log to file. Open the file with this
//program
//------------------------


// WindowsProject1.cpp : Defines the entry point for the application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "WindowsProject1.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <commdlg.h> // Include for OPENFILENAME
#include <iostream>
#include <fstream>
#include <unordered_set> // For efficient lookup of existing IDs
#include <algorithm> //for sorting
#include <bitset>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void RunBackgroundCode();
std::string OpenFileDlg(HWND hwnd);
std::string filePath;
void AddTextToStatic(HWND hStatic, const std::wstring& textToAdd);
void SetTextToStatic(HWND hStatic, const std::wstring& textToAdd);

HWND g_hButton, g_hButtonFileOpenDiag, g_hTextBox, g_hStaticTextBoxContents, g_hStaticTextBoxIDs, g_hStaticTextBoxFiltered,
g_hStaticTextBoxConvertedValues, g_hTextBoxScale, g_hTextBoxOffset, g_hTextBoxFirstByte, g_hTextBoxLastByte, g_hButtonConvert, g_hStaticTextBoxHex;


char* readBinaryFile(const std::string& filename);
std::string InFileName = "C:\\Users\\ALM1024\\Documents\\Visual Studio 2022\\Projects\\CANBusHacking\\WindowsProject1\\DriveToBunnings.log";
char* InFileBuffer= nullptr;
char* IDBuffer = nullptr;
char* filterLinesBuffer=nullptr;
char* convertedBuffer = nullptr;

void ConvertValues();
void ConvertBinValues();

#include <iostream>
#include <cstring>

std::string GetStaticText(HWND hStatic);

bool isValidHexChar(char ch) {
    return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F');
}

#include <sstream>

// Function to convert a hex string to an integer
uint64_t hexStringToInt(const std::string& hexString) {
    std::stringstream ss;
    ss << std::hex << hexString;
    uint64_t result;
    ss >> result;
    return result;
}

std::string reverseString(const std::string& str) {
    // Create a new string to store the reversed string
    std::string reversed;

    // Iterate through the original string in reverse order
    for (int i = str.length() - 1; i >= 0; --i) {
        // Append each character to the reversed string
        reversed += str[i];
    }

    return reversed;
}


uint64_t reverseBits(uint64_t n) {
    uint64_t result = 0;
    int numBits = sizeof(n) * 8; // Number of bits in uint64_t

    for (int i = 0; i < numBits; ++i) {
        result <<= 1; // Left shift result to make space for the next bit
        result |= (n & 1); // Set the least significant bit of result to the least significant bit of n
        n >>= 1; // Right shift n to get next bit
    }

    return result;
}

uint64_t filterAndShiftHexValueNonInvert(const std::string& hexValue, const std::string& startByte, const std::string& byteLength) {
    // Convert hexValue, startByte, and byteLength to integers
    uint64_t value = hexStringToInt(hexValue);
    uint64_t start = std::stoi(startByte);
    uint64_t length = std::stoi(byteLength);


    //uint64_t newstart = ((uint64_t(hexValue.length() * 4)) - (start + length));
    value = reverseBits(value);
    uint64_t mask = ((((uint64_t)1 << (length))) << start);
    if (length == 64) {
        mask = -1;
    }

    // Filter the hex value using startByte and byteLength
    value &= mask; // Mask to keep only the least significant bytes
    value >>= (start); // Shift the value so that the start byte is now byte zero


    return value;
}


uint64_t filterAndShiftHexValue(const std::string& hexValue, const std::string& startByte, const std::string& byteLength) {
    // Convert hexValue, startByte, and byteLength to integers
    uint64_t value = hexStringToInt(hexValue);
    uint64_t start = std::stoi(startByte);
    uint64_t length = std::stoi(byteLength);

    uint64_t newstart = ((uint64_t(hexValue.length() * 4)) - (start+ length));

    uint64_t mask = ((((uint64_t)1 << (length)) - (uint64_t)1) << newstart);
    if (length == 64) {
        mask = -1;
    }

    // Filter the hex value using startByte and byteLength
    value &= mask; // Mask to keep only the least significant bytes
    value >>= (newstart); // Shift the value so that the start byte is now byte zero

    return value;
}

char* convertDataLinesBinary(const char* input, std::string l_FirstByte, std::string l_ByteLength) {
    const char* delimiter = "\r\n"; // Line delimiter (CR/LF)
    const size_t maxLineLength = 50; // Maximum line length (adjust as needed)

    std::vector<std::string> validIDs; // To store valid IDs

    char* copied = new char[strlen(input) + 1];

    // Copy the contents from the original char* to the new one
    strcpy(copied, input);

    //input = copied;

    const char* line = strtok(const_cast<char*>(copied), delimiter);
    while (line != nullptr) {
        // Check if the line starts with valid hex characters followed by a space
        size_t len = strlen(line);

        bool isValid = true;
        if (len < 3) {
            isValid = false;
        }
        int spaces = 0;
        for (int i = 0; i < len; i++) {
            if ((!isValidHexChar(line[i])) && (line[i] != ' ')) {
                isValid = false;
                break; // Exit loop early
            }
            else if (line[i] == ' ') {
                spaces++;
            }
        }
        if (spaces != 1) {
            isValid = false;
        }

        if (isValid) {
            std::string StrLine = line;
            std::string ID = StrLine.substr(0, StrLine.find(" "));
            std::string Contents = StrLine.substr(StrLine.find(" ") + 1, StrLine.length());


            if ((ID.length() <= 3) && (StrLine.length() > ID.length() + 2)) {
                //std::string l_Scale, std::string l_Offset, std::string l_FirstByte, std::string l_ByteLength
                double l_ret = (double)filterAndShiftHexValue(Contents, "0", std::to_string(Contents.length()*4));
                //double l_val = (l_ret * std::stod(l_Scale) + std::stod(l_Offset));


                uint64_t bits = (uint64_t)l_ret;
                std::bitset<64> binary(bits);

                // Store the binary string in a std::string
                std::string binaryString = binary.to_string();

                int l_Start = (binaryString.length() - Contents.length() * 4);
                int l_Length = Contents.length() * 4;
                if ((l_Start >= 0) && (l_Length>0)) {
                    binaryString = binaryString.substr(l_Start, l_Length);
                }

                // Check if the ID is not already in the vector
                //if (std::find(validIDs.begin(), validIDs.end(), ID) == validIDs.end()) {
                validIDs.push_back(binaryString); // Add the ID to the vector
                //}
            }
        }

        line = strtok(nullptr, delimiter); // Move to the next line
    }


    // Construct the result
    std::string result;
    for (const auto& id : validIDs) {
        result.append(id.c_str());
        result.append("\r\n"); // Add CR/LF
    }

    delete[]copied;

    char* temp = new char[result.length() + 1];
    strcpy(temp, result.c_str());

    return temp;
}


char* convertDataLines(const char* input, std::string l_Scale, std::string l_Offset, std::string l_FirstByte, std::string l_ByteLength) {
    const char* delimiter = "\r\n"; // Line delimiter (CR/LF)
    const size_t maxLineLength = 50; // Maximum line length (adjust as needed)

    std::vector<std::string> validIDs; // To store valid IDs

    char* copied = new char[strlen(input) + 1];

    // Copy the contents from the original char* to the new one
    strcpy(copied, input);

    //input = copied;

    const char* line = strtok(const_cast<char*>(copied), delimiter);
    while (line != nullptr) {
        // Check if the line starts with valid hex characters followed by a space
        size_t len = strlen(line);

        bool isValid = true;
        if (len < 3) {
            isValid = false;
        }
        int spaces = 0;
        for (int i = 0; i < len; i++) {
            if ((!isValidHexChar(line[i])) && (line[i] != ' ')) {
                isValid = false;
                break; // Exit loop early
            }
            else if (line[i] == ' ') {
                spaces++;
            }
        }
        if (spaces != 1) {
            isValid = false;
        }

        if (isValid) {
            std::string StrLine = line;
            std::string ID = StrLine.substr(0, StrLine.find(" "));
            std::string Contents = StrLine.substr(StrLine.find(" ") + 1, StrLine.length());


            if ((ID.length() <= 3) && (StrLine.length() > ID.length() + 2)) {
                //std::string l_Scale, std::string l_Offset, std::string l_FirstByte, std::string l_ByteLength
                double l_ret = (double)filterAndShiftHexValue(Contents, l_FirstByte, l_ByteLength);
                double l_val = (l_ret * std::stod(l_Scale) + std::stod(l_Offset));
                std::string l_Value= std::to_string(l_val);
              
                // Check if the ID is not already in the vector
                //if (std::find(validIDs.begin(), validIDs.end(), ID) == validIDs.end()) {
                validIDs.push_back(l_Value); // Add the ID to the vector
                //}
            }
        }

        line = strtok(nullptr, delimiter); // Move to the next line
    }


    // Construct the result
    std::string result;
    for (const auto& id : validIDs) {
        result.append(id.c_str());
        result.append("\r\n"); // Add CR/LF
    }

    delete[]copied;

    char* temp = new char[result.length() + 1];
    strcpy(temp, result.c_str());

    return temp;
}


char* processLines(const char* input) {
    const char* delimiter = "\r\n"; // Line delimiter (CR/LF)
    const size_t maxLineLength = 50; // Maximum line length (adjust as needed)

    std::vector<std::string> validIDs; // To store valid IDs
    std::vector<long> validIDsCount; // To store valid IDs
    std::vector<std::string> validVals; // To store valid IDs
    std::vector<long> validUniqueVals; // To store valid IDs

    char* copied = new char[strlen(input) + 1];

    // Copy the contents from the original char* to the new one
    strcpy(copied, input);

    //input = copied;

    const char* line = strtok(const_cast<char*>(copied), delimiter);
    while (line != nullptr) {
        // Check if the line starts with valid hex characters followed by a space
        size_t len = strlen(line);

        bool isValid = true;
        if (len < 3) {
            isValid = false;
        }
        int spaces = 0;
        for (int i = 0; i < len; i++) {
            if ((!isValidHexChar(line[i])) && (line[i] != ' ')) {
                isValid = false;
                break; // Exit loop early
            }
            else if (line[i] == ' ') {
                spaces++;
            }
        }
        if (spaces != 1) {
            isValid = false;
        }

        if (isValid) {
            std::string StrLine = line;
            std::string ID = StrLine.substr(0, StrLine.find(" "));
            std::string Contents = StrLine.substr(StrLine.find(" ") + 1, StrLine.length());

            if ((ID.length() <= 3) && (StrLine.length() > ID.length() + 2)) {
                // Check if the ID is not already in the vector

                if (std::find(validIDs.begin(), validIDs.end(), ID) == validIDs.end()) {
                    validIDs.push_back(ID); // Add the ID to the vector
                    validIDsCount.push_back(1);
                    validVals.push_back(Contents);
                    validUniqueVals.push_back(1);
                }
                else {
                    //Inc counter
                    auto it = std::find(validIDs.begin(), validIDs.end(), ID);
                    size_t elementNumber = std::distance(validIDs.begin(), it);
                    validIDsCount.at(elementNumber)++;
                    if (Contents != validVals.at(elementNumber)) {
                        validUniqueVals.at(elementNumber)++;
                    }

                }
            }
        }

        line = strtok(nullptr, delimiter); // Move to the next line
    }

    for (int i = 0; i < validIDsCount.size(); i++) {
        validIDs.at(i) = ((validIDs.at(i).append("(")).append(std::to_string(validIDsCount.at(i)))).append(")");
        if ((validIDsCount.at(i) < 20) || (validUniqueVals.at(i) == 1))  {
            validIDsCount.erase(validIDsCount.begin() + i);
            validIDs.erase(validIDs.begin() + i);
            validUniqueVals.erase(validUniqueVals.begin() + i);
            validVals.erase(validVals.begin() + i);
            i -= 1;
        }
    }

    // Sort the valid IDs
    std::sort(validIDs.begin(), validIDs.end());

    // Construct the result
    std::string result;
    for (const auto& id : validIDs) {
        result.append(id.c_str());
        result.append("\r\n"); // Add CR/LF
    }

    delete[]copied;

    char* temp = new char[result.length() + 1];
    strcpy(temp, result.c_str());

    return temp;
}


char* filterLines(const char* input, std::string IDToFilter) {
    const char* delimiter = "\r\n"; // Line delimiter (CR/LF)
    const size_t maxLineLength = 50; // Maximum line length (adjust as needed)

    std::vector<std::string> validIDs; // To store valid IDs

    char* copied = new char[strlen(input) + 1];

    // Copy the contents from the original char* to the new one
    strcpy(copied, input);

    const char* line = strtok(const_cast<char*>(copied), delimiter);
    while (line != nullptr) {
        // Check if the line starts with valid hex characters followed by a space
        size_t len = strlen(line);

        bool isValid = true;
        if (len < 3) {
            isValid = false;
        }
        int spaces = 0;
        for (int i = 0; i < len; i++) {
            if ((!isValidHexChar(line[i])) && (line[i] != ' ')) {
                isValid = false;
                break; // Exit loop early
            }
            else if (line[i] == ' ') {
                 spaces++;
            }
        }
        if (spaces != 1) {
            isValid = false;
        }

        if (isValid) {
            std::string StrLine = line;
            std::string ID = StrLine.substr(0, StrLine.find(" "));
            std::string Contents = StrLine.substr(StrLine.find(" ")+1, StrLine.length() );

            if ((ID.length() <= 3)&&(StrLine.length()>ID.length() + 2)) {
                if (IDToFilter == ID) {
//                    validIDs.push_back(Contents); // Add the ID to the vector
                    validIDs.push_back(StrLine); // Add the ID to the vector
                }
            }
        }

        line = strtok(nullptr, delimiter); // Move to the next line
    }

    // Sort the valid IDs
    //std::sort(validIDs.begin(), validIDs.end());

    // Construct the result
    std::string result;
    for (const auto& id : validIDs) {
        result.append(id.c_str());
        result.append("\r\n"); // Add CR/LF
    }

    delete[]copied;

    char* temp = new char[result.length()+1];
    strcpy(temp, result.c_str());

    return temp;
}

// Convert wchar_t to std::string
std::string WideStringToString(const wchar_t* wideString) {
    std::wstring ws(wideString);
    return std::string(ws.begin(), ws.end());
}

std::string wstringToString(const std::wstring& wstr) {
    // Create a locale object with the system's default locale
    std::locale loc("");

    // Convert the wide string to narrow string using the locale
    std::string result;
    for (wchar_t wch : wstr) {
        result += std::use_facet<std::ctype<wchar_t>>(loc).narrow(wch, '?');
    }

    return result;
}

std::wstring charPtrToWstring(char* charArray) {
    // Convert the char* to a wstring
    std::wstring wstr;
    for (char* ptr = charArray; *ptr != '\0'; ++ptr) {
        wstr += static_cast<wchar_t>(*ptr);
    }
    return wstr;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)BLACK_BRUSH;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   int width = 700; // Specify your desired width here
   int height = 730; // Specify your desired height here
   int xPos = 675; // Specify your desired X position here
   int yPos = 10; // Specify your desired Y position here

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       xPos, yPos, width, height, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return2
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:

            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case 4:
                ConvertBinValues();
                break;
            case 3:
                ConvertValues();
                break;
            case 1:
                { // Button ID is 1
                    // Button clicked, update static text
                    //SetWindowTextW(g_hStaticTextBoxContents, L"Button Pressed!");
                    RunBackgroundCode();
                }
                break;
            case 2:
                // Button clicked event
                filePath = OpenFileDlg(hWnd);
                if (!filePath.empty()) {
                    if (InFileBuffer != nullptr) {
                        SetWindowTextA(g_hStaticTextBoxContents, InFileBuffer);
                        
                        if (IDBuffer != nullptr) {
                            delete[]IDBuffer;
                            IDBuffer = nullptr;
                        }
                        IDBuffer=processLines(InFileBuffer);
                        if (IDBuffer != nullptr) {
                            SetWindowTextA(g_hStaticTextBoxIDs, IDBuffer);
                        }
                    }
                    else {
                        SetWindowTextA(g_hStaticTextBoxContents, (std::string("Error opening: ").append(filePath)).c_str());
                    }
                }

                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
        g_hButton=CreateWindowW(L"BUTTON", L"Filter ID", WS_VISIBLE | WS_CHILD, 10, 90, 100, 30, hWnd, (HMENU)1, NULL, NULL);
        g_hTextBox=CreateWindowW(L"EDIT", L"4C1", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 50, 40, 30, hWnd, NULL, NULL, NULL);
        g_hStaticTextBoxHex = CreateWindowEx(0, L"EDIT", L"Dec", WS_VISIBLE | WS_CHILD | ES_READONLY,
            50, 50, 40, 30, hWnd, NULL, NULL, NULL);

        g_hStaticTextBoxContents = CreateWindowEx(0,L"EDIT", L"Open a file to start", WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
            120, 10, 200, 350, hWnd, NULL, NULL, NULL);
        g_hStaticTextBoxIDs = CreateWindowEx(0, L"EDIT", L"ID List Here", WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
            340, 10, 95, 190, hWnd, NULL, NULL, NULL);
        g_hStaticTextBoxFiltered = CreateWindowEx(0, L"EDIT", L"Filtered ID List Here", WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
            445, 10, 200, 190, hWnd, NULL, NULL, NULL);

        CreateWindowEx(0, L"STATIC", L"0_______8_______16______24______32______40______48______56______64\r\n1_______2_______3_______4_______5_______6_______7_______8", WS_VISIBLE | WS_CHILD, 10, 90 + 40 * 7, 550, 30, hWnd, NULL, NULL, NULL);
                                     //________________________________________________________________
        g_hStaticTextBoxConvertedValues = CreateWindowEx(0, L"EDIT", L"Converted List Here", WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
            10, 90+40*8, 550, 250, hWnd, NULL, NULL, NULL);

        g_hButtonConvert = CreateWindowW(L"BUTTON", L"Convert Values", WS_VISIBLE | WS_CHILD, 10, 90+40*1, 100, 30, hWnd, (HMENU)3, NULL, NULL);
        g_hTextBoxScale = CreateWindowW(L"EDIT", L"1", WS_VISIBLE | WS_CHILD | WS_BORDER, 60, 90+40*2, 50, 30, hWnd, NULL, NULL, NULL);
        CreateWindowEx(0, L"STATIC", L"Scale", WS_VISIBLE | WS_CHILD, 10, 90 + 40 * 2, 40, 30, hWnd, NULL, NULL, NULL);
        g_hTextBoxOffset = CreateWindowW(L"EDIT", L"-40", WS_VISIBLE | WS_CHILD | WS_BORDER, 60, 90 + 40 * 3, 50, 30, hWnd, NULL, NULL, NULL);
        CreateWindowEx(0, L"STATIC", L"Offset", WS_VISIBLE | WS_CHILD, 10, 90 + 40 * 3, 40, 30, hWnd, NULL, NULL, NULL);
        g_hTextBoxFirstByte = CreateWindowW(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER, 60, 90 + 40 * 4, 50, 30, hWnd, NULL, NULL, NULL);
        CreateWindowEx(0, L"STATIC", L"1stByte", WS_VISIBLE | WS_CHILD, 10, 90 + 40 * 4, 40, 30, hWnd, NULL, NULL, NULL);
        g_hTextBoxLastByte = CreateWindowW(L"EDIT", L"64", WS_VISIBLE | WS_CHILD | WS_BORDER, 60, 90 + 40 * 5, 50, 30, hWnd, NULL, NULL, NULL);
        CreateWindowEx(0, L"STATIC", L"NoBytes", WS_VISIBLE | WS_CHILD, 10, 90 + 40 * 5, 40, 30, hWnd, NULL, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Convert Binary", WS_VISIBLE | WS_CHILD, 10, 90 + 40 * 6, 100, 30, hWnd, (HMENU)4, NULL, NULL);


        g_hButtonFileOpenDiag = CreateWindow(
            L"BUTTON",                      // Predefined class; Unicode assumed
            L"Open File",                   // Button text
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
            10, 10, 100, 30,               // Position and size
            hWnd,                           // Parent window
            (HMENU)2,                       // No menu
            NULL,                          // Instance handle
            NULL                            // Additional application data
        );
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CTLCOLOREDIT:
        // Set background color of edit and static controls to system window color
        SetBkColor(reinterpret_cast<HDC>(wParam), RGB(0, 0, 0));
        SetTextColor(reinterpret_cast<HDC>(wParam), RGB(0, 200, 255)); // White background color
        return (LRESULT)GetStockObject(BLACK_BRUSH);
    case WM_CTLCOLORSTATIC:
        // Set background color of edit and static controls to system window color
        SetBkColor(reinterpret_cast<HDC>(wParam), RGB(0, 0, 0));
        SetTextColor(reinterpret_cast<HDC>(wParam), RGB(0, 200, 255)); // White background color
        return (LRESULT)GetStockObject(BLACK_BRUSH);
    case WM_CTLCOLORBTN:
        // Set background color of edit and static controls to system window color
        SetBkColor(reinterpret_cast<HDC>(wParam), RGB(0, 0, 0));
        SetTextColor(reinterpret_cast<HDC>(wParam), RGB(0, 200, 255)); // White background color
        return (LRESULT)GetStockObject(BLACK_BRUSH);
    case WM_CTLCOLORDLG:
        // Set background color of edit and static controls to system window color
        SetBkColor(reinterpret_cast<HDC>(wParam), RGB(0, 0, 0));
        SetTextColor(reinterpret_cast<HDC>(wParam), RGB(0, 200, 255)); // White background color
        return (LRESULT)GetStockObject(BLACK_BRUSH);
    case WM_CTLCOLORLISTBOX:
        // Set background color of edit and static controls to system window color
        SetBkColor(reinterpret_cast<HDC>(wParam), RGB(0, 0, 0));
        SetTextColor(reinterpret_cast<HDC>(wParam), RGB(0, 200, 255)); // White background color
        return (LRESULT)GetStockObject(BLACK_BRUSH);
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Function to run code in the background
void RunBackgroundCode() {
    // Example code that interacts with the GUI elements
    // You can replace this with your actual background code

    HDC hdc = GetDC(g_hStaticTextBoxFiltered);


    // Get text from the text box
    wchar_t buffer[256];
    GetWindowText(g_hTextBox, buffer, 256);

    // Convert wchar_t to std::string
    std::string text = WideStringToString(buffer);

    SetTextToStatic(g_hStaticTextBoxHex, charPtrToWstring((char*)std::to_string(hexStringToInt(text)).c_str()));

    if (filterLinesBuffer != nullptr) {
        delete[] filterLinesBuffer;
        filterLinesBuffer = nullptr;
    }
    if ((IDBuffer != nullptr) && (InFileBuffer != nullptr)) {
        filterLinesBuffer = filterLines(InFileBuffer, text);
    }

    if (filterLinesBuffer != nullptr) {
        //AddTextToStatic(g_hStaticTextBoxFiltered, buffer);
        SetTextToStatic(g_hStaticTextBoxFiltered, charPtrToWstring(filterLinesBuffer));
        //delete[] filterLinesBuffer;
    }
    else {
        //SetTextToStatic(g_hStaticTextBoxContents,std::wstring("Error").c_str());
    }

    //Auto Binary
    ConvertBinValues();

}


// Function to add text to the static text
std::string GetStaticText(HWND hStatic) {
    // Get the current text of the static control
    int textLength = GetWindowTextLength(hStatic);
    std::wstring currentText(textLength + 1, L'\0');
    GetWindowText(hStatic, &currentText[0], textLength + 1);

    currentText = currentText.substr(0, currentText.length() - 1);

    // Append the new text
    //currentText += textToAdd;

    // Set the new text
    //SetWindowText(hStatic, currentText.c_str());
    return wstringToString(currentText);
}


// Function to add text to the static text
void AddTextToStatic(HWND hStatic, const std::wstring& textToAdd) {
    // Get the current text of the static control
    int textLength = GetWindowTextLength(hStatic);
    std::wstring currentText(textLength + 1, L'\0');
    GetWindowText(hStatic, &currentText[0], textLength + 1);

    currentText = currentText.substr(0, currentText.length()-1);

    // Append the new text
    currentText += textToAdd;

    // Set the new text
    SetWindowText(hStatic, currentText.c_str());
}

// Function to add text to the static text
void SetTextToStatic(HWND hStatic, const std::wstring& textToAdd) {
    // Get the current text of the static control
    int textLength = GetWindowTextLength(hStatic);

    // Set the new text
    SetWindowText(hStatic, textToAdd.c_str());
}

// Function to open file dialog and return selected file path
std::string OpenFileDlg(HWND hwnd) {
    OPENFILENAME ofn;
    wchar_t szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE) {
        std::wstring ws(ofn.lpstrFile);

        if (InFileBuffer != nullptr) {
            delete[] InFileBuffer;
            InFileBuffer = nullptr;
        }

        InFileBuffer = readBinaryFile(wstringToString(ws));
        if (InFileBuffer!=nullptr) {
            //delete[] InFileBuffer;
            //Success
        }

        return std::string(ws.begin(), ws.end());
    }
    else {
        return ""; // User canceled dialog
    }
}


char* readBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return nullptr;
    }

    // Get the file size
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate memory for the buffer
    char* buffer = new char[fileSize];
    if (!buffer) {
        std::cerr << "Memory allocation failed." << std::endl;
        return nullptr;
    }

    // Read the entire file into the buffer
    if (!file.read(buffer, fileSize)) {
        std::cerr << "Error reading file: " << filename << std::endl;
        delete[] buffer;
        return nullptr;
    }

    file.close();
    return buffer;
}


void ConvertValues() {

    //g_hStaticTextBoxConvertedValues
    std::string m_Scale=GetStaticText(g_hTextBoxScale);
    std::string m_Offset = GetStaticText(g_hTextBoxOffset);
    std::string m_FirstByte = GetStaticText(g_hTextBoxFirstByte);
    std::string m_Length = GetStaticText(g_hTextBoxLastByte);
    
    if (filterLinesBuffer == NULL) {
        return;
    }
    std::string m_filtered = filterLinesBuffer;

    if (convertedBuffer != nullptr) {
        delete[] convertedBuffer;
        convertedBuffer = nullptr;
    }

    convertedBuffer = convertDataLines(filterLinesBuffer, m_Scale, m_Offset, m_FirstByte, m_Length);

    if (convertedBuffer != nullptr) {
        SetTextToStatic(g_hStaticTextBoxConvertedValues, charPtrToWstring(convertedBuffer));
    }
}


void ConvertBinValues() {

    //g_hStaticTextBoxConvertedValues
    std::string m_FirstByte = GetStaticText(g_hTextBoxFirstByte);
    std::string m_Length = GetStaticText(g_hTextBoxLastByte);


    std::string m_filtered = filterLinesBuffer;

    if (convertedBuffer != nullptr) {
        delete[] convertedBuffer;
        convertedBuffer = nullptr;
    }

    convertedBuffer = convertDataLinesBinary(filterLinesBuffer, m_FirstByte, m_Length);

    if (convertedBuffer != nullptr) {
        SetTextToStatic(g_hStaticTextBoxConvertedValues, charPtrToWstring(convertedBuffer));
    }

}

