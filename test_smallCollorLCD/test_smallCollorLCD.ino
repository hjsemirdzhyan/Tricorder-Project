void setup() {
  Serial.begin(9600);
}

void loop() {
  int output = 100;
  analogWrite(A0, output);
}
