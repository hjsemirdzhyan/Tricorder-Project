#include "dht.h"
#include "Adafruit_ILI9341.h"

#define dht_apin A0  // Analog Pin sensor is connected to

extern Adafruit_ILI9341 tft;
extern dht DHT;

class TempHumid {
  unsigned long _previousPoll;  // will store last time temp/humid reading was updated
  long _pollTime;               // how often in milliseconds to poll the temp sensor
  long _duration;
  double _temp;
  double _humid;

public:
  TempHumid();
  void PrintTempData();
  double CalcTempHumid();
  bool Update(long time);
};