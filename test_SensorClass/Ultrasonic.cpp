#include "Arduino.h"
#include "Ultrasonic.h"

Ultrasonic::Ultrasonic() {}

bool Ultrasonic::Update(long time) {
  _pollTime = time;
  unsigned long currentTime = millis();

  if (currentTime - _previousPoll >= _pollTime) {
    _previousPoll = currentTime;
    return true;
  } else {
    return false;
  }
}

void Ultrasonic::PrintUltraData() {
  CalcDistance();
  tft.setCursor(0, 80);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
  tft.print(_duration);
  tft.println("ms, ");
  tft.print(_inches);
  tft.print("in, ");
  tft.print(_cm);
  tft.println("cm");
  tft.print("Time: ");
  tft.println(millis());
}

long Ultrasonic::CalcDistance() {
  long delay = 100;
  long clear = 2;
  long burst = 10;

  if (ultrasonicRefresh.Update(delay) == true) {
    Serial.println("in 1");
    pinMode(pingPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(pingPin, LOW);
    if (ultrasonicRefresh.Update(clear) == true) {
      digitalWrite(pingPin, HIGH);
      Serial.println("in 2");
    }
    if (ultrasonicRefresh.Update(burst) == true) {
      digitalWrite(pingPin, LOW);
      Serial.println("in 3");
    }
    _duration = pulseIn(echoPin, HIGH);
    _inches = _duration / 74 / 2;
    _cm = _duration / 29 / 2;
  }
}