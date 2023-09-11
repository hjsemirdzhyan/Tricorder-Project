#include "SPI.h"
#include "Adafruit_ILI9341.h"

#define pingPin 7
#define echoPin 6

extern Adafruit_ILI9341 tft;
extern DelayTracker ultrasonicRefresh;

class Ultrasonic {
  unsigned long _previousPoll;  // will store last time temp/humid reading was updated
  long _pollTime;               // how often in milliseconds to poll the temp sensor
  long _duration;
  long _inches;
  long _cm;

public:
  Ultrasonic();
  bool Update(long time);
  void PrintUltraData();
  long CalcDistance();
};