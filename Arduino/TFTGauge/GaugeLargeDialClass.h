#ifndef GAUGELARGEDIALCLASSH
#define GAUGELARGEDIALCLASSH
#include "GaugeDrawClass.h"
#include <vector> // Include the vector header file

const int LargeTickPairs [] PROGMEM = {
  84,166,
  0,112,
  0,55,
  83,0,
  111,0,
  195,54,
  195,111,
  112,166
};

const int SmallTickPairs [] PROGMEM = {
  73,161, 
  5,127,
  5,40,
  73,5,
  127,5,
  195,39,
  195,126,
  127,161
};

const bool Rotations [] PROGMEM = {
  true,true,true,
  false,false,false,
  false,true,false,
  true,false,true,
  false,false,true,
  true,true,false,
  true,false,false,
  false,true,true
};

const int TickLabelPairs [] PROGMEM = {
  120,201, //Bottom
  72,187,
  36,151,
  29,114, //Left
  36,77,
  72,41,
  120,28, //Top
  168,41,
  204,77,
  211,114, //Right
  204,151,
  168,187
};

class LargeDialGauge : public GaugeDraw {
private:

	bool initGauge=false;
	int firstQuadStor=0;
	int secondQuadStor=5;
	std::vector<String> largeTickLabelsStor;
	int xStor=40;
	int yStor=0;
	uint16_t colourStor=rgb565Color(0,188,232);
	int ScaleLowStor=0, ScaleHighStor=1000;
	bool applyMovingAverage=true;
	int avgValue=0;
	int period=10;

  //Only update 24fps max
  unsigned long m_previousMillis = 0;   // Stores the last time the function was called
  long m_interval = 41;           // Interval for 1/24th of a second (in milliseconds)
  double m_lastNumber=-9999.0;

	void DrawGauge(int x, int y, uint16_t colour, int val, int StartOct, int FinishOct);
	void DrawFullGauge(double Number);
	void DrawFullGauge(int x, int y, uint16_t Colour, double Number);
	void DrawFullGauge(int x, int y, uint16_t Colour, double Number, int firstQuad, int secondQuad);
	void DrawFullGauge(int x, int y, uint16_t Colour, double Number, int firstQuad, int secondQuad,int ScaleLow,int ScaleHigh,std::vector<String> &largeTickLabels);

public:

	void DrawGauge(int Number);
  
  //ToDo Fix these:
  void DrawGauge(int x,int y, int number); // Pure virtual function, redefine
  void DrawGauge(int x,int y, double number); // Pure virtual function
  void DrawGauge(int x,int y, double number, int decimalPlaces); // Pure virtual function
  void DrawGauge(double number); // Pure virtual function
  void DrawGauge(double number, int decimalPlaces); // Pure virtual function

};

#endif