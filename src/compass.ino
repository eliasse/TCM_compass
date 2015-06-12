#include "TCM_CLASS.h"

TCM_MB_COMPASS cmps;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HardwareSerial *CompassPort;
  CompassPort = &Serial3;
  //TCM_MB_COMPASS cmps(CompassPort,38400);
  cmps.begin(CompassPort, 38400);
  cmps.set_data_component_heading_pitch_roll();
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  cmps.getModInfo();
  delay(100);
  Serial.println(cmps.type);
  //Serial.print(" ");
  Serial.println(cmps.revision);
  */
  
  cmps.update();
  Serial.print("H: ");
  Serial.println(cmps.heading);
  //Serial.print("P: ");
  //Serial.println(cmps.pitch);
  //Serial.print("R: ");
  //Serial.println(cmps.roll);
  delay(100);
}
