volatile int rotstate1 = 0;
volatile int prevState1 = 0;
const int DEBUG=0;

void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
    pinMode(pin, INPUT);
}

void setup() {
    Serial.begin(9600);
    
    pciSetup(50);
    pciSetup(51);
    prevState1=digitalRead(50);
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
    if (DEBUG) {
      Serial.println("Interrupted: ");
      Serial.println(pin1);
      Serial.println(pin2);
    }
}

ISR(PCINT0_vect)
{
    interruptChange(50, 51, &prevState1, &rotstate1);
}

void loop() {
  Serial.print("Rotation state: ");
  Serial.println(rotstate1);
  delay(1000);
}
