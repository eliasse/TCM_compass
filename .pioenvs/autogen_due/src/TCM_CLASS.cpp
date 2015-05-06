#include <stdint.h>
#include <Arduino.h>

class TCM_MB_COMPASS {
  HardwareSerial *port;

  enum uint8_t {
    kGetModInfo = 1,
    kGetModInfoResp = 2,
    kSetDataComponents = 3,
    kGetData = 4,
    kGetDataResp = 5,
    
    kStartContinousMode = 21,
    kStopContinousMode = 22
  };
  
  enum kSetDataComponents:byte {
    kHeading = 5,
    kPitch = 24,
    kRoll = 25
  };
  
  
  public:
  TCM_MB_COMPASS(HardwareSerial *SerialPort, int baud){
    port = SerialPort;
    port->begin(baud);
  }
  void getModInfo(char *type, char *revision)
  {
    char outByteCount[2];
    char inByteCount[2];

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

    // delay() här?
  
    if (port->available()) {
      port->readBytes(inByteCount,2);
      uint16_t inBytes = (inByteCount[0] << 8 | inByteCount[1]);
      char* frameID;
      port->readBytes(frameID,(size_t)1);
      char payLoad[inBytes-5];
      port->readBytes(payLoad, inBytes-5);
      byte checkSum[2];
      port->readBytes(checkSum, 2);
    }
  }
  //float getHeading()
  //float getRoll()
  //float getPitch()
private:
  // Function for calculating CRC checksum 
  void CRC(byte * pcrc,void * data, uint32_t len)
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
};
	
