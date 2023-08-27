#include "Adafruit_ILI9341.h"
#include "TempHumid.h"

TempHumid::TempHumid() {}

void TempHumid::PrintTempData() {
  CalcTempHumid();
  tft.setCursor(0, 80);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
  tft.print("Temp: ");
  tft.print(_temp);
  tft.print("C ");
  tft.print((_temp * 1.800) + 32.00);
  tft.println("F ");
  tft.print("Humid: ");
  tft.print(_humid);
  tft.println("%");
}

double TempHumid::CalcTempHumid() {
  long delay = 2000;
  
  if (Update(delay) == true) {
    DHT.read11(dht_apin);  // remember to limit poll frequency
    _temp = DHT.temperature;
    _humid = DHT.humidity;
  }
}

bool TempHumid::Update(long time) {
  _pollTime = time;
  unsigned long currentTime = millis();

  if (currentTime - _previousPoll >= _pollTime) {
    _previousPoll = currentTime;
    return true;
  } else {
    return false;
  }
}