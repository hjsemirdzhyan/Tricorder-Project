#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS 10
#define TFT_RST -1
#define TFT_DC 8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const uint8_t Button_pin = 2;

const uint16_t Display_Color_Black = 0x0000;
const uint16_t Display_Color_Blue = 0x001F;
const uint16_t Display_Color_Red = 0xF800;
const uint16_t Display_Color_Green = 0x07E0;
const uint16_t Display_Color_Cyan = 0x07FF;
const uint16_t Display_Color_Magenta = 0xF81F;
const uint16_t Display_Color_Yellow = 0xFFE0;
const uint16_t Display_Color_White = 0xFFFF;

uint16_t Display_Text_Color = Display_Color_White;
uint16_t Display_Backround_Color = Display_Color_Black;

bool isDisplayVisible = false;

void myLCDTest() {
  tft.setCursor(0, 0);
  tft.setTextColor(Display_Text_Color);
  tft.print("Hello, Jack");

}

void setup() {
  tft.initR(INITR_144GREENTAB);  // Init ST7735R chip, green tab
  tft.setFont();
  tft.fillScreen(Display_Backround_Color);
  tft.setTextColor(Display_Text_Color);
  tft.setTextSize(1);
  isDisplayVisible = true;
}

void loop() {
  myLCDTest();
  tft.enableDisplay(isDisplayVisible);
}
