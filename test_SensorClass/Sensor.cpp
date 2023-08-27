#include "Sensor.h"

void Sensor::PrintData() {
  if (_message == 1) {
    tempHumid1.PrintTempData();
  } else if (_message == 2) {
    ultrasonic1.PrintUltraData();
  } else {
    tft.print("No data");
  }
}

TempHumid& Sensor::GetTempHumid() {
  return tempHumid1;
}

Ultrasonic& Sensor::GetUltrasonic() {
  return ultrasonic1;
}

bool Sensor::IsSensor() {
  return true;
}
