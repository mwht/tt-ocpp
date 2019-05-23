#include <Arduino.h>
#include <Wire.h>
#define MICHAL_WIRED

int ctr = 0;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  #ifdef MICHAL_WIRED
  Wire.begin();
  #endif
}

void serialEvent() {
  while(!Serial.available()) {}
  #ifdef MICHAL_WIRED
  char charToSend = Serial.read();
  digitalWrite(13, HIGH);
  Wire.beginTransmission(9);
  Wire.write(charToSend);
  Wire.endTransmission();
  digitalWrite(13, LOW);
  #endif
}

void loop() {
  #ifdef MICHAL_WIRED
  Wire.requestFrom(9, 1, true);
  while(Wire.available()) {
    char fromI2C = Wire.read();
    if(fromI2C != 0)
      Serial.print(fromI2C);
  }
  #else
  String out = "{\"canCharge\": \"false\", \"value\": ";
  out += ctr++;
  out += "}";
  Serial.println(out);
  delay(5000);
  #endif
}