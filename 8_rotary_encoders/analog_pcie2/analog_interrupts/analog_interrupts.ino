volatile int rotstate1 = 0;
volatile int prevState1 = 0;
const int DEBUG=0;

struct rotaryEncoder {
  int pin0;
  int pin1;
  volatile int rotationState;
  volatile int previousState;
}

void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
    pinMode(pin, INPUT);
}

rotaryEncoder encoderSetup(int pin0, int pin1) {
  pciSetup(pin0);
  pciSetup(pin1);
  rotaryEncoder *encoder;
  encoder.pin0=pin0;
  encoder.pin1=pin1;
  encoder.rotationState=0;
  encoder.previousState=digitalRead(pin0);
  return encoder;
}

void setup() {
    Serial.begin(9600);
    rotaryEncoder encoders[8];
    encoders[0] = encoderSetup(53,52);
    encoders[1] = encoderSetup(51,50);
    encoders[2] = encoderSetup(13,12);
    encoders[3] = encoderSetup(11,10);
    encoders[4] = encoderSetup(A8,A9);
    encoders[5] = encoderSetup(A10,A11);
    encoders[6] = encoderSetup(A12,A13);
    encoders[7] = encoderSetup(A14,A15);
}

void interruptChange(int pin1, int pin2, volatile int *prevState, volatile int *rotstate) {
    int i0val = digitalRead(pin1);
    int i1val = digitalRead(pin2);
    if (i0val != i1val) {
      if (i0val != *prevState) {
        *rotstate=*rotstate-1;
      } else {
        *rotstate=*rotstate+1;
      }
    }
    else {
      *prevState = i0val;
    }
    if(DEBUG) {
      Serial.println("Interrupted: ");
      Serial.println(pin1);
      Serial.println(pin2);
    }
}

ISR(PCINT0_vect)
{
    interruptChange(A8, A9, &prevState1, &rotstate1);
}

void loop() {
  Serial.print("Rotation state: ");
  Serial.println(rotstate1);
  delay(1000);
}
