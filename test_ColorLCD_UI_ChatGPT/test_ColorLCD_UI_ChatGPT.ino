#include <Adafruit_ST7735.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST -1

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const int adcPin = A0;
const int sw1Threshold = 15;
const int sw2Threshold = 155;
const int sw3Threshold = 340;
const int sw4Threshold = 515;
const int sw5Threshold = 760;
const int naThreshold = 1024;

int selectedOption = 0;
int numOptions = 4;

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("Menu:");
  tft.setTextSize(1);
  tft.println("Option 1");
  tft.println("Option 2");
  tft.println("Option 3");
  tft.println("Option 4");
  tft.drawRect(0, (selectedOption * 10) + 22, 128, 10, ST7735_WHITE);
}

void loop() {
  int adcValue = analogRead(adcPin);

  if (adcValue < sw1Threshold) {
    // Move up in menu
    if (selectedOption > 0) {
      selectedOption--;
      redrawMenu();
    }
  } else if (adcValue >= sw1Threshold && adcValue < sw2Threshold) {
    // Select option
    selectOption();
  } else if (adcValue >= sw2Threshold && adcValue < sw3Threshold) {
    // Do nothing
  } else if (adcValue >= sw3Threshold && adcValue < sw4Threshold) {
    // Do nothing
  } else if (adcValue >= sw4Threshold && adcValue < sw5Threshold) {
    // Move down in menu
    if (selectedOption < numOptions - 1) {
      selectedOption++;
      redrawMenu();
    }
  } else if (adcValue >= sw5Threshold && adcValue < naThreshold) {
    // Do nothing
  }
}

void redrawMenu() {
  // Redraw menu with new selection
  tft.fillRect(0, 22, 128, 60, ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("Menu:");
  tft.setTextSize(1);
  tft.println("Option 1");
  tft.println("Option 2");
  tft.println("Option 3");
  tft.println("Option 4");
  tft.drawRect(0, (selectedOption * 10) + 22, 128, 10, ST7735_WHITE);
}

void selectOption() {
  // Perform action based on selected option
  switch (selectedOption) {
    case 0:
      // Option 1 selected
      Serial.println("Option 1 selected");
      break;
    case 1:
      // Option 2 selected
      Serial.println("Option 2 selected");
      break;
    case 2:
      // Option 3 selected
      Serial.println("Option 3 selected");
      break;
    case 3:
      Serial.println("Option 4 selected");
      break;
  }
}