#include "TCM_CLASS.h"


TCM_MB_COMPASS::TCM_MB_COMPASS(){}
TCM_MB_COMPASS::TCM_MB_COMPASS(HardwareSerial *SerialPort, int baud)
{
    port = SerialPort;
    port->begin(baud);
}
  
void TCM_MB_COMPASS::begin(HardwareSerial *SerialPort, int baud)
{
  port = SerialPort;
  port->begin(baud);
  Serial.println("Compass communication initialized");
  Serial.flush();
  
}

void TCM_MB_COMPASS::getModInfo(void)
{
  outByteCount[0] = 0;
  outByteCount[1] = 5; // Big endian!
  byte crc_input[3];
  crc_input[0] = outByteCount[0]; 
  crc_input[1] = outByteCount[1]; 
  crc_input[2] = kGetModInfo;
  uint32_t len = sizeof(crc_input);
  byte crc[2];
  CRC(crc,crc_input, len);
  port->write(outByteCount, 2);
  port->write(kGetModInfo);
  port->write(crc[0]);
  port->write(crc[1]);
  port->flush(); // Wait for all data to be sent
}

void TCM_MB_COMPASS::set_data_component_heading(void)
{  
  outByteCount[0] = 0;
  outByteCount[1] = 7; // Big endian!
  uint8_t IDcount = (uint8_t)1;
  byte crc_input[5];
  crc_input[0] = outByteCount[0]; 
  crc_input[1] = outByteCount[1]; 
  crc_input[2] = kSetDataComponents;
  crc_input[3] = IDcount;
  crc_input[4] = kHeading;
  uint32_t len = sizeof(crc_input);
  byte crc[2];
  CRC(crc,crc_input, len);
  port->write(outByteCount, 2);
  port->write(kSetDataComponents);
  port->write(IDcount);
  port->write(kHeading);
  port->write(crc[0]);
  port->write(crc[1]);
  port->flush(); // Wait for all data to be sent
}

void TCM_MB_COMPASS::set_data_component_heading_pitch_roll(void)
{  

  Serial.println("DATA COMPONENT");
  Serial.flush();
  outByteCount[0] = 0;
  outByteCount[1] = 9; // Big endian!
  uint8_t IDcount = (uint8_t)3;
  byte crc_input[7];
  crc_input[0] = outByteCount[0]; 
  crc_input[1] = outByteCount[1]; 
  crc_input[2] = kSetDataComponents;
  crc_input[3] = IDcount;
  crc_input[4] = kHeading;
  crc_input[5] = kPitch;
  crc_input[6] = kRoll;
  uint32_t len = sizeof(crc_input);
  byte crc[2];
  CRC(crc,crc_input, len);
  port->write(outByteCount, 2);
  port->write(kSetDataComponents);
  port->write(IDcount);
  port->write(kHeading);
  port->write(kPitch);
  port->write(kRoll);
  port->write(crc[0]);
  port->write(crc[1]);
  port->flush(); // Wait for all data to be sent
  Serial.println("SETUP FINISHED");
}

void TCM_MB_COMPASS::update(void)
{
  // Send getData request
  outByteCount[0] = 0;
  outByteCount[1] = 5;
  byte crc_input[3];
  crc_input[0] = outByteCount[0]; 
  crc_input[1] = outByteCount[1];
  crc_input[2] = kGetData;
  uint32_t len = sizeof(crc_input);
  byte crc[2];
  CRC(crc, crc_input, len);
  port->write(outByteCount, 2);
  port->write(kGetData);
  port->write(crc[0]);
  port->write(crc[1]);
  port->flush(); // Wait for all data to be sent
  
  read();
}

void TCM_MB_COMPASS::read(void)
{
  if (port->available()) {
    // Read bytecount, frameID and payLoad
    port->readBytes(inByteCount,2);
    uint16_t inBytes = (inByteCount[0] << 8 | inByteCount[1] << 0); // Big endian.
    int frameID = port->read();
    char payLoad[inBytes-5];
    port->readBytes(payLoad, inBytes-5);
    byte checkSum[2];
    port->readBytes(checkSum, 2);
    
    uint8_t fid = (uint8_t)frameID; 
     
    /*
    Serial.println(fid,DEC);
    Serial.println(inBytes); 
    Serial.println(payLoad[0],DEC); // IDcount = 1
    Serial.println(payLoad[1],DEC); // kHeading = 5
    Serial.println(payLoad[6],DEC); // kHeading = 5
    Serial.println(payLoad[11],DEC); // kHeading = 5
    Serial.println(sizeof(payLoad)); 
    */
    
    switch(frameID)
    {
      default:
        Serial.println("Unknown FrameID");
        break;
      case kGetDataResp:
        {
          int k = 0;
          for (int i = 1; i <= payLoad[0]; i++) // payLoad[0] = IDcount
          {
            u.ul = ((payLoad[i+k+1] << 24) | (payLoad[i+k+2] << 16) | 
                    (payLoad[i+k+3] << 8) | ((payLoad[i+k+4]) << 0));
            
            if (payLoad[i+k] == kHeading) { heading = u.f; }
            if (payLoad[i+k] == kPitch) { pitch = u.f; }
            if (payLoad[i+k] == kRoll) { roll = u.f; }
            k += 4;
          }
        }
        break;
      case kGetModInfoResp:
        type[0] = (char)payLoad[0];
        type[1] = (char)payLoad[1];
        type[2] = (char)payLoad[2];
        type[3] = (char)payLoad[3];
        type[4] = '\0';
        revision[0] = (char)payLoad[4];
        revision[1] = (char)payLoad[5];
        revision[2] = (char)payLoad[6];
        revision[3] = (char)payLoad[7];
        revision[4] = '\0';
        break;
        
    }
  }
}

// Function for calculating CRC checksum 
void TCM_MB_COMPASS::CRC(byte * pcrc,void * data, uint32_t len)
{
    uint8_t * dataPtr = (uint8_t*)data;
    uint32_t index = 0;
    // Update the CRCfor transmitted and received data using // the CCITT 16bit algorithm(X^16 + X^12 + X^5 + 1). 
    uint16_t crc = 0;
    while(len--)
      {
	crc = (unsigned char)(crc >> 8) | (crc << 8); 
	crc ^= dataPtr[index++];
	crc ^= (unsigned char)(crc & 0xff) >> 4;
	crc ^= (crc << 8) << 4;
	crc ^= ((crc & 0xff) << 4) << 1; 
      }
    //return crc;
    pcrc[0] = (byte)(crc >> 8);
    pcrc[1] = (byte)((crc << 8) >> 8);
}

	
