#ifndef ADA
#define ADA
#include <Adafruit_GFX.h>    // Include Adafruit graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#endif

#include <vector>
#include <iostream>

#include "Global.h"
#include "GaugeDrawClass.h"
#include "GaugeLineClass.h"
#include "GaugeLargeDialClass.h"
#include "LineGraphClass.h"

#include "MCP_CAN_Class.h"

// Define CAN pins
#define CAN_INT_PIN 10
#define CAN_CS_PIN 21

// Create an instance of the MCP_CAN_Class
MCP_CAN_Class mcp_can(CAN_INT_PIN, CAN_CS_PIN);

#define TFT_CS     2
#define TFT_RST    0
#define TFT_DC     3
#define TFT_WIDTH 320
#define TFT_HEIGHT 240

uint16_t * DisplayBuffer;
uint16_t * TempBuffer;

// Initialize TFT display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

LineGauge * LGauge1;
LineGauge * LGauge2;
LineGauge * LGauge3;
LineGauge * LGauge4;
LineGauge * LGauge5;
LineGauge * LGauge6;
LineGauge * LGauge7;
LineGraph * LGraph8;
//LargeDialGauge * LDial1;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(1,INPUT);

  // Set Tft LED pin as output to control brightness
  pinMode(20, OUTPUT);

  // Initialize TFT display
  tft.init(TFT_HEIGHT, TFT_WIDTH); // Init ST7789 240x135

  tft.invertDisplay(false);

  // Rotate screen if needed
  tft.setRotation(1); 
  
  // Fill screen with white color
  tft.fillScreen(ST77XX_BLACK);

  // Set text color to black
  tft.setTextColor(ST77XX_WHITE);

  DisplayBuffer = new uint16_t [320*240];
  for (int i=0;i<320*240;i++) {
    DisplayBuffer[i]=0;
  }

  TempBuffer= new uint16_t[22*40*5];
  for (int i=0;i<9*100;i++) {
    TempBuffer[i]=0;
  }
  //LDial1 = new LargeDialGauge;

  LGauge1 = new LineGauge;
  LGauge2 = new LineGauge;
  LGauge3 = new LineGauge;
  LGauge4 = new LineGauge;
  LGauge5 = new LineGauge;
  LGauge6 = new LineGauge;
  LGauge7 = new LineGauge;
  LGraph8 = new LineGraph;

  //----------------------------------------
  //Turn this into a constructor
  //----------------------------------------
  LGauge1->DisplayManager(&tft, DisplayBuffer, TempBuffer);
  LGauge2->DisplayManager(&tft, DisplayBuffer, TempBuffer);
  LGauge3->DisplayManager(&tft, DisplayBuffer, TempBuffer);
  LGauge4->DisplayManager(&tft, DisplayBuffer, TempBuffer);
  LGauge5->DisplayManager(&tft, DisplayBuffer, TempBuffer);
  LGauge6->DisplayManager(&tft, DisplayBuffer, TempBuffer);
  LGauge7->DisplayManager(&tft, DisplayBuffer, TempBuffer);
  LGraph8->DisplayManager(&tft, DisplayBuffer, TempBuffer);
  //  LDial1->DisplayManager(&tft, DisplayBuffer, TempBuffer);

  LGauge1->Setup(10,100,70,0,LGauge1->rgb565Color(0,188,232),LGauge1->rgb565Color(0,188,232),0.0,500.0, 0,String("Oil"),String("kPa"),false,1.0,true,true);
  LGauge2->Setup(10,100,70,0,LGauge2->rgb565Color(0,188,232),LGauge2->rgb565Color(0,188,232),0.0,120.0, 0,String("Trans"),String("\bC"),false,1.0,true,true);
  LGauge3->Setup(10,100,70,0,LGauge3->rgb565Color(0,188,232),LGauge3->rgb565Color(0,188,232),0.0,120.0, 0,String("Cool"),String("\bC"),false,1.0,true,true);
  LGauge4->Setup(10,100,70,0,LGauge4->rgb565Color(0,188,232),LGauge4->rgb565Color(0,188,232),0.0,120.0, 0,String("Oil"),String("\bC"),false,1.0,true,true);
  LGauge5->Setup(10,100,70,0,LGauge5->rgb565Color(0,188,232),LGauge5->rgb565Color(0,188,232),0.0,40.0, 0,String("AFR"),String(":1"),false,1.0,true,true);
  LGauge6->Setup(10,100,70,0,LGauge5->rgb565Color(0,188,232),LGauge5->rgb565Color(0,188,232),0.0,400.0, 0,String("MAF"),String("g/s"),false,1.0,true,true);
  LGauge7->Setup(10,100,70,0,LGauge5->rgb565Color(0,188,232),LGauge5->rgb565Color(0,188,232),0.0,40.0, 0,String("Fuel"),String("g/s"),false,1.0,true,true);
  LGraph8->Setup(10,240,70,0,LGraph8->rgb565Color(0,188,232),LGraph8->rgb565Color(0,188,232),0.0,120.0, 0,String("Cool"),String("\bC"),false,1.0,false,true,false,5000);

  LGauge1->SetXYPos(0,0);
  LGauge4->SetXYPos(0,30);
  LGauge2->SetXYPos(0,60);
  LGauge3->SetXYPos(0,90);
  LGauge5->SetXYPos(0,120);
  LGauge6->SetXYPos(0,150);
  LGauge7->SetXYPos(0,180);
  LGraph8->SetXYPos(0,210);

  Serial.println("Initializing MCP2515...");
  delay(2000);
  // Initialize the MCP_CAN_Class instance
  if (mcp_can.begin()==0) {
    // Print initialization message
    Serial.println("MCP2515 Initialized Successfully!");      
  }
  else {
    // Print initialization message
    Serial.println("MCP2515 ERROR!!!!");
  }

  Serial.println("MCP2515 Library Receive Example Modded for Holden...");
}

