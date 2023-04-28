/*
Dimming LED
*/

int potPin = A0;
int potValue = 0;
int led = 9;

void setup() {
   pinMode(led, OUTPUT); 
}

void loop() {
    potValue = analogRead(potPin);
    analogWrite(led, potValue/4);
    delay(10);
}
