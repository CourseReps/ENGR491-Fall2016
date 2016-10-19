#include <Adafruit_NeoPixel.h>

#include <SoftwareSerial.h> // needed for serial comm over digital pins other than onboard UART serial-over-usb
#include <Wire.h> // needed for I2C decives (SDA,SCL)
#include "Adafruit_NeoPixel.h"
#include "Adafruit_TCS34725.h"

//wifi 
#include <SPI.h>
#include <WiFi.h>

//Wifi - WPA2 AES CCPM
char ssid[] = "rover1";     //  your network SSID (name)
char pass[] = "hgoedbgmd";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
IPAddress ip(192, 168, 3, 7);

// Which pin on the Arduino is connected to the NeoPixels?
const int PIN_NEOPIXEL_OUT = 6;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN_NEOPIXEL_OUT, NEO_GRB + NEO_KHZ800);

// Color sensor setup
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

const int PIN_FLOWSENSOR_IN = 2; //must be a pin capable of an external hardware interupt

// Counts the number of times that the HIGH signal is reached
volatile unsigned int flowSensorHighCount;

//------------------------------------------------------------------
// Setup() is the first function to execute.
//------------------------------------------------------------------
void setup()
{  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // Set the data rate and start the SoftwareSerial port
  //BtSerial.begin(9600);
//wifi init part 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  WiFi.config(ip);

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();
//wifi init part done

  // Assign the functionality of pins
  pinMode( PIN_FLOWSENSOR_IN, INPUT );

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  // Initialize the NeoPixel library.
  pixels.begin(); 
  
  // Assign an interrupt so that when the flow sensor pin goes from
  // LOW to HIGH, a function will be triggered.
  attachInterrupt(digitalPinToInterrupt(PIN_FLOWSENSOR_IN), RegisterFlowSensorHigh, RISING);
}

//------------------------------------------------------------------
// Loop() runs continuously until the board is reset or turned off.
//------------------------------------------------------------------
void loop()
{
  //----------------------------------------------
  // FLOW SENSOR
  //----------------------------------------------
  
  // Reset our interrupt counter back to zero.
  flowSensorHighCount = 0;

  // Turn on interrupts so we can collect flowrate sensor interrupts.
  interrupts();
  // Allow interrupt collection for 1 second.
  delay(1000);
  // Turn interrupts off again.
  noInterrupts();
  // Calculate the flowrate from how many interrupts occured during one sec.
  int flowRate = (flowSensorHighCount * 60.0 / 7.5);

  //----------------------------------------------
  // LED LIGHT (neopixel)
  //----------------------------------------------

  // this will naturally initialize to zero, and at the end of this
  // loop function we cycle it from 0-2, where 0=red 1=green 2=blue
  static unsigned currentColorIndex; 

  int ledRedVal, ledGreenVal, ledBlueVal;
  
  if( currentColorIndex == 0 )
  {
    ledRedVal = 200; ledGreenVal = 0; ledBlueVal = 0;
  }
  if( currentColorIndex == 1 )
  {
    ledRedVal = 0; ledGreenVal = 200; ledBlueVal = 0;
  }
  if( currentColorIndex == 2 )
  {
    ledRedVal = 0; ledGreenVal = 0; ledBlueVal = 200;
  }

  // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  pixels.setPixelColor( 0, pixels.Color( ledRedVal, ledGreenVal, ledBlueVal ) );
  pixels.show(); // This sends the updated pixel color to the hardware.

  //----------------------------------------------
  // COLOR SENSOR
  //----------------------------------------------
  
  // Set up vars for color sensor
  uint16_t sensorRedVal, sensorGreenVal, sensorBlueVal, sensorC, colorTemp, lux;

  // Populate color sensor vars
  tcs.getRawData( &sensorRedVal, &sensorGreenVal, &sensorBlueVal, &sensorC );
  // calculate the rest of our color sensor values
  colorTemp = tcs.calculateColorTemperature( sensorRedVal, sensorGreenVal, sensorBlueVal );
  lux = tcs.calculateLux( sensorRedVal, sensorGreenVal, sensorBlueVal );

  //----------------------------------------------
  // COMMUNICATION TO DEVICES (publish our data)
  //----------------------------------------------

  // Print our data in this order:
  // flowRate, currentColorIndex, colorTemp, lux, sensorRedVal, sensorGreenVal, sensorBlueVal, sensorC;

  /*Serial.print(flowRate);
  Serial.print(",");
  Serial.print(currentColorIndex);
  Serial.print(",");
  Serial.print(colorTemp);
  Serial.print(",");
  Serial.print(lux);
  Serial.print(",");
  Serial.print(sensorRedVal);
  Serial.print(",");
  Serial.print(sensorGreenVal);
  Serial.print(",");
  Serial.print(sensorBlueVal);
  Serial.print(",");
  Serial.print(sensorC);
  Serial.print(",\n");*/

  /*BtSerial.print(flowRate);
  BtSerial.print(",");
  BtSerial.print(currentColorIndex);
  BtSerial.print(",");
  BtSerial.print(colorTemp);
  BtSerial.print(",");
  BtSerial.print(lux);
  BtSerial.print(",");
  BtSerial.print(sensorRedVal);
  BtSerial.print(",");
  BtSerial.print(sensorGreenVal);
  BtSerial.print(",");
  BtSerial.print(sensorBlueVal);
  BtSerial.print(",");
  BtSerial.print(sensorC);
  BtSerial.print(",\n");*/

  // check the network connection once every 10 seconds:
  delay(10000);
  printCurrentNet();
  
  // For the next time around, we set up the next LED color to use.
  // This will cycle from 0-2, where 0=red 1=green 2=blue
  currentColorIndex++;
  currentColorIndex = currentColorIndex % 3;
  
} // END loop()

//------------------------------------------------------------------
// Called via interrupt when the PIN_FLOWSENSOR_IN's square wave
// transitions from LOW to HIGH voltage.
//------------------------------------------------------------------
void RegisterFlowSensorHigh()
{
  // Increment the "high" counter global variable.
  flowSensorHighCount++;
}

//wifi functions
void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

// END OF FILE
