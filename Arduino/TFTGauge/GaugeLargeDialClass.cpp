#include "GaugeLargeDialClass.h"


void LargeDialGauge::DrawGauge(int x, int y, uint16_t colour, int val, int StartOct, int FinishOct) {
  //In order starting from 6 o'clock
  //Large Ticks 
  for (int i=StartOct;i<=FinishOct;i++){
    DrawImageRLE(SpeedoLargeTicks_Img,pgm_read_byte_near(Rotations+0+i*3),pgm_read_byte_near(Rotations+1+i*3),x+pgm_read_byte_near(LargeTickPairs+0+i*2),y+pgm_read_byte_near(LargeTickPairs+1+i*2),colour,pgm_read_byte_near(Rotations+2+i*3), false); 
    DrawImageRLE(SpeedoSmallTicks_Img,pgm_read_byte_near(Rotations+0+i*3),pgm_read_byte_near(Rotations+1+i*3),x+pgm_read_byte_near(SmallTickPairs+0+i*2),y+pgm_read_byte_near(SmallTickPairs+1+i*2),colour,pgm_read_byte_near(Rotations+2+i*3), false); 
  }
}

void LargeDialGauge::DrawFullGauge(double Number){
  DrawFullGauge(xStor, yStor, colourStor, Number, firstQuadStor, secondQuadStor);
}

void LargeDialGauge::DrawFullGauge(int x, int y, uint16_t Colour, double Number){
  DrawFullGauge(x, y, Colour, Number, firstQuadStor, secondQuadStor);
}

void LargeDialGauge::DrawFullGauge(int x, int y, uint16_t Colour, double Number, int firstQuad, int secondQuad){
  DrawFullGauge(x, y, Colour, Number, firstQuad, secondQuad, ScaleLowStor, ScaleHighStor, largeTickLabelsStor);
}

void LargeDialGauge::DrawFullGauge(int x, int y, uint16_t Colour, double Number, int firstQuad, int secondQuad,int ScaleLow,int ScaleHigh,std::vector<String> &largeTickLabels){
  
  if (applyMovingAverage) {
    avgValue=(Number+avgValue*period)/(period+1);
    Number=avgValue;
  }
  else if(m_lastNumber==Number) {
    return; //Same number, return
  }
  
  //Nothing to do, no updates required
  if (avgValue==m_lastNumber){
    return;
  }
  
  m_lastNumber=Number;

  // Get the current time
  unsigned long l_currentMillis = millis();

  // Check if 1/24th of a second has passed
  if (l_currentMillis - m_previousMillis >= m_interval) {
    // Save the current time for the next comparison
    m_previousMillis = l_currentMillis;

  }
  else {
    //Calling draw too fast, limit refresh rate
    return;
  }


  //Init Gauge
  if (!initGauge) {
    //Draws the gauge background
    DrawGauge(x,y,Colour,100,0,5);

    initGauge=true;
    firstQuadStor=firstQuad;
    secondQuadStor=secondQuad;
    xStor=x;
    yStor=y;
    ScaleLowStor=ScaleLow;
    ScaleHighStor=ScaleHigh;
    colourStor=Colour;
    largeTickLabelsStor=largeTickLabels;
    avgValue=Number;

    //Draw Tick Numbers
    int Space = 0;
    int buffWidth=20*3;
    for (int i=0;i<(secondQuadStor-firstQuadStor)*2;i++){
      memset(p_DisplayBuffer, 0x0, buffWidth*9 * sizeof(uint16_t));
      String Temp;
      if (i<largeTickLabelsStor.size()) {
        Temp=largeTickLabelsStor.at(i);
      }
      else {
        Temp="1k";
      }
      Space = DrawSmall(Temp,Colour,p_DisplayBuffer, buffWidth);

      /*
      Debug
      Temp = String(Space);
      DrawMed(Temp,0,kl,Colour);
      kl+=9;
      */

      //Inefficient but works:      
      uint16_t * HoldBuff = p_TempBuff;
      p_TempBuff = (uint16_t*)realloc(p_TempBuff, (22*Space) * sizeof(uint16_t));
      if (p_TempBuff==NULL) {
        p_TempBuff=HoldBuff; //realloc failed, copy pointer back
      } 
      if (Space<buffWidth) {
        //Resize buffer to fit contents
        for (int col=0;col<Space;col++) {
          for (int row=0;row<9;row++){
            p_TempBuff[col+row*Space]=p_DisplayBuffer[col+row*buffWidth];
          }
        }
        buffWidth=Space;
      }
      
      if ((i==0)||(i==6)) {Space=-Space/2;}
      if ((i>=1)&&(i<=5)) {Space=0;}
      if ((i>=7)&&(i<=12)) {Space=-Space;}
      p_tft->drawRGBBitmap(pgm_read_byte_near(TickLabelPairs+i*2)+x+Space, pgm_read_byte_near(TickLabelPairs+i*2+1)+y, p_TempBuff, buffWidth, 9);
      buffWidth=20*3;
      //delete[]p_TempBuff;
    }  
  }


  //Draw Large Number
  int buffWidth=50*3;
  memset(p_DisplayBuffer, 0x0, buffWidth*28 * sizeof(uint16_t));
  DrawLargeNumber(Number,Colour,p_DisplayBuffer,buffWidth);
  p_tft->drawRGBBitmap(x+140, y+160, p_DisplayBuffer, buffWidth, 28);

  //Draw Needle
  //kl++;
  int centerX = 240/2+x; // Center X coordinate of the screen
  int centerY = 240/2+y; // Center Y coordinate of the screen
  int innerRadius = 5; // Radius of the inner circle
  int outerRadius = 50; // Radius of the outer circle
  int tipWidth = 5;

  // Draw the needle
  drawNeedle(centerX, centerY, innerRadius, outerRadius, CalculateAngle(firstQuadStor,secondQuadStor,ScaleLowStor,ScaleHighStor,Number), tipWidth, Colour);

  //if (kl>3600) {kl=-1;}
}

void LargeDialGauge::DrawGauge(double number) {
//-----------
//DRAW FULL SIZE GAUGE
//-----------
  if (initGauge) {
    DrawFullGauge(40,0,rgb565Color(0,188,232),number);
  }
  else {
    //Init Gauge
    std::vector<String> largeTickLabels;
    largeTickLabels.push_back("0");
    largeTickLabels.push_back("1");
    largeTickLabels.push_back("2");
    largeTickLabels.push_back("3");
    largeTickLabels.push_back("4");
    largeTickLabels.push_back("5");
    largeTickLabels.push_back("6");
    largeTickLabels.push_back("7");
    largeTickLabels.push_back("8");
    largeTickLabels.push_back("9");
    largeTickLabels.push_back("10");
    DrawFullGauge(40,0,rgb565Color(0,188,232),number,0,5,0,900,largeTickLabels);
  }	
}


void LargeDialGauge::DrawGauge(int x,int y, int number){} // Pure virtual function, redefine 
void LargeDialGauge::DrawGauge(int x,int y, double number){} // Pure virtual function
void LargeDialGauge::DrawGauge(int x,int y, double number, int decimalPlaces){} // Pure virtual function
void LargeDialGauge::DrawGauge(int number){} // Pure virtual function
void LargeDialGauge::DrawGauge(double number, int decimalPlaces){} // Pure virtual function
