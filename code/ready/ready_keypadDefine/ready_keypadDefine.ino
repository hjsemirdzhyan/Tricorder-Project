// Quick way to determine analog range of keyes_AD_key pad

#define btn_apin A0
int buttonState = 0;


void setup() {
  Serial.begin(9600);
}

void loop() {
  buttonState = analogRead(btn_apin);
  Serial.println(buttonState);
  delay(250);
}
