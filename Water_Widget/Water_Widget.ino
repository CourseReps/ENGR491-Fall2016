/* Oh, so you want to use this code, eh?...
 * 
 * Purpose:
 * Run on an arduino based platform to collect sensor data related
 * to water flow, spectroscopy of water, and transmit that data
 * over a wireless connection to a secondary device.
 * 
 * Microcontroller: Teensy 3.1 (Sometimes Teensy++ 2.0)
 * 
 * Water flow sensor: YF-402 by SEA
 *                    Working range: 0.3-6.0 L/min
 *                    Water pressure: <=0.8 MPa
 * 
 */

#include <Adafruit_NeoPixel.h>
#include "TimerOne.h"
#include "SoftwareSerial.h"






//------------------------------------------------------------------
// System-wide settings
//------------------------------------------------------------------

// Pin number assignments

#define NEOPIXEL_OUTPUT_PIN 6
#define NUM_NEOPIXELS      1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_NEOPIXELS, NEOPIXEL_OUTPUT_PIN, NEO_GRB + NEO_KHZ800);

const unsigned int FLOW_SENSOR_INPUT_PIN = 12;
const unsigned int HEARTBEAT_LED_OUTPUT_PIN = 13;
const unsigned int RPM_INDICATOR_LED_OUTPUT_PIN = 13;

// Heartbeat LED related settings
const unsigned int HEARTBEAT_INTERVAL_MS = 5000;
const unsigned int HEARTBEAT_LED_DURATION_MS = 50;

// Counts the number of times that the HIGH signal is reached
volatile unsigned int flowSensorHighCount;

// Variable to save the time of the last LED heartbeat blink.
volatile unsigned long lastHeartbeatMillis;

// Specify digital ports for Bluetooth Serial connection
SoftwareSerial BluetoothSerial(5, 6); // RX, TX

//------------------------------------------------------------------
// Setup() is the first function to execute.
//------------------------------------------------------------------
void setup()
{
  // Initialize the hardline USB serial port.
  Serial.begin(9600);
  // Initialize the bluetooth module UART serial connection.
  BluetoothSerial.begin(9600);

  // Assign the functionality of pins
  pinMode( HEARTBEAT_LED_OUTPUT_PIN, OUTPUT );
  pinMode( RPM_INDICATOR_LED_OUTPUT_PIN, OUTPUT );
  pinMode( FLOW_SENSOR_INPUT_PIN, INPUT );

  // Assign an interrupt so that when the flow sensor pin goes from
  // LOW to HIGH, a function will be triggered.
  attachInterrupt(FLOW_SENSOR_INPUT_PIN, RegisterFlowSensorHigh, RISING);

  // This initializes the NeoPixel library.
  pixels.begin(); 
}

//------------------------------------------------------------------
// Loop() runs continuously until the board is reset or turned off.
//------------------------------------------------------------------
void loop()
{
  // Loop through all neopixels
  for( int i = 0; i < NUM_NEOPIXELS; i++ )
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(500); // Delay for a period of time (in milliseconds).
  }
  flowSensorHighCount = 0;
  
  interrupts();
  delay(1000);
  noInterrupts();
  
  double calc = (flowSensorHighCount * 60.0 / 7.5);

  String calcDataString = "Liters per hour: " + String(calc, 4) ; // String( doubleVar , decimalPlaces)
  Serial.println(calcDataString);
  BluetoothSerial.println(calcDataString);

  // Blink the led and transmit over serial to show we're alive
  Heartbeat();
}

// Called via interrupt when the FLOW_SENSOR_PIN's square wave
// transitions from LOW to HIGH voltage.
void RegisterFlowSensorHigh()
{
  // Increment the "high" counter global variable.
  flowSensorHighCount++;
  
  // Flash the RPM_INDICATOR_LED_PIN each time we hit a high signal.
  BlinkRpmIndicatorLed();
}

//------------------------------------------------------------------
// Controls a visual feedback LED which flashes faster when the
// flow rate increases, and slower when flow decreases.
//------------------------------------------------------------------
void BlinkRpmIndicatorLed()
{
  // Variable to save the time of the last rpm blink.
  static unsigned long lastRpmBlinkMillis;

  // Grab the current milliseconds as reported by the arduino.
  unsigned long nowMillis = millis();
  
  // If we blink too fast, then the LED appears to always be on.
  // To prevent this, we enforce a speed limit on the flashing.
  if( nowMillis - lastRpmBlinkMillis >= 30 )
  {
    // Blink the led
    digitalWrite( RPM_INDICATOR_LED_OUTPUT_PIN, HIGH );
    delay( 5 );
    digitalWrite( RPM_INDICATOR_LED_OUTPUT_PIN, LOW );
    
    // Save the current milliseconds for the next time we run.
    lastHeartbeatMillis = nowMillis;
    lastRpmBlinkMillis = nowMillis;
  }
}

//------------------------------------------------------------------
// Makes sure the heartbeat led flashes at a regular interval, so
// you know that your board hasn't locked up.
//------------------------------------------------------------------
void Heartbeat()
{
  // Variable to save the time of the last heartbeat.
  static unsigned long lastHeartbeatMillis;

  // Simple counter so something is changing on our message
  static unsigned int heartbeatCounter;

  // Grab the current milliseconds as reported by the arduino.
  unsigned long nowMillis = millis();
  
  // Don't do anything unless the defined amount of time has passed.
  if( nowMillis - lastHeartbeatMillis >= HEARTBEAT_INTERVAL_MS )
  {
    // Transmit over serial
    Serial.print( heartbeatCounter );
    Serial.println( ": Heartbeat..." );

    String heartbeatDataString = "Heartbeat " + heartbeatCounter;
    Serial.println(heartbeatDataString);
    BluetoothSerial.println(heartbeatDataString);

    // Blink the led
    digitalWrite( HEARTBEAT_LED_OUTPUT_PIN, HIGH );
    delay( HEARTBEAT_LED_DURATION_MS );
    digitalWrite( HEARTBEAT_LED_OUTPUT_PIN, LOW );
    
    // Save the current milliseconds for the next time we run.
    lastHeartbeatMillis = nowMillis;

    heartbeatCounter++;
  }
}




