#include "rotaryencoder.h"

uint8_t powerLevels[8];
int8_t coolantDelta[8];
RotaryEncoder encoders[8];

const int DEBUG=0;
const uint8_t waitingForData='w';
const uint8_t openingChar = 's';

char buffer[16];

void pciSetup(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
  PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
  PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
  pinMode(pin, INPUT);
}

RotaryEncoder encoderSetup(int pin0, int pin1) {
  pciSetup(pin0);
  pciSetup(pin1);
  RotaryEncoder *encoder = (RotaryEncoder *) malloc(sizeof(RotaryEncoder));
  RotaryEncoder enc;
  enc.pin0=pin0;
  enc.pin1=pin1;
  enc.rotationState=0;
  enc.previousState=digitalRead(pin0);
  *encoder=enc;
  return *encoder;
}



void setup() {
  Serial.begin(9600);
  for (int i=0; i<8; ++i) {
    powerLevels[i] = 100;
  }
  encoders[0] = encoderSetup(53,52);
  encoders[1] = encoderSetup(51,50);
  encoders[2] = encoderSetup(13,12);
  encoders[3] = encoderSetup(11,10);
  encoders[4] = encoderSetup(A8,A9);
  encoders[5] = encoderSetup(A10,A11);
  encoders[6] = encoderSetup(A12,A13);
  encoders[7] = encoderSetup(A14,A15);
  //Start the duplex send&receive messaging.
  Serial.write(waitingForData);
}

void interruptChange(RotaryEncoder *encoder) {
  int i0val = digitalRead((*encoder).pin0);
  int i1val = digitalRead((*encoder).pin1);
  if (i0val != i1val) {
    if (i0val != (*encoder).previousState) {
      (*encoder).rotationState=(*encoder).rotationState-1;
    } 
    else {
      (*encoder).rotationState=(*encoder).rotationState+1;
    }
  }
  else {
    (*encoder).previousState = i0val;
  }
  if(DEBUG) {
    Serial.println("Interrupted: ");
    Serial.println((*encoder).pin0);
    Serial.println((*encoder).pin1);
  }
}

ISR(PCINT0_vect)
{
  interruptChange(&encoders[0]);
  interruptChange(&encoders[1]);
  interruptChange(&encoders[2]);
  interruptChange(&encoders[3]);
}

ISR(PCINT2_vect)
{
  interruptChange(&encoders[4]);
  interruptChange(&encoders[5]);
  interruptChange(&encoders[6]);
  interruptChange(&encoders[7]);
}

// Should read from analog pin and map it to [0,255]
uint8_t getPowerLevel(int sliderIndex) {
  powerLevels[sliderIndex] = powerLevels[sliderIndex] + coolantDelta[sliderIndex]*25;
  return powerLevels[sliderIndex];
}

void transmit() {
  Serial.write(openingChar);
  for (int i=0;i<8;++i) {
    //Power level should be read here as analogRead
    Serial.write(getPowerLevel(i)); 
  }
  for (int i=0;i<8;++i) {
    Serial.write(coolantDelta[i]);
    coolantDelta[i]=0;
  }
  Serial.write(waitingForData);
}

void receiveAndTransmit() {

   if (Serial.available()) {
     int openingByte = Serial.read();
     if (openingByte == openingChar) {
       byte bytesRead = Serial.readBytes(buffer, 16);
       int waitingByte = Serial.read();
        transmit();       
     } else {
       while(Serial.available()) {
         Serial.read();
       }
       Serial.write(waitingForData);
     }
   } 
}

void loop() {
  receiveAndTransmit();
  delay(10);
}
