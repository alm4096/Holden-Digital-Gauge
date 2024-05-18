#ifndef GRAPHLINECLASSH
#define GRAPHLINECLASSH
#include "GaugeDrawClass.h"

class LineGraph : public GaugeDraw {
private:
  int m_colour=rgb565Color(0,188,232), m_colour2=rgb565Color(0,200,200);
  int m_boxshiftx=70,m_boxshifty=0, m_height=10, m_width=100, m_dp=0;
  double m_ScaleLow=0.0, m_ScaleHigh=200.0;
  double m_avgValue=0, m_period=1;
  String m_Name1=String("Temp"), m_Unit1 = String("\bC");
  bool m_applyMovingAverage=true;
  bool m_drawNumber=true;
  bool m_drawBarGraph=true;
  int m_upToX=0;
  int m_PastValsSize=320;
  int m_PastVals[320];
  int m_PastValsIndex=0;
  bool m_init=false;
  bool m_Scrolling=false;  

  //---------------------
  //Performance Section
  //---------------------
  //Only update 24fps max
  unsigned long m_previousMillis = 0;   // Stores the last time the function was called
  long m_interval = 41;           // Interval for 1/24th of a second (in milliseconds)
  double m_lastNumber=-9999.0;

  long m_intervalG = 1000;           // Interval for 1/24th of a second (in milliseconds)
  unsigned long m_previousMillisG = 0;   // Stores the last time the function was called
  //---------------------

  void DrawLineGraph(int l_x,int l_y, double l_number, int l_decimalPlaces);

public:

/**
 * Setup the line gauge
 * 
 * @param height height of the bar graph (total gauge height will be approx 2x this).
 * @param width width of the bar graph.
 * @param boxshiftx how far with shift the bar graph relative to x (if not enough space for the name it will be shifted).
 * @param boxshifty how far with shift the bar graph relative to y.
 * @param colour 565 colour to use for gauge.
 * @param colour2 565 colour to use for border of bar graph.
 * @param ScaleLow Lower scale.
 * @param ScaleHigh Upper scale.
 * @param DP Decimal Places for Scale.
 * @param Name Name to display to the left of the bar graph.
 * @param Units Units to display under the name.
 * @param Average Whether or not to apply a moving average filter to the number.
 * @param Period How long the moving average is calculated over.
 * @param DrawNumber Bool on whether to draw the number after the graph or not.
 * @param drawBarGraph Bool on whether to draw the bar graph or not.
 */
  void Setup(int l_height, int l_width, int l_boxshiftx, int l_boxshifty, uint16_t l_colour, uint16_t l_colour2, double l_ScaleLow, double l_ScaleHigh, int l_dp, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph, bool l_Scrolling, int l_UpdateInterval);
  void Setup(uint16_t l_colour, double l_ScaleLow, double l_ScaleHigh, int l_dp, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph, bool l_Scrolling);
  void Setup(uint16_t l_colour, String l_Name1, String l_Unit1, bool l_average, double l_period, bool l_drawNumber, bool l_drawBarGraph);
  void Setup(uint16_t l_colour, String l_Name1, String l_Unit1, bool l_drawNumber, bool l_drawBarGraph);
  void Setup(uint16_t l_colour, String l_Name1, String l_Unit1);

/**
 * Draws a line gauge
 * 
 * @param x x coordinate of gauge.
 * @param y y coordinate of gauge.
 * @param number number to display.
 * @param decimalplaces number of decimal places (optional, assumed same as scale if not given).
 */
  void DrawGauge(int l_x,int l_y, int l_number); 
  void DrawGauge(int l_number); 

/**
 * Draws a line gauge
 * 
 * @param x x coordinate of gauge.
 * @param y y coordinate of gauge.
 * @param number number to display.
 * @param decimalplaces number of decimal places (optional, assumed same as scale if not given).
 */
  void DrawGauge(int l_x,int l_y, double l_number); 
  void DrawGauge(double l_number); 

/**
 * Draws a line gauge
 * 
 * @param x x coordinate of gauge.
 * @param y y coordinate of gauge.
 * @param number number to display.
 * @param decimalplaces number of decimal places (optional, assumed same as scale if not given).
 */
  void DrawGauge(int l_x,int l_y, double l_number, int l_decimalPlaces); 
  void DrawGauge(double l_number, int l_decimalPlaces);

};

#endif