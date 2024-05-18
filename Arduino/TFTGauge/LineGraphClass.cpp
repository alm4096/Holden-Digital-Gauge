#include "LineGraphClass.h"


void LineGraph::Setup(int l_height, int l_width, int l_boxshiftx, int l_boxshifty, uint16_t l_colour, uint16_t l_colour2, double l_ScaleLow, double l_ScaleHigh, int l_dp, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph, bool l_Scrolling, int l_UpdateInterval) {
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
  m_Scrolling=l_Scrolling;
  m_intervalG=l_UpdateInterval;
}

void LineGraph::Setup(uint16_t l_colour, double l_ScaleLow, double l_ScaleHigh, int l_dp, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph, bool l_Scrolling) {
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
  m_Scrolling=l_Scrolling;
}

void LineGraph::Setup(uint16_t l_colour, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph) {
  m_Name1= l_Name1;
  m_Unit1=l_Unit1;
  m_colour=l_colour;
  m_colour2=l_colour;
  m_applyMovingAverage=l_average;
  m_period=l_period;
  m_drawNumber=l_drawNumber;
  m_drawBarGraph=l_drawBarGraph;
}
void LineGraph::Setup(uint16_t l_colour, String l_Name1, String l_Unit1, bool l_drawNumber, bool l_drawBarGraph) {
  m_Name1= l_Name1;
  m_Unit1=l_Unit1;
  m_colour=l_colour;
  m_colour2=l_colour;
  m_drawNumber=l_drawNumber;
  m_drawBarGraph=l_drawBarGraph;
}

void LineGraph::Setup(uint16_t l_colour, String l_Name1, String l_Unit1) {
  m_Name1= l_Name1;
  m_Unit1=l_Unit1;
  m_colour=l_colour;
  m_colour2=l_colour;
}


void LineGraph::DrawGauge(int l_number) {
  DrawGauge(m_xPos,m_yPos,l_number);
}
void LineGraph::DrawGauge(double l_number) {
  DrawGauge(m_xPos,m_yPos,l_number);
}
void LineGraph::DrawGauge(double l_number, int l_numberStr) {
  DrawGauge(m_xPos,m_yPos,l_number,l_numberStr);
}
void LineGraph::DrawGauge(int l_x,int l_y, int l_number) {
  DrawLineGraph(l_x, l_y, (double)l_number, 0);
} 
void LineGraph::DrawGauge(int l_x,int l_y, double l_number) {
  DrawLineGraph(l_x, l_y, l_number, m_dp);
} 
void LineGraph::DrawGauge(int l_x,int l_y, double l_number, int l_numberStr) {
  DrawLineGraph(l_x, l_y, l_number, l_numberStr);
}

void LineGraph::DrawLineGraph(int l_x,int l_y, double l_number, int l_decimalPlaces) {

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

  int TextHeight=8+2;
  int l_space=0;
  int m_buffWidth=60;

  if (!m_init) {
    //Set database to zero
    for (int j =0;j<m_PastValsSize;j++){
      m_PastVals[j]=m_height+TextHeight-2;
    }
    
    //Draw Name and Units
    l_space = DrawSmall(m_Name1, l_x, l_y+1, m_colour);
    if (m_boxshiftx<l_space) m_boxshiftx=l_space+2;
    l_space = DrawSmall(m_Unit1, l_x, l_y+m_height+2, m_colour);
    if (m_boxshiftx<l_space) m_boxshiftx=l_space+2;

    //Draw Box Outline
    p_tft->drawLine(l_x+m_boxshiftx, l_y+m_boxshifty, l_x+m_boxshiftx+m_width, l_y+m_boxshifty, m_colour2);
    p_tft->drawLine(l_x+m_boxshiftx, l_y+m_boxshifty+m_height+TextHeight, l_x+m_boxshiftx+m_width, l_y+m_boxshifty+m_height+TextHeight, m_colour2);
    p_tft->drawLine(l_x+m_boxshiftx, l_y+m_boxshifty+m_height+TextHeight, l_x+m_boxshiftx, l_y+m_boxshifty, m_colour);
    p_tft->drawLine(l_x+m_boxshiftx+m_width, l_y+m_boxshifty, l_x+m_boxshiftx+m_width, l_y+m_boxshifty+m_height+TextHeight, m_colour2);
    m_init=true;
  }

  //Only Update at speed required
  if ((m_drawBarGraph)&&(l_currentMillis - m_previousMillisG >= m_intervalG)) {

	  //Draw Graph
    m_previousMillisG = l_currentMillis;

    //Calc new value
    int l_ValueShift=0;
    int l_barHeight = (m_height+TextHeight-2);
    m_PastVals[m_PastValsIndex]=(l_barHeight-int(double(double(l_barHeight)*l_number/(m_ScaleHigh-m_ScaleLow))));
    if (m_PastVals[m_PastValsIndex]<0) {
      m_PastVals[m_PastValsIndex]=0;
    }
    if (m_PastVals[m_PastValsIndex]>l_barHeight) {
      m_PastVals[m_PastValsIndex]=l_barHeight;
    }
    l_barHeight++;

    //Draw Graph
    if (m_Scrolling) {
      //Move graph
      int k=0;
      int m=0;
      for (int j=0;j<m_width-2;j++) {
        k=(m_PastValsIndex-j);
        if (k<0) {k=k+m_PastValsSize;}
        m=k-1;
        if (m<0) {m=m+m_PastValsSize;}
        
        //Blank line we are up to
        p_tft->drawLine(l_x+m_boxshiftx+j+1, l_y+m_boxshifty+m_height+TextHeight-1, l_x+m_boxshiftx+j+1, l_y+m_boxshifty+1, 0);
 
        //Draw line
        if (j==0) {
          //If we at 0 then just draw a pixel
          p_tft->drawPixel(l_x+m_boxshiftx+j+1,l_y+m_boxshifty+m_PastVals[k],m_colour);
        }
        else {
          //Else draw a line
          p_tft->drawLine(l_x+m_boxshiftx+j,l_y+m_boxshifty+m_PastVals[k],l_x+m_boxshiftx+j+1,l_y+m_boxshifty+m_PastVals[m],m_colour);
        }
      }
    }
    else { //Blank Graph
      //Draw GRAPH HERE
      int k=m_PastValsIndex-1;
      if (k<0) {k=k+m_PastValsSize;}

      int m=m_upToX+8;
      if (m>m_width-2) {m=m-(m_width-2);}

      //Blank line so we can see where the graph is at
      p_tft->drawLine(l_x+m_boxshiftx+m, l_y+m_boxshifty+m_height+TextHeight-1, l_x+m_boxshiftx+m, l_y+m_boxshifty+1, 0);

      //Make it draw a line instead
      if (m_upToX==0) {
        //First dot
        p_tft->drawPixel(l_x+m_boxshiftx+1+m_upToX,l_y+m_boxshifty+m_PastVals[m_PastValsIndex],m_colour);
      }
      else {
        //Draw a line
        p_tft->drawLine(l_x+m_boxshiftx+1+m_upToX,l_y+m_boxshifty+m_PastVals[k],l_x+m_boxshiftx+1+m_upToX,l_y+m_boxshifty+m_PastVals[m_PastValsIndex],m_colour);
      }

      m_upToX++;
      if (m_upToX>= m_width-2) {
        m_upToX=0;
      }
    }
    m_PastValsIndex=(m_PastValsIndex+1)%m_PastValsSize;
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
}
