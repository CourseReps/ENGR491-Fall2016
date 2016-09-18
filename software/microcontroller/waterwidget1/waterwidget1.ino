#include <SoftwareSerial.h> // needed for serial comm over digital pins other than onboard UART serial-over-usb
#include <Wire.h> // needed for I2C decives (SDA,SCL)
#include "Adafruit_NeoPixel.h"
#include "Adafruit_TCS34725.h"


// Which pin on the Arduino is connected to the NeoPixels?
const int PIN_NEOPIXEL_OUT = 6;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN_NEOPIXEL_OUT, NEO_GRB + NEO_KHZ800);

// Color sensor setup
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

// Set up SoftwareSerial to Bluetooth transmitter
const int PIN_BTSERIAL_RX = 10;
const int PIN_BTSERIAL_TX = 11;
SoftwareSerial BtSerial(PIN_BTSERIAL_RX, PIN_BTSERIAL_TX); // RX, TX


const int PIN_FLOWSENSOR_IN = 3;

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
  BtSerial.begin(9600);

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

  Serial.print(flowRate);
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
  Serial.print(",\n");

  BtSerial.print(flowRate);
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
  BtSerial.print(",\n");
  
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


// END OF FILE
