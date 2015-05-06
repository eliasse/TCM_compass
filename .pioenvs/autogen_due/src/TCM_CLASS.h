#ifndef _TCM_MB_COMPASS_
#define _TCM_MB_COMPASS_

#include <stdint.h>
#include <Arduino.h>

class TCM_MB_COMPASS {
  HardwareSerial *port;
public:
  TCM_MB_COMPASS(HardwareSerial *SerialPort, int baud);
  void getModInfo(char *type, char *revision);
  void CRC(byte * pcrc,void * data, uint32_t len);
};

#endif
