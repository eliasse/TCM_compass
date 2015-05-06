
enum CMD:uint8_t
{
  kGetModInfo = 1,
  kGetModInfoResp = 2
};

/*enum payload:uint16_t
{
  pGetModInfo = 0
  // pGetModInfoResp = ;
};
*/

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
  outByteCount[1] = 5; // Big endian!
  byte crc_input[3];
  crc_input[0] = outByteCount[0]; crc_input[1] = outByteCount[1]; crc_input[2] = kGetModInfo;
  uint32_t len = sizeof(crc_input);
  uint16_t crc = CRC(crc_input, len);
  Serial3.write(outByteCount, 2);
  Serial3.write(kGetModInfo);
  Serial3.write(crc);
  
  // Verifiera att datan aer korrekt
  Serial.print(outByteCount[0], HEX);
  Serial.print(outByteCount[1], HEX);
  Serial.print(kGetModInfo, HEX);
  Serial.print(crc,HEX);
  Serial.println();
  
  delay(100);
  
  if (Serial3.available()) {
    Serial.println("Reading...");
    Serial3.readBytes(inByteCount,2);
    uint16_t inBytes = (inByteCount[1] << 8 | inByteCount[0]);
    frameID = Serial3.read();
    char payLoad[inBytes-3];
    Serial3.readBytes(payLoad, inBytes-2);
    Serial3.readBytes(checkSum, 2);
    
    Serial.write("Frame ID: \n");
    Serial.print(frameID);
    Serial.print(payLoad);
  }
  delay(500);
}

// Function for calculating CRC checksum
uint16_t CRC(void * data, uint32_t len)
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
  return crc; 
}
