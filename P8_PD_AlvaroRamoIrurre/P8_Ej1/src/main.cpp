#include <Arduino.h>

void setup() {
  Serial.begin(115200);   
  Serial2.begin(115200); 
}

void loop() {
  
  if (Serial.available()) {
    char c = Serial.read();
    Serial2.write(c);
  }
  
  if (Serial2.available()) {
    char c = Serial2.read();
    Serial.write(c);
  }
}
