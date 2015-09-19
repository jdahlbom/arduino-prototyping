void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(0, int0changed, FALLING);
  attachInterrupt(1, int1changed, FALLING);
}

void loop() {
  Serial.println("Checkpoint");
  int val2 = digitalRead(2);
  int val3 = digitalRead(3);
  Serial.print(val2);
  Serial.print(" ");
  Serial.println(val3);
delay(1000);
}

void int0changed() {
  Serial.println("Int 0 triggered");
  int val2 = digitalRead(2);
  int val3 = digitalRead(3);
  Serial.print(val2);
  Serial.print(" ");
  Serial.println(val3);
}

void int1changed() {
  Serial.println("Int 1 triggered");
  int val2 = digitalRead(2);
  int val3 = digitalRead(3);
  Serial.print(val2);
  Serial.print(" ");
  Serial.println(val3);
}

