#include "GaugeDrawClass.h"

void GaugeDraw::SetXYPos(int l_x, int l_y)
{
  m_xPos = l_x;
  m_yPos = l_y;
}

double GaugeDraw::mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  // Ensure the input value is within the input range
  // x = constrain(x, in_min, in_max);
  if (x > in_max)
  {
    x = in_max;
  }
  if (x < in_min)
  {
    x = in_min;
  }

  // Map the input value to the output range
  double output = out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min);

  return output;
}

void GaugeDraw::DisplayManager(Adafruit_ST7789 *l_tft, uint16_t *l_DisplayBuff, uint16_t *l_TempBuffer)
{
  p_DisplayBuffer = l_DisplayBuff;
  p_TempBuff = l_TempBuffer;
  p_tft = l_tft;
}

void GaugeDraw::DrawPixel(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y)
{
  DrawPixelBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, false, NULL, 0, true);
}

void GaugeDraw::DrawPixel(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, bool DrawBlack)
{
  DrawPixelBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, false, NULL, 0, DrawBlack);
}

void GaugeDraw::DrawPixel(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, uint16_t *buff, int buffWidth)
{
  DrawPixelBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, true, buff, buffWidth, true);
}

void GaugeDraw::DrawPixel(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, uint16_t *buff, int buffWidth, bool DrawBlack)
{
  DrawPixelBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, true, buff, buffWidth, DrawBlack);
}

void GaugeDraw::DrawPixelBuffer(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, bool useBuff, uint16_t *buff, int buffWidth)
{
  DrawPixelBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, true, buff, buffWidth, true);
}

void GaugeDraw::DrawPixelBuffer(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, bool useBuff, uint16_t *buff, int buffWidth, bool DrawBlack)
{

  if ((Colour > 0) || (DrawBlack))
  {
    if (HFlip)
    {
      x = img.width - x + HPos;
    }
    else
    {
      x = x + HPos;
    }
    if (VFlip)
    {
      y = img.height - y + VPos;
    }
    else
    {
      y = y + VPos;
    }
    if (useBuff)
    {
      buff[x + y * buffWidth] = Colour;
    }
    else
    {
      p_tft->drawPixel(x, y, Colour);
    }
  }
}

void GaugeDraw::DrawLine(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90)
{
  DrawLineBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, count, rotate90, false, NULL /*not used*/, 0 /*not used*/, true);
}

void GaugeDraw::DrawLine(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, bool DrawBlack)
{
  DrawLineBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, count, rotate90, false, NULL /*not used*/, 0 /*not used*/, DrawBlack);
}

void GaugeDraw::DrawLine(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, uint16_t *buff, int buffWidth)
{
  DrawLineBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, count, rotate90, true, buff, buffWidth, true);
}

void GaugeDraw::DrawLine(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, uint16_t *buff, int buffWidth, bool DrawBlack)
{
  DrawLineBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, count, rotate90, true, buff, buffWidth, DrawBlack);
}

void GaugeDraw::DrawLineBuffer(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, bool useBuff, uint16_t *buff, int buffWidth)
{
  DrawLineBuffer(img, HFlip, VFlip, HPos, VPos, Colour, x, y, count, rotate90, true, buff, buffWidth, true);
}

void GaugeDraw::DrawLineBuffer(Image &img, bool &HFlip, bool &VFlip, int &HPos, int &VPos, int Colour, int x, int y, int count, bool rotate90, bool useBuff, uint16_t *buff, int buffWidth, bool DrawBlack)
{
  int y2;
  int x2;
  if ((Colour > 0) || (DrawBlack))
  {
    if (HFlip)
    {
      x = img.width - x + HPos;
      x2 = x - count;
    }
    else
    {
      x = x + HPos;
      x2 = x + count;
    }
    if (VFlip)
    {
      y = img.height - y + VPos;
      y2 = y - count;
    }
    else
    {
      y = y + VPos;
      y2 = y + count;
    }
    if (!rotate90)
    {
      if (useBuff)
      {
        for (int i = x; i <= x2; i++)
        {
          buff[i + y * buffWidth] = Colour;
        }
      }
      else
      {
        p_tft->drawLine(x, y, x2, y, Colour);
      }
    }
    else
    {
      if (useBuff)
      {
        for (int i = y; i <= y2; i++)
        {
          buff[x + i * buffWidth] = Colour;
        }
      }
      else
      {
        p_tft->drawLine(x, y, x, y2, Colour);
      }
    }
  }
}

