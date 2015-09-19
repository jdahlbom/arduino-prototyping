#include "rotaryencoder.h"

const int DEBUG=0;


RotaryEncoder encoders[8];

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
  encoders[0] = encoderSetup(53,52);
  encoders[1] = encoderSetup(51,50);
  encoders[2] = encoderSetup(13,12);
  encoders[3] = encoderSetup(11,10);
  encoders[4] = encoderSetup(A8,A9);
  encoders[5] = encoderSetup(A10,A11);
  encoders[6] = encoderSetup(A12,A13);
  encoders[7] = encoderSetup(A14,A15);
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

void loop() {
  Serial.print("Rotation state: ");
  Serial.println(encoders[5].rotationState);
  delay(1000);
}

