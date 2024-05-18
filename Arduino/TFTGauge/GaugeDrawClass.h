#ifndef GAUGEDRAWCLASSH
#define GAUGEDRAWCLASSH

#ifndef ADA
#define ADA
#include <Adafruit_GFX.h>	 // Include Adafruit graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#endif

#include <iostream>
#include "Art.h"

struct Image;

class GaugeDraw
{
public:
	virtual void DrawGauge(int x, int y, int number);						// Pure virtual function, redefine
	virtual void DrawGauge(int x, int y, double number);					// Pure virtual function
	virtual void DrawGauge(int x, int y, double number, int decimalPlaces); // Pure virtual function
	virtual void DrawGauge(int number);										// Pure virtual function, redefine
	virtual void DrawGauge(double number);									// Pure virtual function
	virtual void DrawGauge(double number, int decimalPlaces);				// Pure virtual function

	// Constructor to initialize the display pointer
	void DisplayManager(Adafruit_ST7789 *l_tft, uint16_t *l_DisplayBuff, uint16_t *l_TempBuffer);

	// Helper to convert colours
	uint16_t rgb565Color(unsigned char r, unsigned char g, unsigned char b);

	void SetXYPos(int l_X, int l_y);

private:
	float prevAngle = 0.0;
	int prevX1 = 0, prevY1 = 1, prevX2 = 2, prevY2 = 3, prevX3 = 4, prevY3 = 5, prevX4 = 6, prevY4 = 7; // Previous needle bounding box coordinates

protected:
	uint16_t *p_DisplayBuffer;
	uint16_t *p_TempBuff;
	Adafruit_ST7789 *p_tft;
	int m_xPos = 0;
	int m_yPos = 0;

	double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

	void DrawPixel(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y);
	void DrawPixel(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, bool DrawBlack);
	void DrawPixel(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, uint16_t *buff, int buffWidth);
	void DrawPixel(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, uint16_t *buff, int buffWidth, bool DrawBlack);
	void DrawPixelBuffer(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, bool useBuff, uint16_t *buff, int buffWidth);
	void DrawPixelBuffer(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, bool useBuff, uint16_t *buff, int buffWidth, bool DrawBlack);

	void DrawLine(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90);
	void DrawLine(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, bool DrawBlack);
	void DrawLine(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, uint16_t *buff, int buffWidth);
	void DrawLine(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, uint16_t *buff, int buffWidth, bool DrawBlack);
	void DrawLineBuffer(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, bool useBuff, uint16_t *buff, int buffWidth);
	void DrawLineBuffer(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, bool useBuff, uint16_t *buff, int buffWidth, bool DrawBlack);

	uint16_t Gray2Colour(unsigned char &brightness, uint16_t &color565);

	void DrawImageRLE(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90);
	void DrawImageRLE(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, bool DrawBlack);
	void DrawImageRLE(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, uint16_t *buff, int buffWidth);
	void DrawImageRLE(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, uint16_t *buff, int buffWidth, bool DrawBlack);
	void DrawImageRLEBuffer(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, bool useBuff, uint16_t *buff, int buffWidth);
	void DrawImageRLEBuffer(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, bool useBuff, uint16_t *buff, int buffWidth, bool DrawBlack);

	void DrawImage(Image img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour);

	String add127ToString(const String &inputString);

	int CalculateAngle(int fQ, int sQ, int sLo, int sHi, int num);

	void writeFastHLine(unsigned char *buff, uint16_t x, uint16_t y, uint16_t length, uint16_t buffWidth);
	void subFastHLine(unsigned char *buff, uint16_t x, uint16_t y, uint16_t length, uint16_t buffWidth, int minX, int minY, int maxX, int maxY);

	void _swap_int16_t(int16_t &x0, int16_t &x1);

	// Functions to draw a triangle into a buffer. If the BuffHeight is given it assumes you are trying to subtract the triangle from the given buffer (i.e. erasing previous location of gauge needle)
	void fillTriangleBuffer(unsigned char *buff, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int buffWidth, int offx, int offy);
	void fillTriangleBuffer(unsigned char *buff, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int buffWidth, int buffHeight, int offx, int offy, int maxx, int maxy);

	// Function to draw the needle
	void drawNeedle(int centerX, int centerY, int innerRadius, int outerRadius, float angleDeg, int width, uint16_t color);
	void drawNeedle(int centerX, int centerY, int innerRadius, int outerRadius, float angleDeg, int width, uint16_t color, uint16_t bgColor);
	void clearNonOverlapAreas(int point1X, int point1Y, int point2X, int point2Y, int point3X, int point3Y, int point4X, int point4Y, uint16_t bgColor);

	int DrawLargeNumber(int number, uint16_t colour, uint16_t *buff, int buffWidth);
	int DrawLargeNumber(int number, int x, int y, uint16_t colour);
	int DrawLargeNumber(int number, int x, int y, uint16_t colour, int Space);
	int DrawLargeNumber(int number, int x, int y, uint16_t colour, int Space, uint16_t *buff, int buffWidth);
	int DrawLargeNumberBuffer(int number, int x, int y, uint16_t colour, int Space, bool useBuff, uint16_t *buff, int buffWidth);

	int DrawMed(String message, uint16_t colour, uint16_t *buff, int buffWidth);
	int DrawMed(String message, int x, int y, uint16_t colour);
	int DrawMed(String message, int x, int y, uint16_t colour, int Space);
	int DrawMed(String message, int x, int y, uint16_t colour, int Space, uint16_t *buff, int buffWidth);
	int DrawMed(String message, int x, int y, uint16_t colour, uint16_t *buff, int buffWidth);
	int DrawMedBuffer(String message, int x, int y, uint16_t colour, int Space, bool useBuff, uint16_t *buff, int buffWidth);

	int DrawSmall(String message, uint16_t colour, uint16_t *buff, int buffWidth);
	int DrawSmall(String message, int x, int y, uint16_t colour);
	int DrawSmall(String message, int x, int y, uint16_t colour, int Space);
	int DrawSmall(String message, int x, int y, uint16_t colour, int Space, uint16_t *buff, int buffWidth);
	int DrawSmall(String message, int x, int y, uint16_t colour, uint16_t *buff, int buffWidth);
	int DrawSmallBuffer(String message, int x, int y, uint16_t colour, int Space, bool useBuff, uint16_t *buff, int buffWidth);

	int DrawTextBuffer(String message, int x, int y, uint16_t colour, int Space, bool useBuff, uint16_t *buff, int buffWidth, int PixelSize);
};

#endif