int z;
//int k=0;
unsigned char PID[] = {0x14, 0x70};

void loop() {
  z=analogRead(1);

  //*****TODO********
  //HIGH PRIORITY
  //*. Develop Board
  //*. Develop Case
  //
  // MEDIUM PRIORITY
  //
  //LOW PRIORITY
  //*. Test and see if we can get rid of reset in CAN Bus class
  //*. Fix all variables to be m_members or l_local or p_pointers
  //*. Large dial working except for double.
  //*. Get SD Card Working
  //*. Add Touch
  //*. Write program (using chat GPT) to find PIDs using higher, lower, not, or equal
  
/*
k++;
if (k==9000) {k=0;}
  LDial1->DrawGauge(k/10);
delay(1);
*/

  // Read CAN messages
  mcp_can.readCAN();
  LGauge1->DrawGauge(mcp_can.getData(0x4D1,1)); //Eng Oil Pressure kPa
  mcp_can.readCAN(); 
  PID[0] = 0x19; PID[1] = 0x40;
  LGauge2->DrawGauge(mcp_can.getData(PID)); //Trans temp C
  mcp_can.readCAN();
//  LGauge3->DrawGauge(/*mcp_can.getData(0x4C1,1)*/double(z)/2.0); //Coolant Temp C
  LGauge3->DrawGauge(mcp_can.getData(0x4C1,1)); //Coolant Temp C
  mcp_can.readCAN();
  LGauge4->DrawGauge(mcp_can.getData(0x4D1,0)); //Eng Oil Temp C
  mcp_can.readCAN();
  //AFR Calc
  if (mcp_can.getData(0x1ED,1)<=0.1) { 
    LGauge5->DrawGauge(99.0); //Air fuel ratio
  }
  else {
    LGauge5->DrawGauge(mcp_can.getData(0x1EF,0)/mcp_can.getData(0x1ED,1)); //Air fuel ratio
  }
  mcp_can.readCAN();
  LGauge6->DrawGauge(mcp_can.getData(0x1EF,0)); //MAF
  mcp_can.readCAN();
  LGauge7->DrawGauge(mcp_can.getData(0x1ED,1)); //Fuel
  mcp_can.readCAN();
  LGraph8->DrawGauge(mcp_can.getData(0x4C1,1));//Coolant
  mcp_can.readCAN();
  
  //analogWrite(20, z*255/4100);
  analogWrite(20, 4096);
}

