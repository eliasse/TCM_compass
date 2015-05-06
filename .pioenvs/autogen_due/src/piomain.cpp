#include <Arduino.h>
#include "TCM_CLASS.h"


void setup();

void loop();
void CRC(byte * pcrc,void * data, uint32_t len);
#line 3 "compass.ino"
enum CMD:uint8_t
{
kGetModInfo = 1,
kGetModInfoResp = 2
};








char outByteCount[2];
byte inByteCount[2];
byte checkSum[2];
uint8_t frameID;

void setup() {
Serial.begin(9600);
Serial3.begin(38400);
Serial.write("Trying to communicate with compass");
}

void loop() {
outByteCount[1] = 5;
byte crc_input[3];
crc_input[0] = outByteCount[0]; crc_input[1] = outByteCount[1]; crc_input[2] = kGetModInfo;
uint32_t len = sizeof(crc_input);
byte crc[2];
CRC(crc,crc_input, len);
Serial3.write(outByteCount, 2);
Serial3.write(kGetModInfo);
Serial3.write(crc[0]);
Serial3.write(crc[1]);

Serial.print(outByteCount[0], HEX);
Serial.print(outByteCount[1], HEX);
Serial.print(kGetModInfo, HEX);
Serial.print(crc[0],HEX);
Serial.print(crc[1],HEX);
Serial.println();

Serial3.flush();

delay(100);

if (Serial3.available()) {
Serial.println("Reading...");
Serial3.readBytes(inByteCount,2);
uint16_t inBytes = (inByteCount[0] << 8 | inByteCount[1]);
Serial.print("Total Packet Bytes: ");
Serial.println(inBytes);
char* frameID;
Serial3.readBytes(frameID,(size_t)1);
Serial.print("FrameID: ");
Serial.println(frameID);
char payLoad[inBytes-5];
Serial.print("Payload size: ");
Serial.println(inBytes-5);
Serial3.readBytes(payLoad, inBytes-5);
Serial3.readBytes(checkSum, 2);

Serial.println(payLoad);






}
delay(500);
}



void CRC(byte * pcrc,void * data, uint32_t len)
{
uint8_t * dataPtr = (uint8_t*)data;
uint32_t index = 0;

uint16_t crc = 0;
while(len--)
{
crc = (unsigned char)(crc >> 8) | (crc << 8);
crc ^= dataPtr[index++];
crc ^= (unsigned char)(crc & 0xff) >> 4;
crc ^= (crc << 8) << 4;
crc ^= ((crc & 0xff) << 4) << 1;
}

pcrc[0] = (byte)(crc >> 8);
pcrc[1] = (byte)((crc << 8) >> 8);
}