uint16_t GaugeDraw::Gray2Colour(unsigned char &brightness, uint16_t &color565)
{
  /*
   // Extracting red, green, and blue components from the color565 value
    uint16_t red = (color565 >> 11) & 0x1F;    // 5 bits
    uint16_t green = (color565 >> 5) & 0x3F;   // 6 bits
    uint16_t blue = color565 & 0x1F;           // 5 bits

    // Scale each component based on the brightness value (0-255)
    red = (red * brightness) / 255;
    green = (green * brightness) / 255;
    blue = (blue * brightness) / 255;

    // Combining the scaled components into a new color565 value
    uint16_t scaledColor565 = (red << 11) | (green << 5) | blue;

    return scaledColor565;
  */

  return (((((color565 >> 11) & 0x1F) * (uint16_t)brightness) / 255) << 11) | (((((color565 >> 5) & 0x3F) * (uint16_t)brightness) / 255) << 5) | (((color565 & 0x1F) * (uint16_t)brightness) / 255);
}

void GaugeDraw::DrawImageRLE(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90)
{
  DrawImageRLEBuffer(img, HFlip, VFlip, HPos, VPos, Colour, rotate90, false, NULL /*not used*/, 0 /*not used*/, true);
}

void GaugeDraw::DrawImageRLE(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, bool DrawBlack)
{
  DrawImageRLEBuffer(img, HFlip, VFlip, HPos, VPos, Colour, rotate90, false, NULL /*not used*/, 0 /*not used*/, DrawBlack);
}

void GaugeDraw::DrawImageRLE(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, uint16_t *buff, int buffWidth)
{
  DrawImageRLEBuffer(img, HFlip, VFlip, HPos, VPos, Colour, rotate90, true, buff, buffWidth, true);
}

void GaugeDraw::DrawImageRLE(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, uint16_t *buff, int buffWidth, bool DrawBlack)
{
  DrawImageRLEBuffer(img, HFlip, VFlip, HPos, VPos, Colour, rotate90, true, buff, buffWidth, DrawBlack);
}

void GaugeDraw::DrawImageRLEBuffer(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, bool useBuff, uint16_t *buff, int buffWidth)
{
  DrawImageRLEBuffer(img, HFlip, VFlip, HPos, VPos, Colour, rotate90, true, buff, buffWidth, true);
}

