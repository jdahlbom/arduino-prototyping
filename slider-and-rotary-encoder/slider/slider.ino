#include<Wire.h>

int IODIRA   = 0x00;
int IODIRB   = 0x01;
int GPINTENB = 0x05;
int GPPUB    = 0x0D;
int DEFVALB  = 0x07;
int INTCONB  = 0x09;
//int INTFB    = 0x0F;
int INTCAPB  = 0x11;
int GPIOA    = 0x12;
int GPIOB    = 0x13;
//int IOCON    = 0x0B;

int INTERRUPT_PIN_B = 0;

int led1 = 0x20;
int led2 = 0x40;
int led3 = 0x80;

int ADDRESS = 0x20;

int interrupt_b_triggered = 0;
int dir1Events = 0;
int dir2Events = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  writeBank(ADDRESS, IODIRA, 0x00);   // Set A pins as outputs
  writeBank(ADDRESS, IODIRB, 0xFF);   // Set B pins as inputs
  writeBank(ADDRESS, GPINTENB, 0xFF); // Enable interrupts for B pins
  writeBank(ADDRESS, GPPUB, 0x00);    // Set pull up resistors (100kohm) for B pins
  writeBank(ADDRESS, DEFVALB, 0xFF);  // Use HIGH as comparison value for all B pins
  writeBank(ADDRESS, INTCONB, 0xFF);  // Compare interrupt value against previous value */
  attachInterrupt(INTERRUPT_PIN_B, processBInterrupts, FALLING);
}

void writeBank(int address, int reg, int value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

byte readBank(int address, int reg) {
  // From: https://coronax.wordpress.com/2012/11/18/arduino-port-expansion/
  Wire.beginTransmission (address);
  Wire.write (reg); 
  Wire.endTransmission ();  
  // Then we try to read one byte from that chip
  Wire.requestFrom (address, 1);
  byte value = Wire.read(); 
  return value;
}

void processBInterrupts() {
  interrupt_b_triggered = 1;
}

void loop() {

  static int iterations = 0;
    if (interrupt_b_triggered) {
    Serial.print("Interrupt triggered");
    byte interruptValues = readBank(ADDRESS, INTCAPB);
    Serial.println("Interrupt values:");
    Serial.println(interruptValues);
    if ( ~interruptValues & 0x01) {
      Serial.print("Pin 1 triggered!");
      dir1Events += 1;
    }
    if (~interruptValues & 0x02) {
      Serial.print("Pin 2 triggered!");
      dir2Events += 1;
    }
    iterations=11;
    interrupt_b_triggered = 0;
  }

  int sensorValue = analogRead(A0);

  float powerValue = sensorValue * 100.0 / 1023.0;
  writeBank(ADDRESS, GPIOA, indicatorPinValues(powerValue));

  Serial.print(powerValue);
  Serial.print(",");
  ++iterations;
  if (iterations > 10) {
    iterations = 0;
    Serial.println("");
  }
  delay(100);
}

int indicatorPinValues(int powerValue) {
  float maxTolerance = 1.0;
  int leds = 0;
  float maxValue = 100.0;
  float numberOfLeds = 3.0;
  if (powerValue > maxValue/numberOfLeds) {
    leds += led1;
  }
  if (powerValue > 2*maxValue/numberOfLeds) {
    leds += led2;
  }
  if (powerValue > maxValue - maxTolerance) {
    leds += led3;
  }
  return leds;
}
