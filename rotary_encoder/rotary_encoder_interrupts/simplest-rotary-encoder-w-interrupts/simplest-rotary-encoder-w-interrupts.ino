const int intZero = 2;
const int intOne = 3;

int interrupted = 0;
int i0val=0;
int i1val=0;
int prevState=2;

int rotstate = 0;

void setup() {
    Serial.begin(9600);
    attachInterrupt(0, interruptChange, CHANGE);
    attachInterrupt(1, interruptChange, CHANGE);
    pinMode(intZero, INPUT);
    pinMode(intOne, INPUT);
    prevState=digitalRead(intZero);
}

void interruptChange() {
    i0val = digitalRead(intZero);
    i1val = digitalRead(intOne);
    if (i0val != i1val) {
      if (i0val != prevState) {
        --rotstate;
      } else {
        ++rotstate;
      }
    }
    else {
      prevState = i0val;
    }
    
    Serial.println(rotstate);
}

void loop() {

  delay(10);
}
