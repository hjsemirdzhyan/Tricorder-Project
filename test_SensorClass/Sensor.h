#include "Adafruit_ILI9341.h"
#include "TempHumid.h"
#include "Ultrasonic.h"

extern Adafruit_ILI9341 tft;

const uint16_t Sensor_Color = ILI9341_DARKGREY;

class Sensor {
  TempHumid tempHumid1;
  Ultrasonic ultrasonic1;
  int _message = 0;

public:
  void PrintData();
  TempHumid& GetTempHumid();
  Ultrasonic& GetUltrasonic();
  bool IsSensor();

};