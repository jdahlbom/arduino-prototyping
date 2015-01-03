// @author Jukka Dahlbom
// @created 30.12.2014

// Adapted from Wire Master Writer sample code 
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates running multiple leds in sequence on MCP23017
// I2C port expander.


#include <Wire.h>

int ADDRESS = 0x20; // Default address for MCP23017
int IODIRA = 0x00;
int GPIOA = 0x12; // IOCON.BANK bit resets to 0, so using that mode.

void setup()
{
  delay(1000);
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  Wire.beginTransmission(ADDRESS);
  Wire.write(IODIRA);         // IODIRA register
  Wire.write(0x00);        // Set bank A to output
  Wire.endTransmission();    // stop transmitting
}

void loop()
{  
  setOutput(computeUpdatedState());
  delay(300);
}

int computeUpdatedState() {
  static int phase = 0;
  int pin6 = 0x00, pin7 = 0x00, pin8 = 0x00;
  if (phase > 0 && phase < 4) {
    pin6 = 0x20;
  }
  if (phase > 1 && phase < 5) {
    pin7 = 0x40;
  }
  if (phase > 2 && phase < 6) {
    pin8 = 0x80;
  }
  phase = ++phase % 6;
  return pin6 + pin7 + pin8;
}

void setOutput(int value) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(GPIOA);        // select GPIOA register
  Wire.write(value);        // Set output bits in bank A
  Wire.endTransmission();   // stop transmitting
}
