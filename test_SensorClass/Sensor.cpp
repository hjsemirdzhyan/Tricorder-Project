#include "Sensor.h"

Sensor::Sensor(int x) : _message(x) {}

void Sensor::PrintData() {
  if (_message == 1) {
   tempHumid1.PrintTempData();
  } else if (_message == 2) {
    //ultrasonic1.PrintUltraData();
  } else {
    tft.print("No data");
  }
}
