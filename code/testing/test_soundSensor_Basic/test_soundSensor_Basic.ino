int digitalResult = 0;
int analogResult = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  analogResult = analogRead(0);
  digitalResult = digitalRead(6);
  Serial.print(digitalResult);
  Serial.println(analogResult);
  delay(100);
}
