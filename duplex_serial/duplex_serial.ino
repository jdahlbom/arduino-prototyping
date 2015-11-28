#include "rotaryencoder.h"

uint8_t powerLevels[8];
int8_t coolantDelta[8];
RotaryEncoder encoders[8];

//#define DEBUG

const uint8_t waitingForData='w';
const uint8_t openingChar = 's';

const uint8_t msgTypeData = 1;
const uint8_t msgTypeString = 2;

const uint8_t signalPowerMax = 255;
const uint8_t signalPowerMin = 0;
const uint8_t txPowerMax = 123;
const uint8_t txPowerMin = 33;
const uint8_t txCoolantOffset = 100;
const uint8_t txCoolantMin = 65;
const uint8_t txCoolantMax = 122;

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
    powerLevels[i] = (uint8_t)(100.0/300.0*255);
  }
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
#ifdef DEBUG
    Serial.println("Interrupted: ");
    Serial.println((*encoder).pin0);
    Serial.println((*encoder).pin1);
    Serial.println((*encoder).rotationState);
#endif
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

// Should read from analog pin and map it to [0,100]
float getPowerLevel(int sliderIndex) {
  powerLevels[sliderIndex] = powerLevels[sliderIndex] + coolantDelta[sliderIndex]*5;
  return scalePowerSignalToPercentage(powerLevels[sliderIndex]);
}

void computeCoolants() {
  for (int i=0;i<8;++i) {
    coolantDelta[i] = encoders[i].rotationState;
#ifdef DEBUG
    Serial.print(i);
    Serial.println(coolantDelta[i]);
#endif
    encoders[i].rotationState=0;
  }
}

uint8_t scaleCoolant(int8_t delta) {
  return max(txCoolantMin, min(txCoolantOffset + delta, txCoolantMax));
}

float scalePowerSignalToPercentage(uint8_t powerSignal) {
  return max(0.0f, min(100.0f, ((float)powerSignal) / 255.0f));
}

uint8_t scalePowerToTx(float powerPercentage) {
  return (uint8_t) (powerPercentage * (txPowerMax-txPowerMin) + txPowerMin);
}

void sendMessageHeader(uint8_t messageType, uint8_t messageLength) {
  for (int i=0;i<2; ++i) {
    Serial.write(0);
  }
  Serial.write(messageType);
  Serial.write(messageLength);
}

void transmit() {
  sendMessageHeader(msgTypeData, 16);
  for (int i=0;i<8;++i) {
    //Power level should be read here as analogRead
    Serial.write(scalePowerToTx(getPowerLevel(i))); 
  }
  for (int i=0;i<8;++i) {
    Serial.write(scaleCoolant(coolantDelta[i]));
  }
}

void transmitString(char* string) {
  int len = strlen(string)+1;
  sendMessageHeader(msgTypeString, len);
  Serial.write(string);
}

void printStatus() {
    char str[120];

  sprintf(str, "Power: ");
  int len = 0;
  for (int i=0; i<8; ++i) {
    len = strlen(str);
    sprintf(str+len, " %3u", getPowerLevel(i));
  }
  len = strlen(str);
  sprintf(str+len, "\nCoolant: ");
  for (int i=0; i<8; ++i) {
    len = strlen(str);
    sprintf(str+len, " %2d", coolantDelta[i]);
  }
  transmitString(str);
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
     }
   } 
}

void loop() {
  computeCoolants();
  transmit();
  printStatus();
  delay(3000);
}
