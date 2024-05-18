#ifndef MCP_CAN_CLASS_H
#define MCP_CAN_CLASS_H

#include <mcp_can.h>
#include <SPI.h>
#include <pins_arduino.h>
#include <vector>


class MCP_CAN_Class {
public:
    MCP_CAN_Class(int canIntPin, int canCsPin);
    int begin();
    void readCAN();
    float getData(unsigned char l_PID[2]); //Get data based off PID
    float getData(unsigned long l_canID, int index); //Get high speed can bus

private:
    MCP_CAN can;
    int _canIntPin;
    int _canCsPin;
    long unsigned int _rxId;
    unsigned char _len;
    unsigned char _ext;
    unsigned char _rxBuf[8];
    char _msgString[128];
    long _messages;
    long _timePrev;
    int _upto;
    int _uptoMess;
    struct can_frame {
        unsigned long can_id; //What ID to send send the request to (7E0? 7E2?)
        unsigned char can_dlc; //message length
        unsigned char data[2] __attribute__((aligned(8))); //PID data
        unsigned long response; //Response ID (7E8 for 7E0, always, + 8)
        float responseData; //Stored value of last response
        String ColloquialName; //Human readable name of PID
        float Offset;//Offset of response
        float Scale; //Scale of response
        bool TwoByte; //Is it a two byte PID response?
        unsigned long updateSpeedMillis; //Speed in millis to request an update from the ECU
        unsigned long prevMillis; //Previous Millis
        String Units; //Human readable units
    };
    struct can_data_struct {
        unsigned char message_start_byte=0; //Start byte of response
        unsigned char message_length_bytes=0; //length response
        bool isSigned=false;
        bool isString=false;
        float Offset=0.0;//Offset of response
        float Scale=0.0; //Scale of response
        float responseData=0.0; //Stored value of last response
        String ColloquialName; //Human readable name of PID
        String Units; //Human readable units
    };
    struct highspeed_can_frame {
        unsigned long canID; //CAN ID
        std::vector<can_data_struct> data_struct;
    };   
    static const int msgSize = 5;
    std::vector<can_frame> _canMsg;
    std::vector<highspeed_can_frame> highspeed_can_messages;
    void setupMessages();
    byte sendCanBus(can_frame &canMsg1);
};

#endif
