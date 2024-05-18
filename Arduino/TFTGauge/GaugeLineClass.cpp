#include "GaugeLineClass.h"


void LineGauge::Setup(int l_height, int l_width, int l_boxshiftx, int l_boxshifty, uint16_t l_colour, uint16_t l_colour2, double l_ScaleLow, double l_ScaleHigh, int l_dp, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph) {
  m_boxshiftx=l_boxshiftx;
  m_boxshifty=l_boxshifty;
  m_height = l_height;
  m_width = l_width;
  m_ScaleLow = l_ScaleLow;
  m_ScaleHigh = l_ScaleHigh;
  m_Name1= l_Name1;
  m_Unit1=l_Unit1;
  m_colour=l_colour;
  m_colour2=l_colour2;
  m_applyMovingAverage=l_average;
  m_period=l_period;
  m_dp=l_dp;
  m_drawNumber=l_drawNumber;
  m_drawBarGraph=l_drawBarGraph;
}

void LineGauge::Setup(uint16_t l_colour, double l_ScaleLow, double l_ScaleHigh, int l_dp, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph) {
  m_ScaleLow = l_ScaleLow;
  m_ScaleHigh = l_ScaleHigh;
  m_Name1= l_Name1;
  m_Unit1=l_Unit1;
  m_colour=l_colour;
  m_colour2=l_colour;
  m_applyMovingAverage=l_average;
  m_period=l_period;
  m_drawNumber=l_drawNumber;
  m_drawBarGraph=l_drawBarGraph;
}

void LineGauge::Setup(uint16_t l_colour, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph) {
  m_Name1= l_Name1;
  m_Unit1=l_Unit1;
  m_colour=l_colour;
  m_colour2=l_colour;
  m_applyMovingAverage=l_average;
  m_period=l_period;
  m_drawNumber=l_drawNumber;
  m_drawBarGraph=l_drawBarGraph;
}
void LineGauge::Setup(uint16_t l_colour, String l_Name1, String l_Unit1, bool l_drawNumber, bool l_drawBarGraph) {
  m_Name1= l_Name1;
  m_Unit1=l_Unit1;
  m_colour=l_colour;
  m_colour2=l_colour;
  m_drawNumber=l_drawNumber;
  m_drawBarGraph=l_drawBarGraph;
}

void LineGauge::Setup(uint16_t l_colour, String l_Name1, String l_Unit1) {
  m_Name1= l_Name1;
  m_Unit1=l_Unit1;
  m_colour=l_colour;
  m_colour2=l_colour;
}


void LineGauge::DrawGauge(int l_number) {
  DrawGauge(m_xPos,m_yPos,l_number);
}
void LineGauge::DrawGauge(double l_number) {
  DrawGauge(m_xPos,m_yPos,l_number);
}
void LineGauge::DrawGauge(double l_number, int l_numberStr) {
  DrawGauge(m_xPos,m_yPos,l_number,l_numberStr);
}
void LineGauge::DrawGauge(int l_x,int l_y, int l_number) {
  DrawLineGauge(l_x, l_y, (double)l_number, 0);
} 
void LineGauge::DrawGauge(int l_x,int l_y, double l_number) {
  DrawLineGauge(l_x, l_y, l_number, m_dp);
} 
void LineGauge::DrawGauge(int l_x,int l_y, double l_number, int l_numberStr) {
  DrawLineGauge(l_x, l_y, l_number, l_numberStr);
}