void GaugeDraw::DrawImageRLEBuffer(Image &img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour, bool rotate90, bool useBuff, uint16_t *buff, int buffWidth, bool DrawBlack)
{
  unsigned char pixel;
  unsigned char val;
  unsigned char count;
  int upto = 0;
  int x, y;
  for (int i = 0; i < img.datalen; i++)
  {
    pixel = pgm_read_byte_near(img.data + i);

    if (pixel == 0x64)
    { // Check if we have a special escape char
      pixel = pgm_read_byte_near(img.data + i + 1);
      count = pgm_read_byte_near(img.data + i + 2);
      if (upto / img.width == (upto + count - 1) / img.width)
      {
        if (!rotate90)
        {
          if (useBuff)
          {
            DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), upto % img.width, upto / img.width, (int)count - 1, rotate90, buff, buffWidth, DrawBlack);
          }
          else
          {
            DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), upto % img.width, upto / img.width, (int)count - 1, rotate90, DrawBlack);
          }
        }
        else
        {
          if (useBuff)
          {
            DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), upto / img.width, upto % img.width, (int)count - 1, rotate90, buff, buffWidth, DrawBlack);
          }
          else
          {
            DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), upto / img.width, upto % img.width, (int)count - 1, rotate90, DrawBlack);
          }
        }
      }
      else
      {
        // RLE spans more than one line
        int countbackup2 = count;
        int uptobackup = upto;

        while (upto + count > uptobackup)
        {
          if (countbackup2 < (img.width - (uptobackup % img.width)))
          {
            // Can draw the line as normal
            if (!rotate90)
            {
              if (useBuff)
              {
                DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), uptobackup % img.width, uptobackup / img.width, countbackup2 - 1, rotate90, buff, buffWidth, DrawBlack);
              }
              else
              {
                DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), uptobackup % img.width, uptobackup / img.width, countbackup2 - 1, rotate90, DrawBlack);
              }
            }
            else
            {
              if (useBuff)
              {
                DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), uptobackup / img.width, uptobackup % img.width, countbackup2 - 1, rotate90, buff, buffWidth, DrawBlack);
              }
              else
              {
                DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), uptobackup / img.width, uptobackup % img.width, countbackup2 - 1, rotate90, DrawBlack);
              }
            }
            uptobackup += countbackup2 + 1;
            countbackup2 -= countbackup2;
          }
          else
          {
            // Line too big,need to split
            if (!rotate90)
            {
              if (useBuff)
              {
                DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), uptobackup % img.width, uptobackup / img.width, img.width - (uptobackup) % img.width - 1, rotate90, buff, buffWidth, DrawBlack);
              }
              else
              {
                DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), uptobackup % img.width, uptobackup / img.width, img.width - (uptobackup) % img.width - 1, rotate90, DrawBlack);
              }
            }
            else
            {
              if (useBuff)
              {
                DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), uptobackup / img.width, uptobackup % img.width, img.width - (uptobackup) % img.width - 1, rotate90, buff, buffWidth, DrawBlack);
              }
              else
              {
                DrawLine(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), uptobackup / img.width, uptobackup % img.width, img.width - (uptobackup) % img.width - 1, rotate90, DrawBlack);
              }
            }
            countbackup2 -= img.width - (uptobackup) % img.width;
            uptobackup += img.width - (uptobackup) % img.width;
          }
        }
      }

      upto += count;
      i += 2;
    }
    else
    {
      if (!rotate90)
      {
        if (useBuff)
        {
          DrawPixel(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), upto % img.width, upto / img.width, buff, buffWidth, DrawBlack);
        }
        else
        {
          DrawPixel(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), upto % img.width, upto / img.width, DrawBlack);
        }
      }
      else
      {
        if (useBuff)
        {
          DrawPixel(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), upto / img.width, upto % img.width, buff, buffWidth, DrawBlack);
        }
        else
        {
          DrawPixel(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), upto / img.width, upto % img.width, DrawBlack);
        }
      }
      upto++;
    }
  }
}

void GaugeDraw::DrawImage(Image img, bool HFlip, bool VFlip, int HPos, int VPos, uint16_t Colour)
{
  unsigned char pixel;
  int x, y;

  for (int w = 0; w < img.width; w++)
  {
    for (int h = 0; h < img.height; h++)
    {
      pixel = pgm_read_byte_near(img.data + w + h * img.width);
      DrawPixel(img, HFlip, VFlip, HPos, VPos, Gray2Colour(pixel, Colour), w, h);
    }
  }
}

String GaugeDraw::add127ToString(const String &inputString)
{
  String result = ""; // Initialize an empty string to store the result

  // Iterate through each character in the input string
  for (int i = 0; i < inputString.length(); i++)
  {
    result += (char)(((unsigned char)inputString.charAt(i) + 127) % 256); // Add 127 to each char and append the modified character to the result string
  }

  return result; // Return the modified string
}

int GaugeDraw::CalculateAngle(int fQ, int sQ, int sLo, int sHi, int num)
{
  return map(num, sLo, sHi, fQ * 45, (sQ + 1) * 45);
}

