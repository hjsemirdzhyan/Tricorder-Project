#include "dht.h"
#include "Adafruit_ILI9341.h"

#define dht_apin A0  // Analog Pin sensor is connected to

extern Adafruit_ILI9341 tft;
extern dht DHT;

class TempHumid {
  double _temp;
  double _humid;

public:
  TempHumid();
  void PrintTempData();
  double CalcTempHumid();
};