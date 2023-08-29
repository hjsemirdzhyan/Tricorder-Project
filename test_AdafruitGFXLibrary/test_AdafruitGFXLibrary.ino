#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
  Serial.begin(9600);
  tft.begin();
}

void loop() {
  while (millis() <= 3000) {
    startup();
  }
  if (millis() > 5000) {
    tft.fillRect(0, 50, 128, 14, ILI9341_BLACK);
    
  } else if (millis() > 3000) {
    tft.setCursor(20, 50);
    tft.print("Done Deal");
    Serial.print(tft.getCursorX());
    Serial.print(", ");
    Serial.println(tft.getCursorY());
  }
}

void startup() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 20);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.print("Hello World");
}