void GaugeDraw::writeFastHLine(unsigned char *buff, uint16_t x, uint16_t y, uint16_t length, uint16_t buffWidth)
{
  for (int i = x; i < x + length; i++)
  {
    buff[i + y * buffWidth] = 0xFF;
  }
  /*    Serial.print(x);
      Serial.print(",");
      Serial.print(y);
      Serial.println("");*/
}
void GaugeDraw::subFastHLine(unsigned char *buff, uint16_t x, uint16_t y, uint16_t length, uint16_t buffWidth, int minX, int minY, int maxX, int maxY)
{
  if ((y <= maxY) && (y >= minY))
  {
    for (int i = x; i < x + length; i++)
    {
      if ((i >= minX) && (i <= maxX))
      {
        if (buff[(i - minX) + (y - minY) * buffWidth] == 0xFF)
        {
          buff[(i - minX) + (y - minY) * buffWidth] = 0x10;
        }
        else
        {
          buff[(i - minX) + (y - minY) * buffWidth] = 0x20;
        }
      }
    }
  }
  /*    Serial.print(x);
      Serial.print(",");
      Serial.print(y);
      Serial.println("");*/
}

void GaugeDraw::_swap_int16_t(int16_t &x0, int16_t &x1)
{
  x0 ^= x1;
  x1 ^= x0;
  x0 ^= x1;
}

void GaugeDraw::fillTriangleBuffer(unsigned char *buff, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int buffWidth, int offx, int offy)
{
  fillTriangleBuffer(buff, x0, y0, x1, y1, x2, y2, buffWidth, 0, offx, offy, 0, 0);
}

void GaugeDraw::fillTriangleBuffer(unsigned char *buff, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int buffWidth, int buffHeight, int offx, int offy, int maxx, int maxy)
{

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1)
  {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }
  if (y1 > y2)
  {
    _swap_int16_t(y2, y1);
    _swap_int16_t(x2, x1);
  }
  if (y0 > y1)
  {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }

  // startWrite();
  if (y0 == y2)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    if (buffHeight > 0)
    {
      subFastHLine(buff, a, y0, b - a + 1, buffWidth, offx, offy, maxx, maxy);
    }
    else
    {
      writeFastHLine(buff, a - offx, y0 - offy, b - a + 1, buffWidth);
    }
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++)
  {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    if (a > b)
      _swap_int16_t(a, b);
    if (buffHeight > 0)
    {
      subFastHLine(buff, a, y, b - a + 1, buffWidth, offx, offy, maxx, maxy);
    }
    else
    {
      writeFastHLine(buff, a - offx, y - offy, b - a + 1, buffWidth);
    }
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++)
  {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    if (a > b)
      _swap_int16_t(a, b);
    if (buffHeight > 0)
    {
      subFastHLine(buff, a, y, b - a + 1, buffWidth, offx, offy, maxx, maxy);
    }
    else
    {
      writeFastHLine(buff, a - offx, y - offy, b - a + 1, buffWidth);
    }
  }
}

uint16_t GaugeDraw::rgb565Color(unsigned char r, unsigned char g, unsigned char b)
{
  // Ensure r, g, and b are within the valid range (0-255)
  r = (r > 255) ? 255 : (r < 0) ? 0
                                : r;
  g = (g > 255) ? 255 : (g < 0) ? 0
                                : g;
  b = (b > 255) ? 255 : (b < 0) ? 0
                                : b;

  // Convert RGB888 to RGB565
  uint16_t color565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

  return color565;
}

void GaugeDraw::drawNeedle(int centerX, int centerY, int innerRadius, int outerRadius, float angleDeg, int width, uint16_t color)
{
  drawNeedle(centerX, centerY, innerRadius, outerRadius, angleDeg, width, color, 0);
}

