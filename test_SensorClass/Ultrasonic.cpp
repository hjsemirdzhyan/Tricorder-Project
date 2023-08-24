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
  tft.print(_duration);
  tft.print("ms, ");
  tft.print(_inches);
  tft.print("in, ");
  tft.print(_cm);
  tft.println("cm");
}

long Ultrasonic::CalcDistance() {
  long delay = 100;
  long clear = 2;
  long burst = 10;

  if (Update(delay) == true) {
    pinMode(pingPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(pingPin, LOW);
    if (Update(clear) == true) {
      digitalWrite(pingPin, HIGH);
    }
    if (Update(burst) == true) {
      digitalWrite(pingPin, LOW);
    }
    _duration = pulseIn(echoPin, HIGH);
    _inches = _duration / 74 / 2;
    _cm = _duration / 29 / 2;
  }
}