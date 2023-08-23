#include "TempHumid.h"

TempHumid::TempHumid() {}

void TempHumid::PrintTempData() {
  CalcTempHumid();
  tft.print("Temp: ");
  tft.println(_temp);
  tft.setCursor(30, 80);
  tft.print("Humid: ");
  tft.println(_humid);
}

double TempHumid::CalcTempHumid() {
  DHT.read11(dht_apin);  // remember to limit poll frequency
  _temp = DHT.temperature;
  _humid = DHT.humidity;
}