// Function to draw the needle
void GaugeDraw::drawNeedle(int centerX, int centerY, int innerRadius, int outerRadius, float angleDeg, int width, uint16_t color, uint16_t bgColor)
{
  // Convert angle from degrees to radians
  float angleRad = radians(angleDeg);

  // Calculate the coordinates of the first two points on the center circle
  int point1X = centerX + innerRadius * cos(angleRad);
  int point1Y = centerY + innerRadius * sin(angleRad);
  int point2X = centerX - innerRadius * cos(angleRad);
  int point2Y = centerY - innerRadius * sin(angleRad);

  // Calculate the coordinates of the midpoint of the base of the rhombus
  int midpointX = centerX + outerRadius * cos(angleRad + PI / 2);
  int midpointY = centerY + outerRadius * sin(angleRad + PI / 2);

  // Calculate the coordinates of the final two points
  int point3X = midpointX + width / 2 * cos(angleRad);
  int point3Y = midpointY + width / 2 * sin(angleRad);
  int point4X = midpointX - width / 2 * cos(angleRad);
  int point4Y = midpointY - width / 2 * sin(angleRad);

  // Clear previous triangle.
  // p_tft->fillTriangle(prevX1, prevY1, prevX2, prevY2, prevX3, prevY3, 0x0);
  // p_tft->fillTriangle(prevX2, prevY2, prevX3, prevY3, prevX4, prevY4, 0x0);

  // Draw the shaded area with triangles
  p_tft->fillTriangle(point1X, point1Y, point2X, point2Y, point3X, point3Y, color);
  p_tft->fillTriangle(point2X, point2Y, point3X, point3Y, point4X, point4Y, color);

  // Clear areas occupied by the previous rhombus but not the current one
  clearNonOverlapAreas(point1X, point1Y, point2X, point2Y, point3X, point3Y, point4X, point4Y, 0x0);

  // Draw the centre circle
  p_tft->fillCircle(centerX, centerY, innerRadius, color);

  // Update the previous needle coordinates
  prevX1 = point1X;
  prevY1 = point1Y;
  prevX2 = point2X;
  prevY2 = point2Y;
  prevX3 = point3X;
  prevY3 = point3Y;
  prevX4 = point4X;
  prevY4 = point4Y;
}

// Function to clear areas occupied by the previous rhombus but not the current one
void GaugeDraw::clearNonOverlapAreas(int point1X, int point1Y, int point2X, int point2Y, int point3X, int point3Y, int point4X, int point4Y, uint16_t bgColor)
{
  // Find the bounding box of the two rhombuses
  int minX = min(min(prevX1, prevX2), min(prevX3, prevX4));
  int maxX = max(max(prevX1, prevX2), max(prevX3, prevX4));
  int minY = min(min(prevY1, prevY2), min(prevY3, prevY4));
  int maxY = max(max(prevY1, prevY2), max(prevY3, prevY4));

  int width = maxX - minX + 2;
  int height = maxY - minY + 2;

  // Create a buffer to store the bitmap
  unsigned char *buff = (unsigned char *)p_DisplayBuffer; // new unsigned char [width * height];

  // Initialize the buffer with 0x00 (unfilled)
  for (int i = 0; i < width * (height); i++)
  {
    buff[i] = 0x00;
  }

  fillTriangleBuffer(buff, prevX1, prevY1, prevX2, prevY2, prevX3, prevY3, width, minX, minY);
  fillTriangleBuffer(buff, prevX2, prevY2, prevX3, prevY3, prevX4, prevY4, width, minX, minY);
  fillTriangleBuffer(buff, point1X, point1Y, point2X, point2Y, point3X, point3Y, width, height, minX, minY, maxX, maxY);
  fillTriangleBuffer(buff, point2X, point2Y, point3X, point3Y, point4X, point4Y, width, height, minX, minY, maxX, maxY);

  // Iterate over the bounding box of the previous rhombus
  for (int x = minX; x < maxX + 1; x++)
  {
    for (int y = minY; y < maxY + 1; y++)
    {
      if (buff[(x - minX) + (y - minY) * width] == 0xFF)
      {
        p_tft->drawPixel(x, y, bgColor);
      }
    }
  }
  // delete [] buff;
}

int GaugeDraw::DrawLargeNumber(int number, uint16_t colour, uint16_t *buff, int buffWidth)
{
  return DrawLargeNumberBuffer(number, 0, 0, colour, 0, true, buff, buffWidth);
}

int GaugeDraw::DrawLargeNumber(int number, int x, int y, uint16_t colour)
{
  return DrawLargeNumberBuffer(number, x, y, colour, 0, false, NULL, 0);
}

int GaugeDraw::DrawLargeNumber(int number, int x, int y, uint16_t colour, int Space)
{
  return DrawLargeNumberBuffer(number, x, y, colour, Space, false, NULL, 0);
}

int GaugeDraw::DrawLargeNumber(int number, int x, int y, uint16_t colour, int Space, uint16_t *buff, int buffWidth)
{
  return DrawLargeNumberBuffer(number, x, y, colour, Space, true, buff, buffWidth);
}

