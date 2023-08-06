#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// Pin definitions for the ST7735 display
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

// Initialize the ST7735 display with the correct pins
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  tft.initR(INITR_144GREENTAB); // Initialize the display
  tft.fillScreen(ST7735_BLACK); // Clear the screen
  tft.drawRect(0, 0, 20, 20, ST7735_WHITE); // Draw a rectangle
  tft.setCursor(30, 10); // Set the text cursor
  tft.setTextColor(ST7735_WHITE); // Set the text color
  tft.setTextSize(2); // Set the text size
  tft.print("Hello!"); // Display some text
}

void loop() {
  // You can add your sensor code and update the display here
}
