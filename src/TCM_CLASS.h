#ifndef _TCM_MB_COMPASS_
#define _TCM_MB_COMPASS_

#include <stdint.h>
#include <Arduino.h>

class TCM_MB_COMPASS {
  public:
  float heading;
  float pitch;
  float roll;
  char type[5];
  char revision[5];
  
  enum uint8_t {
    kGetModInfo = 1,
    kGetModInfoResp = 2,
    kSetDataComponents = 3,
    kGetData = 4,
    kGetDataResp = 5,
    
    kStartContinousMode = 21, // Not yet implemented
    kStopContinousMode = 22   // ------ || ------
  };
  
  enum kSetDataComponents:byte {
    kHeading = 5,
    kPitch = 24,
    kRoll = 25,
  };
  
  TCM_MB_COMPASS();
  TCM_MB_COMPASS(HardwareSerial *SerialPort, int baud);
  void begin(HardwareSerial *SerialPort, int baud);
  void getModInfo(void);
  void set_data_component_heading(void);
  void set_data_component_heading_pitch_roll(void);
  void update(void); // Sends GetData
  
  private:
  char outByteCount[2];
  char inByteCount[2];
  
  union {
    float f;
    unsigned long ul;
  } u;
  HardwareSerial *port;
  void CRC(byte * pcrc,void * data, uint32_t len);
  void read(void); // Read and decode incoming data
};

#endif
