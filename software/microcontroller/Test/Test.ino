#include <WiFi.h>

int status = WL_IDLE_STATUS;     // the Wifi radio's status

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WiFi shield not present")); 
    // don't continue:
    while(true);
  } 

  // check firmware version
  Serial.print(F("Firmware version: "));
  Serial.println(WiFi.firmwareVersion());
}

void loop() {
}