void LineGauge::DrawLineGauge(int l_x,int l_y, double l_number, int l_decimalPlaces) {

  //---------------------
  //Performance Check
  //---------------------
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

  //---------------------
  //Number Check
  //---------------------
  //Smooth incoming number if requested to
  if (m_applyMovingAverage) {
    m_avgValue=(l_number+m_avgValue*m_period)/(m_period+1);
    l_number=m_avgValue;
  }
  else if (m_lastNumber==l_number) {
      return; //Same number, return
  }
  //Nothing to do, no updates required
  if (m_avgValue==m_lastNumber){
    return;
  }
  m_lastNumber=l_number;

  //---------------------
  //DRAW SMALL LINE GAUGE
  //---------------------
  String l_numberStr;
  if (l_decimalPlaces==0){ //Stops weird stuff happening
    l_numberStr=String((int)l_number);
  }
  else{
    l_numberStr=String(l_number,l_decimalPlaces);
  }

  int l_space=0;
  
  if (!m_init) {
    l_space = DrawSmall(m_Name1, l_x, l_y+1, m_colour);
    //Draw Name and Units
    if (m_boxshiftx<l_space) m_boxshiftx=l_space+2;
    l_space = DrawSmall(m_Unit1, l_x, l_y+m_height+2, m_colour);
    if (m_boxshiftx<l_space) m_boxshiftx=l_space+2;
  }

  int m_buffWidth=60;
  if (m_drawBarGraph) {
    if (!m_init) {

      //Draw Box Outline
      p_tft->drawLine(l_x+m_boxshiftx, l_y+m_boxshifty, l_x+m_boxshiftx+m_width, l_y+m_boxshifty, m_colour2);
      p_tft->drawLine(l_x+m_boxshiftx, l_y+m_boxshifty+m_height, l_x+m_boxshiftx+m_width, l_y+m_boxshifty+m_height, m_colour2);
      p_tft->drawLine(l_x+m_boxshiftx, l_y+m_boxshifty+m_height, l_x+m_boxshiftx, l_y+m_boxshifty, m_colour);
      p_tft->drawLine(l_x+m_boxshiftx+m_width, l_y+m_boxshifty, l_x+m_boxshiftx+m_width, l_y+m_boxshifty+m_height, m_colour2);

      //Draw Lower Numbers
      if (m_ScaleLow==0.00) { //Fix 0 being written as .0
        DrawSmall(String("0"), l_x+m_boxshiftx, l_y+m_height+2, m_colour);
      }
      else {
        if (m_dp==0) {
          DrawSmall(String((int)m_ScaleLow), l_x+m_boxshiftx, l_y+m_height+2, m_colour);
        }
        else {
          DrawSmall(String(m_ScaleLow,m_dp), l_x+m_boxshiftx, l_y+m_height+2, m_colour);
        }
      }
      memset(p_DisplayBuffer,0,sizeof(uint16_t)*m_buffWidth*9);
      if (m_dp==0) {
        l_space = DrawSmall(String((int)m_ScaleHigh), m_colour,p_DisplayBuffer,m_buffWidth);
      }
      else{
        l_space = DrawSmall(String(m_ScaleHigh,m_dp), m_colour,p_DisplayBuffer,m_buffWidth);
      }
      //Resize display buffer so it doesn't affect the large number
      //Inefficient but works:      
      if (l_space<m_buffWidth) {
        //Resize buffer to fit contents
        for (int l_col=0;l_col<l_space;l_col++) {
          for (int l_row=0;l_row<9;l_row++){
            p_TempBuff[l_col+l_row*l_space]=p_DisplayBuffer[l_col+l_row*m_buffWidth];
          }
        }
        p_tft->drawRGBBitmap(l_x+m_boxshiftx+m_width-l_space, l_y+m_height+2, p_TempBuff, l_space, 9);
      }
      else{
        p_tft->drawRGBBitmap(l_x+m_boxshiftx+m_width-l_space, l_y+m_height+2, p_DisplayBuffer, m_buffWidth, 9);
      }  
    }

    //Draw Bar:
    int l_val=100;
    l_val = (int)mapDouble(l_number, m_ScaleLow, m_ScaleHigh, 0.00, (double)(m_width-1));
    if (l_val>m_width-1) {l_val=m_width-1;}
    if (l_val<0) {l_val=0;}
    p_tft->fillRect(l_x+m_boxshiftx+1, l_y+m_boxshifty+1, l_val, m_height-1, m_colour);
    if (l_val!=m_width-1) {
      p_tft->fillRect(l_x+m_boxshiftx+1+l_val, l_y+m_boxshifty+1, m_width-2-l_val, m_height-1, 0x00);
    }
  }

  if (m_drawNumber) {
    //Draw Large Number at end
    m_buffWidth=41*4;
    memset(p_DisplayBuffer,0,sizeof(uint16_t)*m_buffWidth*22);

    l_space=-1;
    l_space = DrawMed(l_numberStr, m_colour, p_DisplayBuffer, m_buffWidth);
    
    if ((l_space<m_buffWidth)&&(l_space>0)) {
      //Resize buffer to fit contents
      for (int l_col=0;l_col<l_space;l_col++) {
        for (int l_row=0;l_row<22;l_row++){
          p_TempBuff[l_col+l_row*l_space]=p_DisplayBuffer[l_col+l_row*m_buffWidth];
        }
      }
      if (m_drawBarGraph) {
        p_tft->drawRGBBitmap(l_x+m_boxshiftx+m_width+2, l_y, p_TempBuff, l_space, 22);    
        p_tft->fillRect(l_x+m_boxshiftx+m_width+2+l_space, l_y, m_buffWidth-l_space, 22, 0);
      }
      else {
        p_tft->drawRGBBitmap(l_x+m_boxshiftx, l_y, p_TempBuff, l_space, 22);    
        p_tft->fillRect(l_x+m_boxshiftx+l_space, l_y, m_buffWidth-l_space, 22, 0);
      }

    }
    else {
      if (m_drawBarGraph) {
        p_tft->drawRGBBitmap(l_x+m_boxshiftx+m_width+2, l_y, p_DisplayBuffer, m_buffWidth, 22);
      }
      else {
        p_tft->drawRGBBitmap(l_x+m_boxshiftx, l_y, p_DisplayBuffer, m_buffWidth, 22);
      }
    }
  }
  
  m_init=true;
}
