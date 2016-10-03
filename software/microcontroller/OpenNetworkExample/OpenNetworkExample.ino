#include <WiFi.h>

char ssid[] = "HRG";     // the name of your network
int status = WL_IDLE_STATUS;     // the Wifi radio's status

void setup() {
  // initialize serial:
  Serial.begin(9600);

  // attempt to connect to an open network:
  Serial.println("Attempting to connect to open network...");
  status = WiFi.begin(ssid);

  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  // if you are connected :
  else {
      Serial.print("Connected to the network");
  }
}

void loop() {
  // do nothing
}
