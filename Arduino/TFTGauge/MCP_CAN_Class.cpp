#include "MCP_CAN_Class.h"

void(* resetFunc) (void) = 0; //declare reset function @ address 0

MCP_CAN_Class::MCP_CAN_Class(int canIntPin, int canCsPin) : can(canCsPin) {
    _canIntPin = canIntPin;
    _canCsPin = canCsPin;
}

int MCP_CAN_Class::begin() {
    setupMessages();
    pinMode(_canIntPin, INPUT);
    if (can.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
      can.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
      can.enOneShotTX();
      return 0;
    }
    return -1;
}

void MCP_CAN_Class::readCAN() {
  //Serial.println("here");
  //Need to read as fast as possible, only stores last two messages so if we are too slow we may miss data
  while(!digitalRead(_canIntPin))                         // If CAN0_INT pin is low, read receive buffer
  {
    can.readMsgBuf(&_rxId, &_ext, &_len, _rxBuf);      // Read data: len = data length, buf = data byte(s)
    //Serial.println(_rxId);

    //Reset if locked up
    _messages++;
    if ((_messages>20000) || (_len>32)){
      Serial.print("Reset ");
      Serial.println(millis()-_timePrev);
      resetFunc();
    }

    if ((_len==8)&&(
      ((_rxId & 0xFF0)==0x7E0) ||
       ((_rxId & 0xFF0)==0x5E0) //(Don't care about 5E0, not using multiple PIDs, can remove)
       ))
    {
      if((_rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      } else {
        bool _found=false;
        //Serial.print("ID ");
        //Serial.println(_rxId);
        for (int j=0;j<_canMsg.size();j++){
          //Check correct PID
          if ((_rxBuf[2]==_canMsg.at(j).data[0]) && (_rxBuf[3]==_canMsg.at(j).data[1])) {
            _found=true;
            //Check correct module responded
            if (_rxId==_canMsg.at(j).response) {
              //Check if response is valid
              if ((_rxBuf[1]==0x62)) {
                //Two or single byte response
                if ((_canMsg.at(j).TwoByte)&&(_rxBuf[0]==0x5)) {
                  _canMsg.at(j).responseData=((256.0f*(float)_rxBuf[4]+(float)_rxBuf[5])+_canMsg.at(j).Offset)*_canMsg.at(j).Scale;
                }
                else if ((!_canMsg.at(j).TwoByte)&&(_rxBuf[0]==0x4)) {
                  _canMsg.at(j).responseData=(((float)_rxBuf[4])+_canMsg.at(j).Offset)*_canMsg.at(j).Scale;
                }
              }
              else {
                Serial.print(_canMsg.at(j).data[0],HEX);
                Serial.print(",");
                Serial.print(_canMsg.at(j).data[1],HEX);
                Serial.println(" Bad Response, message not OK");
              }
            }
          }
        }

        //Useful for debugging new PIDs
        /*          
        if (!_found) {
          //Unknown response, spill data
          Serial.print(_rxId,HEX);
          Serial.print(" ");

          for(byte i = 0; i<_len; i++){
            if (_rxBuf[i]<0x10) {
              Serial.print("0");
            }
            Serial.print(_rxBuf[i],HEX);
          }
          Serial.println("");
        }
        else {
          //Only update serial if first PID is received (prevent spamming unnecessarily)
          if ((_rxBuf[2]==_canMsg[0].data[0]) && (_rxBuf[3]==_canMsg[0].data[1])) {
            //Print Values
            for (int j=0;j<msgSize;j++){
              Serial.print(_canMsg[j].ColloquialName); Serial.print(" ");
              Serial.print(_responseData[j],2); Serial.print(" ");
              Serial.print(_canMsg[j].Units); Serial.print(" ");
            }
            Serial.println("");
          }
        }
        */

      }
    }
    //NEED TO ADD CODE HERE FOR ALL PIDs
    else if ((_len<=8)&&(_len>0))
    {
      if((_rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      }
      else {

        for (int i=0;i<highspeed_can_messages.size();i++) {
          if ((highspeed_can_messages.at(i).canID==_rxId)) {
            for (int j=0;j<highspeed_can_messages.at(i).data_struct.size();j++) {
              unsigned long l_StartBit = (highspeed_can_messages.at(i).data_struct.at(j)).message_start_byte;
              unsigned long l_LengthBit = (highspeed_can_messages.at(i).data_struct.at(j)).message_length_bytes;
              if(l_StartBit+l_LengthBit <= _len*8){
                double tempD = 0;
                unsigned long tempInt = 0;

                unsigned long interTemp=0;
                for (unsigned long k = l_StartBit;k<l_StartBit+l_LengthBit;k++ ) {
                  int m = k/8;
                  int n = 7-(k%8);
                  tempInt = tempInt*2;
                  interTemp = _rxBuf[m];
                  interTemp = interTemp >> n; 
                  interTemp = interTemp & 1;
                  tempInt = tempInt+interTemp;
                }

                //Need Code for signed values
                //isSigned;
                if (((highspeed_can_messages.at(i).data_struct.at(j)).isSigned)&&(tempInt&(1<<(l_LengthBit-1))!=0)){
                  tempInt=(1<<l_LengthBit)-tempInt;
                  tempD=-tempInt;
                }
                else {
                  tempD=tempInt;
                }

                tempD*=(highspeed_can_messages.at(i).data_struct.at(j)).Scale;
                tempD+=(highspeed_can_messages.at(i).data_struct.at(j)).Offset;

                (highspeed_can_messages.at(i).data_struct.at(j)).responseData=tempD;
                
                /*
                if (_rxId==0x4D1) {
                  Serial.print(_rxId);
                  Serial.print(" ");
                  Serial.print(_len);
                  Serial.print(" ");
                  Serial.print(tempInt);
                  Serial.print(" ");
                  Serial.print(_rxBuf[1]);
                  Serial.print(" ");
                  Serial.print(_rxBuf[2]);
                  Serial.print(" ");
                  Serial.println(tempD);
                }*/
              }
            }

/*            if ((index<highspeed_can_messages.at(i).data_struct.size())) {
              return (highspeed_can_messages.at(i).data_struct.at(index)).responseData;
            }*/
          }
        }
        
        /*
        float MCP_CAN_Class::getData(unsigned long l_canID, int index){ //Get data based off PID
          for (int i=0;i<highspeed_can_messages.size();i++) {
            if ((highspeed_can_messages.at(i).canID==l_canID)) {
              if ((index<highspeed_can_messages.at(i).data_struct.size())) {
                return (highspeed_can_messages.at(i).data_struct.at(index)).responseData;
              }
              return -2.0;
            }
          }
          return -1.0;
        }

        */        
    }

  }


    //Send Stuff Here (cycle through all codes)
    unsigned long l_currentMillis;

    for (int j=0;j<msgSize;j++){
      l_currentMillis = millis();
      if (l_currentMillis-_canMsg[j].prevMillis>=_canMsg[j].updateSpeedMillis){
        sendCanBus(_canMsg[j]);
        _canMsg[j].prevMillis=l_currentMillis;
      }
    }
  }
  /*
  _upto++;
  if (_upto==100) {
    sendCanBus(_canMsg[_uptoMess]);
    _uptoMess++;
    if (_uptoMess>=msgSize) _uptoMess=0;
    _upto=0;
  }*/
  
  //Reset if locked up
  if (millis()-_timePrev>6000){
    _timePrev=millis();
    _messages=0;
  }

}

//Return stored PID value
float MCP_CAN_Class::getData(unsigned char l_PID[2]){ //Get data based off PID
  for (int i=0;i<_canMsg.size();i++) {
    if ((l_PID[0]==_canMsg.at(i).data[0])&&(l_PID[1]==_canMsg.at(i).data[1])) {
      return _canMsg.at(i).responseData;
    }
  }
  return -1.0;
}

//Return stored high speed PID value
float MCP_CAN_Class::getData(unsigned long l_canID, int index){ //Get data based off PID
  for (int i=0;i<highspeed_can_messages.size();i++) {
    if ((highspeed_can_messages.at(i).canID==l_canID)) {
      if ((index<highspeed_can_messages.at(i).data_struct.size())) {
        return (highspeed_can_messages.at(i).data_struct.at(index)).responseData;
      }
      return -2.0;
    }
  }
  return -1.0;
}

byte MCP_CAN_Class::sendCanBus(can_frame &canMsg1) {
    unsigned char bdata[8] __attribute__((aligned(8)));
    bdata[0] = 0x03;
    bdata[1] = 0x22;
    bdata[2] = canMsg1.data[0];
    bdata[3] = canMsg1.data[1];
    bdata[4] = 0x00;
    bdata[5] = 0x00;
    bdata[6] = 0x00;
    bdata[7] = 0x00;

    byte sndStat = can.sendMsgBuf(canMsg1.can_id, 0, canMsg1.can_dlc, bdata);
    if(sndStat == CAN_OK){
        //Serial.println("Success");
    } else {
        //Serial.println("Error Sending Message...");
    }
    return sndStat;
}

void MCP_CAN_Class::setupMessages() {
  can_frame temp_canMsg;
/*
  //Engine Oil Pressure
  //7E8 0462147031AAAAAA Response
  temp_canMsg.ColloquialName = "Eng Oil P";
  temp_canMsg.can_id  = 0x7E0;
  temp_canMsg.can_dlc = 8;
  temp_canMsg.data[0] = 0x14; //PID Byte #1
  temp_canMsg.data[1] = 0x70; //PID Byte #2
  temp_canMsg.response = 0x7E8;
  temp_canMsg.Offset = 0.0f;
  temp_canMsg.Scale = 4.0f;
  temp_canMsg.TwoByte = false;
  temp_canMsg.Units = "kPa";
  temp_canMsg.responseData = 0.0;
  temp_canMsg.updateSpeedMillis=50;
  temp_canMsg.prevMillis=0;
  _canMsg.push_back(temp_canMsg);
*/
  //Transmission Temp
  //7EA 0462194075AAAAAA Response
  temp_canMsg.ColloquialName = "Trans T";
  temp_canMsg.can_id  = 0x7E2;
  temp_canMsg.can_dlc = 8;
  temp_canMsg.data[0] = 0x19; //PID Byte #1
  temp_canMsg.data[1] = 0x40; //PID Byte #2
  temp_canMsg.response = 0x7EA;
  temp_canMsg.Offset = -40.0f;
  temp_canMsg.Scale = 1.0f;
  temp_canMsg.TwoByte = false;
  temp_canMsg.Units = "C";
  temp_canMsg.responseData = 0.0;
  temp_canMsg.updateSpeedMillis=5100;
  temp_canMsg.prevMillis=0;
  _canMsg.push_back(temp_canMsg);
/*
  //Engine Coolant Temp
  temp_canMsg.ColloquialName = "Eng Cool T";
  temp_canMsg.can_id  = 0x7E0;
  temp_canMsg.can_dlc = 8;
  temp_canMsg.data[0] = 0x00; //PID Byte #1
  temp_canMsg.data[1] = 0x05; //PID Byte #2
  temp_canMsg.response = 0x7E8;
  temp_canMsg.Offset = -40.0f;
  temp_canMsg.Scale = 1.0f;
  temp_canMsg.TwoByte = false;
  temp_canMsg.Units = "C";
  temp_canMsg.responseData = 0.0;
  temp_canMsg.updateSpeedMillis=5000;
  temp_canMsg.prevMillis=0;
  _canMsg.push_back(temp_canMsg);

  //Engine Oil Temp
  temp_canMsg.ColloquialName = "Eng Oil T";
  temp_canMsg.can_id  = 0x7E0;
  temp_canMsg.can_dlc = 8;
  temp_canMsg.data[0] = 0x11; //PID Byte #1
  temp_canMsg.data[1] = 0x54; //PID Byte #2
  temp_canMsg.response = 0x7E8;
  temp_canMsg.Offset = -40.0f;
  temp_canMsg.Scale = 1.0f;
  temp_canMsg.TwoByte = false;
  temp_canMsg.Units = "C";
  temp_canMsg.responseData = 0.0;
  temp_canMsg.updateSpeedMillis=5200;
  temp_canMsg.prevMillis=0;
  _canMsg.push_back(temp_canMsg);

  //Air Fuel Ratio Commanded
  temp_canMsg.ColloquialName = "Air Fuel Ratio";
  temp_canMsg.can_id  = 0x7E0;
  temp_canMsg.can_dlc = 8;
  temp_canMsg.data[0] = 0x11; //PID Byte #1
  temp_canMsg.data[1] = 0x9E; //PID Byte #2
  temp_canMsg.response = 0x7E8;
  temp_canMsg.Offset = 0.0f;
  temp_canMsg.Scale = 0.1f;
  temp_canMsg.TwoByte = false;
  temp_canMsg.Units = "%";
  temp_canMsg.responseData = 0.0;
  temp_canMsg.updateSpeedMillis=100;
  temp_canMsg.prevMillis=0;
  _canMsg.push_back(temp_canMsg);*/

  highspeed_can_frame l_tempHighSpeedMessage;
  can_data_struct l_tempHighSpeedDataPoint;

  //Tyre Pressure Module
  l_tempHighSpeedMessage.canID=0x52A;
  l_tempHighSpeedDataPoint.message_start_byte=8*2; //Byte 3
  l_tempHighSpeedDataPoint.message_length_bytes=8;
  l_tempHighSpeedDataPoint.ColloquialName="Tyre Pressure - Front Left";
  l_tempHighSpeedDataPoint.Units="kPa";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=4.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*3; //Byte 4
  l_tempHighSpeedDataPoint.ColloquialName="Tyre Pressure - Front Right";
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*4; //Byte 5
  l_tempHighSpeedDataPoint.ColloquialName="Tyre Pressure - Rear Left";
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*5; //Byte 6
  l_tempHighSpeedDataPoint.ColloquialName="Tyre Pressure - Rear Right";
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Wheel Speed
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x34A;
  l_tempHighSpeedDataPoint.message_start_byte=8*0; //Byte 1-2
  l_tempHighSpeedDataPoint.message_length_bytes=16; //16bit
  l_tempHighSpeedDataPoint.ColloquialName="Wheel Speed - Rear Left";
  l_tempHighSpeedDataPoint.Units="km/h";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/32.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*2; //Byte 3-4
  l_tempHighSpeedDataPoint.ColloquialName="Wheel Speed - Rear Right";
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Wheel Speed
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x348;
  l_tempHighSpeedDataPoint.message_start_byte=8*0; //Byte 1-2
  l_tempHighSpeedDataPoint.message_length_bytes=16; //16bit
  l_tempHighSpeedDataPoint.ColloquialName="Wheel Speed - Front Left";
  l_tempHighSpeedDataPoint.Units="km/h";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/32.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*2; //Byte 3-4
  l_tempHighSpeedDataPoint.ColloquialName="Wheel Speed - Front Right";
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //G-force
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x1E9;
  l_tempHighSpeedDataPoint.message_start_byte=8*0+3; //Byte 1 Bit 4
  l_tempHighSpeedDataPoint.message_length_bytes=10; //16bit
  l_tempHighSpeedDataPoint.ColloquialName="Lateral Accelleration";
  l_tempHighSpeedDataPoint.Units="m/s2";
  l_tempHighSpeedDataPoint.isSigned=true;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/64.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*4+3; //Byte 5 Bit 4
  l_tempHighSpeedDataPoint.message_length_bytes=10; //16bit
  l_tempHighSpeedDataPoint.ColloquialName="Yaw Rate";
  l_tempHighSpeedDataPoint.Units="grad/s";
  l_tempHighSpeedDataPoint.isSigned=true;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/16.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Steering Wheel
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x1E5;
  l_tempHighSpeedDataPoint.message_start_byte=8*1; //Byte 2
  l_tempHighSpeedDataPoint.message_length_bytes=16; //16bit
  l_tempHighSpeedDataPoint.ColloquialName="Steering Angle";
  l_tempHighSpeedDataPoint.Units="Deg";
  l_tempHighSpeedDataPoint.isSigned=true;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/16.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*3+4; //Byte 4 Bit 5
  l_tempHighSpeedDataPoint.message_length_bytes=12; //12bit
  l_tempHighSpeedDataPoint.ColloquialName="Steering Rate";
  l_tempHighSpeedDataPoint.Units="Deg/s";
  l_tempHighSpeedDataPoint.isSigned=true;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*6+5; //Byte 7 Bit 6
  l_tempHighSpeedDataPoint.message_length_bytes=1;
  l_tempHighSpeedDataPoint.ColloquialName="Steering Straight";
  l_tempHighSpeedDataPoint.Units="T/F";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Accelerator
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x1C4;
  l_tempHighSpeedDataPoint.message_start_byte=8*5; //Byte 6
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Accelerator Pedal Position";
  l_tempHighSpeedDataPoint.Units="%";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Speed Distance
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x3E9;
  l_tempHighSpeedDataPoint.message_start_byte=8*0; //Byte 1
  l_tempHighSpeedDataPoint.message_length_bytes=16; 
  l_tempHighSpeedDataPoint.ColloquialName="Speed";
  l_tempHighSpeedDataPoint.Units="km/hr";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=100.0/1.6;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*4; //Byte 5
  l_tempHighSpeedDataPoint.ColloquialName="Speed2";
  l_tempHighSpeedDataPoint.Units="km/hr";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=100.0/1.6;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*2; //Byte 3
  l_tempHighSpeedDataPoint.message_length_bytes=16; 
  l_tempHighSpeedDataPoint.ColloquialName="Distance";
  l_tempHighSpeedDataPoint.Units="km";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=8.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*6; //Byte 6
  l_tempHighSpeedDataPoint.message_length_bytes=16; 
  l_tempHighSpeedDataPoint.ColloquialName="Distance2";
  l_tempHighSpeedDataPoint.Units="km";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=8.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Engine
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0xC9;
  l_tempHighSpeedDataPoint.message_start_byte=8*1; //Byte 2
  l_tempHighSpeedDataPoint.message_length_bytes=16; 
  l_tempHighSpeedDataPoint.ColloquialName="Engine RPM";
  l_tempHighSpeedDataPoint.Units="rpm";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/4.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*4; //Byte 4
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Accelerator Position";
  l_tempHighSpeedDataPoint.Units="%";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=100.0/255.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Fuel
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x1ED;
  l_tempHighSpeedDataPoint.message_start_byte=8*0; //Byte 1
  l_tempHighSpeedDataPoint.message_length_bytes=16; 
  l_tempHighSpeedDataPoint.ColloquialName="Fuel Pressure - Requested";
  l_tempHighSpeedDataPoint.Units="kPa";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/32.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*2; //Byte 3
  l_tempHighSpeedDataPoint.message_length_bytes=16; 
  l_tempHighSpeedDataPoint.ColloquialName="Instantaneous Fuel Flow Estimate";
  l_tempHighSpeedDataPoint.Units="g/s";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/1024.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*4; //Byte 5
  l_tempHighSpeedDataPoint.message_length_bytes=13; 
  l_tempHighSpeedDataPoint.ColloquialName="(Possible)Fuel Economy";
  l_tempHighSpeedDataPoint.Units="L/100km";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Acceleration
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x1A1;
  l_tempHighSpeedDataPoint.message_start_byte=8*6; //Byte 7
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Accelerator Pedal Position";
  l_tempHighSpeedDataPoint.Units="%";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=100.0/255.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*1+2; //Byte 3 Bit 3
  l_tempHighSpeedDataPoint.message_length_bytes=1; 
  l_tempHighSpeedDataPoint.ColloquialName="Car Moving";
  l_tempHighSpeedDataPoint.Units="T/F";
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*1+3; //Byte 3 Bit 4
  l_tempHighSpeedDataPoint.ColloquialName="Neutral/Park";
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Brake
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0xF1;
  l_tempHighSpeedDataPoint.message_start_byte=8*1; //Byte 2
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Brake Pedal Position";
  l_tempHighSpeedDataPoint.Units="%";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=100.0/255.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Odometer
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x120;
  l_tempHighSpeedDataPoint.message_start_byte=8*2; //Byte 3
  l_tempHighSpeedDataPoint.message_length_bytes=8*3; 
  l_tempHighSpeedDataPoint.ColloquialName="Odometer";
  l_tempHighSpeedDataPoint.Units="km";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/64.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Brakes
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0xBE;
  l_tempHighSpeedDataPoint.message_start_byte=8*1; //Byte 2
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Brake Pedal Position";
  l_tempHighSpeedDataPoint.Units="%";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=100.0/255.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Fuel
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x1EB;
  l_tempHighSpeedDataPoint.message_start_byte=8*0; //Byte 1
  l_tempHighSpeedDataPoint.message_length_bytes=16; 
  l_tempHighSpeedDataPoint.ColloquialName="Fuel System Pressure";
  l_tempHighSpeedDataPoint.Units="kPa";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Air Flow
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x1EF;
  l_tempHighSpeedDataPoint.message_start_byte=8*2; //Byte 3
  l_tempHighSpeedDataPoint.message_length_bytes=16; 
  l_tempHighSpeedDataPoint.ColloquialName="Mass Air Flow";
  l_tempHighSpeedDataPoint.Units="g/s";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/100.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Engine
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x4D1;
  l_tempHighSpeedDataPoint.message_start_byte=8*1; //Byte 2
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Engine Oil Temperature";
  l_tempHighSpeedDataPoint.Units="*C";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=-40.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*2; //Byte 3
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Engine Oil Pressure";
  l_tempHighSpeedDataPoint.Units="kPa";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=4.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*5; //Byte 6
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Fuel Level";
  l_tempHighSpeedDataPoint.Units="%";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=100.0/255.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*3+3;
  l_tempHighSpeedDataPoint.message_length_bytes=12; 
  l_tempHighSpeedDataPoint.ColloquialName="Fuel Total Capacity";
  l_tempHighSpeedDataPoint.Units="L";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/8.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //Air Flow
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x4C1;
  l_tempHighSpeedDataPoint.message_start_byte=8*1; //Byte 1
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Barometric Pressure";
  l_tempHighSpeedDataPoint.Units="kPa";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=0.0;
  l_tempHighSpeedDataPoint.Scale=1.0/2.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*2;
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Engine Coolant Temperature";
  l_tempHighSpeedDataPoint.Units="*C";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=-40.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*3;
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Intake Air Temperature";
  l_tempHighSpeedDataPoint.Units="*C";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=-40.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  l_tempHighSpeedDataPoint.message_start_byte=8*4;
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Outside Air Temperature";
  l_tempHighSpeedDataPoint.Units="*C";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=-40.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //TCM
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x4C9;
  l_tempHighSpeedDataPoint.message_start_byte=8*1;
  l_tempHighSpeedDataPoint.message_length_bytes=8; 
  l_tempHighSpeedDataPoint.ColloquialName="Transmission Control Module Temperature";
  l_tempHighSpeedDataPoint.Units="*C";
  l_tempHighSpeedDataPoint.isSigned=false;
  l_tempHighSpeedDataPoint.Offset=-40.0;
  l_tempHighSpeedDataPoint.Scale=1.0;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);
/*
  //VIN Part 1
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x514;
  l_tempHighSpeedDataPoint.message_start_byte=8*0;
  l_tempHighSpeedDataPoint.message_length_bytes=8*8; 
  l_tempHighSpeedDataPoint.ColloquialName="VIN Part 1";
  l_tempHighSpeedDataPoint.Units="";
  l_tempHighSpeedDataPoint.isString=true;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);

  //VIN Part 2
  l_tempHighSpeedMessage.data_struct.clear();
  l_tempHighSpeedMessage.canID=0x4E1;
  l_tempHighSpeedDataPoint.message_start_byte=8*0;
  l_tempHighSpeedDataPoint.message_length_bytes=8*8; 
  l_tempHighSpeedDataPoint.ColloquialName="VIN Part 2";
  l_tempHighSpeedDataPoint.Units="";
  l_tempHighSpeedDataPoint.isString=true;
  l_tempHighSpeedMessage.data_struct.push_back(l_tempHighSpeedDataPoint);
  highspeed_can_messages.push_back(l_tempHighSpeedMessage);
*/

/*
-----
NOTE:
-----
If you delete or add messages above, make sure you change "msgSize"
*/

  //------------------------------------------
  //0x7E0 Engine Control Module (Response 7E8)
  //------------------------------------------
  //220005 Engine Coolant Temp (A-40)*C
  //221154 Engine Oil temp (A-40)*C
  //22002F Fuel Level (A*100/255)%
  //221470 Engine Oil Pressure (A*4)kPA
  //221193-A Injector Pulse Width 1-8 ((A*256+B)*100/131)ms
  //22162F-36 Balance Rate 1-8 ((A*256+B)*5/32-5120)mm3/str
  //220042 Control Module Voltage ((A*256+B)/1000)V
  //22002F Fuel Level (A*100/255)%
  //22002F Fuel Remaining (A/255*35.25)L
  //22000D Speed (A)km/h
  //221172 Intake Air Control Position (A/255*100)%
  //22119E Commanded Air Fuel Ratio (A/10)
  //221564 Aircon High Side Pressure (A*4) kPa 
  //224051-4 ABS F/R L/R Wheel Speed (A) km/hr
  //22F432 Fuel Tank Pressure ((A*256+B)*0.00003)PSI
  //221538 Intake Air Temperature 2 (A-40)*C
  //221161 Outside Air Temperature (A-40)*C
  //221251 Barometric Pressure (A)*kPa
  //2219DE Engine Torque (((A*256+B)*10/2)/10)ft/lbs
  //22119F Remaining Oil Life ((A*200/51)/10)%
  //22119D Barometer V8 ((A*3)/10)inHg
  //221251 Barometer V6 ((A*3)/10)inHg
  //2211A6 Knock Retard ((A*45/50)/10)Deg
  //2211A1 Elapsed Time Since Engine Start (A*256+B)Sec
  //221201-4 Cylinder 1-4 Misfire History (A*256+B)
  //2211F8-B Cylinder 5-8 Misfire History (A*256+B)
  //221205-8 Cylinder 1-4 Misfire Current (A*256+B)
  //2211EA-D Cylinder 5-8 Misfire Current (A*256+B)
  //22163D Desired Fuel Pressure ((A*145/10)/100)kPsi
  //22163E Actual Fuel Pressure ((A*145/10)/100)kPSI
  //2211A6 Knock Retard (A*22.5/256)Deg
  //22125D Knock Retard 2 (A*22.5/256)Deg
  //22125E Knock Active Count (A)Count
  //221141 Ignition 1 Voltage (A/10)V
  //221145 H2OS Sensor (A*4.34)mV
  //221192 Commanded Idle Speed (A*12.5)RPM
  //221992 Last Shift Time (A/40)sec
  //221993 1-2 Shift Time (A/40)sec
  //221994 2-3 Shift Time (A/40)sec
  //221995 3-4 Shift Time (A/40)sec
  //221997 1-2 Shift Error (A/40)sec
  //221998 2-3 Shift Error (A/40)sec
  //221999 3-4 Shift Error (A/40)sec
  //22199E PC Solenoid Actual Current (A*0.0195)Amp
  //22199F PC Solenoid Reference Current (A*0.0195)Amp
  //22004C *M Commanded Throttle Pos (A*100/255)%
  //22203F *M Engine Torque (((256*A)+B)/4)Nm
  //221131 *M Fuel System Status (A)B1
  //22000F *M Intake Air Temp IAT ((A-40)/2)C
  //221573 *M Long Term Fuel Trip Bank 2 (100*A/255)%
  //221572 *M Long Term Fuel Trip Bank1 (100*A/255)%
  //221570 *M Short Term Fuel Trip Bank1 (100*A/255)%
  //221571 *M Short Term Fuel Trip Bank2 (100*A/255)%
  //22000E *M Spark Advance (A/2-64)Deg
  //222434 BRAKE ((A*256+B)/64)V
  //22801E Outside Temp Filtered (((A-40)/8))C
  //------------------------------------------

  //------------------------------------------
  //0x7E2 Transm Control Module (Response 7EA)
  //------------------------------------------
  //222862 Transmission Oil Pressure (S_A*256+B)
  //221940 Transmission Oil Temp (A-40)
  //221941 Transmission Input Shaft Speed ((A*256+B)/4)rpm
  //22199A Current Gear (A)
  //222889 PRNDL (A)
  //221191 Torque Converter Clutch Slip ((A*256+B)/8)rpm
  //------------------------------------------

/*
High Speed CAN Bus messages

Bytes start from 0, bits start from 0:
0x0F9, 	Byte3, 	16Bit, 	Unsigned, 		,(unknown) Brake or Accelerator?
0x0F9, 	Byte4, 	16Bit, 	Unsigned, 		,(unknown) Brake or Accelerator (hold)?
0x0F1, 	Byte1, 	16Bit, 	Unsigned, 		,Brake Pedal Position
0x0BE, 	Byte1, 	16Bit, 	Unsigned, 		,(unknown) Brake or Accelerator?
0x52A, 	Byte2, 	8Bit, 	Unsigned, [A*4] kPa	,Tyre KPA Front Left
0x52A, 	Byte3, 	8Bit, 	Unsigned, [A*4] kPa	,Tyre KPA Front Right
0x52A, 	Byte4, 	8Bit, 	Unsigned, [A*4] kPa	,Tyre KPA Rear Left
0x52A, 	Byte5, 	8Bit, 	Unsigned, [A*4] kPa	,Tyre KPA Rear Right
0x4E9, 	Byte4, 	8Bit, 	Unsigned, 		,(unknown) Temperature? Seems to increase linearly
0x451, 	Byte4, 	8Bit, 	Unsigned, 		,(unknown) Countdown timer  
0x3F9, 	Byte1, 	16Bit, 	Signed, 		,(unknown) Counter? Drive Time?  
0x3F9, 	Byte3, 	8Bit, 	Unsigned, 		,(unknown)
0x3F9, 	Byte5, 	8Bit, 	Unsigned, 		,(unknown) Brake
0x3F9, 	Byte7, 	8Bit, 	Unsigned, 		,(unknown)
0x3F1, 	Byte1, 	8Bit, 	Unsigned, 		,(unknown)
0x3D3, 	Byte1, 	8Bit, 	Unsigned, 		,(unknown)
0x3D1, 	Byte1, 	8Bit, 	Unsigned, 		,(unknown) Cruise control?
0x3D1, 	Byte3, 	8Bit, 	Unsigned, 		,(unknown) Cruise control?
0x34A, 	Byte0, 	16Bit, 	Unsigned, [A/32] km/h	,(unknown) Wheel Speed Rear Left
0x34A, 	Byte2, 	16Bit, 	Unsigned, [A/32] km/h	,(unknown) Wheel Speed Rear Right
0x348, 	Byte0, 	16Bit, 	Unsigned, [A/32] km/h	,(unknown) Wheel Speed Front Left
0x348, 	Byte2, 	16Bit, 	Unsigned, [A/32] km/h	,(unknown) Wheel Speed Front Right
0x2F9, 	Byte0, 	8Bit, 	Unsigned, 		,(unknown) Unknown, seems a bit random. First 4 bits might be wheel sensor pulses healthy, last 2 might be system healthy pulses?
0x2F9, 	Byte5, 	8Bit, 	Unsigned, 		,(unknown) Brakes?
0x2C3, 	Byte0, 	16Bit, 	Unsigned, 		,Stays at 100% but drops occasionally and recharges
0x2C3, 	Byte3, 	8Bit, 	Unsigned, 		,(unknown)
0x2C3, 	Byte4, 	16Bit, 	Unsigned, 		,(unknown)
0x2C3, 	Byte6, 	8Bit, 	Unsigned, 		,(unknown)
0x214, 	Byte1, 	8Bit, 	Unsigned, 		,(unknown) Brakes?
0x1FC, 	Byte0, 	16Bit, 	Signed, 		,(unknown)
0x1FC, 	Byte2, 	16Bit, 	Signed, 		,(unknown)
0x1F5, 	Byte3, 	8Bit, 	Bits, 			,Shift position PRNDL
0x1F5, 	By3B0, 	1Bit, 	Bits, 			,Engine Off
0x1F5, 	By3B2, 	1Bit, 	Bits, 			,Engine On
0x1F5, 	By3B5, 	1Bit, 	Bits, 			,Drive
0x1F5, 	By3B7, 	1Bit, 	Bits, 			,Park
0x1F5, 	By0B3, 	4Bit, 	Bits, 			,(unknown) Gears?
0x1F5, 	By1B3, 	4Bit, 	Bits, 			,(unknown) Gears Selected?
0x1F5, 	By6B6, 	2Bit, 	Bits, 			,(unknown) TC Active?
0x1F1, 	By4B3, 	1Bit, 	Bits, 			,Park Brake On/Off
0x1F1, 	By6B0, 	5Bit, 	Bits, 			,(Unknown) Gear/Moving?
0x1E9, 	By0B3, 	10Bit, 	Signed, [A/64] m/s2	,Lateral Acceleration
0x1E9, 	By4B3, 	10Bit, 	Signed, [A/16] grad/s	,Yaw Rate
0x1E9, 	By0B3, 	1Bit, 	Bits, 			,Brake Pedal Pressed
0x1E5, 	Byte1, 	16Bit, 	Signed, [A*0.0625] Deg	,Steering Angle
0x1E5, 	By3B4, 	12Bit, 	Signed, [A] Deg/s	,Steering Rate
0x1E5, 	By3B0, 	4Bit, 	Bits, 			,(unknown)
0x1E5, 	By6B5, 	1Bit, 	Bits, 			,(unknown) Steering Wheel Straight
0x1C4, 	By6B7, 	9Bit, 	Signed, 		,(unknown) g's?
0x1C4, 	Byte5, 	8Bit, 	Unsigned, 		,Accelerator Pedal
0x1C3, 	Byte0, 	16Bit, 	Signed, 		,(unknown) g's?
0x1C3, 	Byte2, 	16Bit, 	Signed, 		,(unknown) g's?
0x1C3, 	Byte6, 	8Bit, 	Unsigned, 		,(unknown) Accelerator
0x19D, 	Byte5, 	16Bit, 	Unsigned, 		,(unknown) Accelerator or Brake?
0x191, 	Byte0, 	16Bit, 	Signed, 		,(unknown) g's?
0x191, 	Byte2, 	16Bit, 	Signed, 		,(unknown) g's? Same as above
0x191, 	Byte4, 	16Bit, 	Signed, 		,(unknown) g's?
0x191, 	Byte6, 	8Bit, 	Unsigned, 		,(unknown) Accellerator or Brake?
0x185, 	By0B5, 	10Bit, 	Unsigned, 		,(unknown) g's?
0x3E9, 	Byte0, 	16Bit, 	Unsigned, [A/100] km/hr	,Speed
0x3E9, 	Byte4, 	16Bit, 	Unsigned, [A/100] km/hr	,Speed (Holds)
0x3E9, 	Byte2, 	16Bit, 	Unsigned, [A/8]	km	,Distance
0x3E9, 	Byte6, 	16Bit, 	Unsigned, [A/8]	km	,Distance (Holds)
0x3FB, 	By0B6, 	1Bit, 	Bits, 			,(unknown) Engine up to temp?
0x3FB, 	By0B7, 	1Bit, 	Bits, 			,(unknown) Engine cold?
0x0C9, 	By0B0, 	1Bit, 	Bits, 			,Engine on
0x0C9, 	By0B5, 	1Bit, 	Bits, 			,Braking
0x0C9, 	Byte1, 	16Bit, 	Unsigned, [A/4] rpm	,Engine Speed
0x0C9, 	By3B3, 	1Bit, 	Bits, 			,Braking
0x0C9, 	By5B7, 	1Bit, 	Bits, 			,Braking
0x0C9, 	Byte4, 	8Bit, 	Unsigned, [A/255*100] %	,Accelerator Actual Position
0x1ED, 	Byte0, 	16Bit, 	Unsigned, [A/32] kPa	,Fuel Pressure Requested
0x1ED, 	Byte2, 	16Bit, 	Unsigned, [A/1024] g/s	,Instantaneous Fuel Flow Estimate
0x1ED, 	Byte5, 	13Bit, 	Unsigned,  		,(unknown) L/100?
0x1A1, 	Byte6, 	8Bit, 	Unsigned, [A/255*100] %	,Accelerator Actual Position
0x1A1, 	By1B2, 	1Bit, 	Bits, 			,Car Moving
0x1A1, 	By1B3, 	1Bit, 	Bits, 			,Neutral/Park
0x0F1, 	Byte1, 	8Bit, 	Unsigned, [A] %		,Brake Pressure
0x135, 	By0B0, 	4Bit, 	Bits, 			,4= Sports Shift
0x135, 	By0B4, 	4Bit, 	Bits, 			,0=park, 1=neutral, 2=drive/L, 3=reverse
0x135, 	By1B3, 	1Bit, 	Bits, 			,Traction Control on/off
0x135, 	By1B4, 	1Bit, 	Bits, 			,Loss of Traction
0x120, 	Byte2, 	16Bit, 	Unsigned, [A/64] km	,ODO
0x120, 	Byte0, 	16Bit, 	Unsigned, [A] kPa	,Fuel Pressure Estimated
0x1EF, 	Byte2, 	16Bit, 	Unsigned, [A/100] g/s	,Mass air flow sensor (MAF)
0x4D1, 	Byte1, 	8Bit, 	Unsigned, [A-40] *C	,Engine Oil Temperature
0x4D1, 	Byte2, 	8Bit, 	Unsigned, [A*4] kPa	,Engine Oil Pressure
0x4D1, 	Byte5, 	8Bit, 	Unsigned, [A*100/255] %	,Fuel Level
0x4D1, 	By3B3, 	12Bit, 	Unsigned, [A/8] L	,Fuel Total Capacity
0x4C1, 	Byte1, 	8Bit, 	Unsigned, [A/2] kPa	,Barometric Pressure
0x4C1, 	Byte2, 	8Bit, 	Unsigned, [A-40] *C	,Engine Coolant Temperature
0x4C1, 	Byte3, 	8Bit, 	Unsigned, [A-40] *C	,Intake Air Temperature
0x4C1, 	Byte4, 	8Bit, 	Unsigned, [A/2-40] *C	,Outside Air Temperature
0x4C9, 	Byte1, 	8Bit, 	Unsigned, [A-40] *C	,Transmission Control Module Temperature
0x514, 	Byte0, 	64Bit, 	Char, 			,Vin Part 1 in ASCII
0x4E1, 	Byte0, 	64Bit, 	Char, 			,Vin Part 2 in ASCII

*/

}