int GaugeDraw::DrawLargeNumberBuffer(int number, int x, int y, uint16_t colour, int Space, bool useBuff, uint16_t *buff, int buffWidth)
{
  return DrawTextBuffer(String(number), x, y, colour, Space, useBuff, buff, buffWidth, 0);
}

int GaugeDraw::DrawMed(String message, uint16_t colour, uint16_t *buff, int buffWidth)
{
  return DrawMedBuffer(message, 0, 0, colour, 0, true, buff, buffWidth);
}

int GaugeDraw::DrawMed(String message, int x, int y, uint16_t colour)
{
  return DrawMedBuffer(message, x, y, colour, 0, false, NULL, 0);
}

int GaugeDraw::DrawMed(String message, int x, int y, uint16_t colour, int Space)
{
  return DrawMedBuffer(message, x, y, colour, Space, false, NULL, 0);
}

int GaugeDraw::DrawMed(String message, int x, int y, uint16_t colour, int Space, uint16_t *buff, int buffWidth)
{
  return DrawMedBuffer(message, x, y, colour, Space, true, buff, buffWidth);
}

int GaugeDraw::DrawMed(String message, int x, int y, uint16_t colour, uint16_t *buff, int buffWidth)
{
  return DrawMedBuffer(message, x, y, colour, 0, true, buff, buffWidth);
}

// Have to buffer this number and compare to previous number so that only changed pixels are drawn
int GaugeDraw::DrawMedBuffer(String message, int x, int y, uint16_t colour, int Space, bool useBuff, uint16_t *buff, int buffWidth)
{
  return DrawTextBuffer(message, x, y, colour, Space, useBuff, buff, buffWidth, 1);
}

int GaugeDraw::DrawSmall(String message, uint16_t colour, uint16_t *buff, int buffWidth)
{
  return DrawSmallBuffer(message, 0, 0, colour, 0, true, buff, buffWidth);
}

int GaugeDraw::DrawSmall(String message, int x, int y, uint16_t colour)
{
  return DrawSmallBuffer(message, x, y, colour, 0, false, NULL, 0);
}

int GaugeDraw::DrawSmall(String message, int x, int y, uint16_t colour, int Space)
{
  return DrawSmallBuffer(message, x, y, colour, Space, false, NULL, 0);
}

int GaugeDraw::DrawSmall(String message, int x, int y, uint16_t colour, int Space, uint16_t *buff, int buffWidth)
{
  return DrawSmallBuffer(message, x, y, colour, Space, true, buff, buffWidth);
}

int GaugeDraw::DrawSmall(String message, int x, int y, uint16_t colour, uint16_t *buff, int buffWidth)
{
  return DrawSmallBuffer(message, x, y, colour, 0, true, buff, buffWidth);
}

// Have to buffer this number and compare to previous number so that only changed pixels are drawn
int GaugeDraw::DrawSmallBuffer(String message, int x, int y, uint16_t colour, int Space, bool useBuff, uint16_t *buff, int buffWidth)
{
  return DrawTextBuffer(message, x, y, colour, Space, useBuff, buff, buffWidth, 2);
}

int GaugeDraw::DrawTextBuffer(String message, int x, int y, uint16_t colour, int Space, bool useBuff, uint16_t *buff, int buffWidth, int PixelSize)
{
  int Digit = 0;
  int MaxDigit = *(maxDigitArray[PixelSize]);

  for (int i = 0; i < message.length(); i++)
  {
    bool found=false;
    Digit = (unsigned char)message.c_str()[i];
    for (int j=0;j<MaxDigit;j++) {
      if((*(bitmapArrays[PixelSize][j])).charVal==Digit) {
        found=true;
        Digit=j;
        break;
      }
    }
    if (!found) {
      Digit=0;
    }

    if (useBuff)
    {
      DrawImageRLE((*(bitmapArrays[PixelSize][Digit])), false, false, Space, 0, colour, false, buff, buffWidth);
    }
    else
    {
      DrawImageRLE((*(bitmapArrays[PixelSize][Digit])), false, false, x + Space, y, colour, false);
    }
    Space += bitmapArrays[PixelSize][Digit]->width + 2;
  }
  return Space;
}
