/*
  Adapted from https://bytesnbits.co.uk/arduino-touchscreen-calibration-coding/#1606910525368-25b26a28-b2d4
  Note that code on website is broken due to greater than, less than, etc, not formatting correctly. It has beed addressed in this code. 
  Also note that the library used in that code was replaced with Adafruits TouchScreen.h library which does not have a function for isTouched(). 
  A local function was written just to get the code running. It was simple enough to implement so should be no problem not having it in the library. 
*/


#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "TouchScreen.h"
#include "Math.h"

// For the Adafruit shield, these are the default.
#define TFT_CS 10
#define TFT_DC 9

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be any digital pin
#define XP 9   // can be any digital pin


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC/RST
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 320);

// calibration values
float xCalM = 0.0, yCalM = 0.0;      // gradients
float xCalC = 0.0, yCalC = 0.0;  // y axis crossing points

int8_t blockWidth = 20;  // block size
int8_t blockHeight = 20;
int16_t blockX = 0, blockY = 0;  // block position (pixels)

class ScreenPoint {
public:
  int16_t x;
  int16_t y;

  ScreenPoint() {
    // default contructor
  }

  ScreenPoint(int16_t xIn, int16_t yIn) {
    x = xIn;
    y = yIn;
  }
};

class Button {
public:
  int x;
  int y;
  int width;
  int height;
  char *text;

  Button() {
  }

  void initButton(int xPos, int yPos, int butWidth, int butHeight, char *butText) {
    x = xPos;
    y = yPos;
    width = butWidth;
    height = butHeight;
    text = butText;
    render();
  }

  void render() {
    tft.fillRect(x, y, width, height, ILI9341_GREEN);  // draw rectangle
    tft.setCursor(x + 5, y + 5);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(text);
  }

  bool isClicked(ScreenPoint sp) {
    if ((sp.x >= x) && (sp.x <= (x + width)) && (sp.y >= y) && (sp.y <= (y + height))) {
      return true;
    } else {
      return false;
    }
  }
};

ScreenPoint getScreenCoords(int16_t x, int16_t y) {
  int16_t xCoord = round((x * xCalM) + xCalC);
  int16_t yCoord = round((y * yCalM) + yCalC);
  if (xCoord < 0) xCoord = 0;
  if (xCoord >= tft.width()) xCoord = tft.width() - 1;
  if (yCoord < 0) yCoord = 0;
  if (yCoord >= tft.height()) yCoord = tft.height() - 1;
  return (ScreenPoint(xCoord, yCoord));
}

bool isTouched() {
  TSPoint p = ts.getPoint();
  if (p.z > ts.pressureThreshhold) {
    return true;
  } else {
    return false;
  }
}

void calibrateTouchScreen() {
  TSPoint p;
  int16_t x1, y1, x2, y2;

  tft.fillScreen(ILI9341_BLACK);
  // wait for no touch
  while (isTouched())
    ;
  tft.drawFastHLine(10, 20, 20, ILI9341_RED);
  tft.drawFastVLine(20, 10, 20, ILI9341_RED);
  while (!isTouched())
    ;
  delay(50);
  p = ts.getPoint();
  x1 = p.x;
  y1 = p.y;
  tft.drawFastHLine(10, 20, 20, ILI9341_BLACK);
  tft.drawFastVLine(20, 10, 20, ILI9341_BLACK);
  delay(500);
  while (isTouched())
    ;
  tft.drawFastHLine(tft.width() - 30, tft.height() - 20, 20, ILI9341_RED);
  tft.drawFastVLine(tft.width() - 20, tft.height() - 30, 20, ILI9341_RED);
  while (!isTouched())
    ;
  delay(50);
  p = ts.getPoint();
  x2 = p.x;
  y2 = p.y;
  tft.drawFastHLine(tft.width() - 30, tft.height() - 20, 20, ILI9341_BLACK);
  tft.drawFastVLine(tft.width() - 20, tft.height() - 30, 20, ILI9341_BLACK);

  int16_t xDist = tft.width() - 40;
  int16_t yDist = tft.height() - 40;

  // translate in form pos = m x val + c
  // x
  xCalM = (float)xDist / (float)(x2 - x1);
  xCalC = 20.0 - ((float)x1 * xCalM);
  // y
  yCalM = (float)yDist / (float)(y2 - y1);
  yCalC = 20.0 - ((float)y1 * yCalM);

  Serial.print("x1 = ");
  Serial.print(x1);
  Serial.print(", y1 = ");
  Serial.println(y1);
  Serial.print("x2 = ");
  Serial.print(x2);
  Serial.print(", y2 = ");
  Serial.println(y2);
  Serial.print("xCalM = ");
  Serial.print(xCalM);
  Serial.print(", xCalC = ");
  Serial.println(xCalC);
  Serial.print("yCalM = ");
  Serial.print(yCalM);
  Serial.print(", yCalC = ");
  Serial.println(yCalC);
}

Button button;

void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  calibrateTouchScreen();
  button.initButton(50, 50, 125, 30, "Click Me");
}

unsigned long lastFrame = millis();

void loop(void) {
  ScreenPoint sp;
  // limit frame rate
  while ((millis() - lastFrame) < 20)
    ;
  lastFrame = millis();

  if (isTouched()) {
    TSPoint p = ts.getPoint();
    sp = getScreenCoords(p.x, p.y);
    if (button.isClicked(sp)) {
      tft.setCursor(50, 100);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_WHITE);
      tft.print("Clicked");
    } else {
      tft.fillRect(50, 100, 125, 30, ILI9341_BLACK);  // draw rectangle
    }
  } else {
    tft.fillRect(50, 100, 125, 30, ILI9341_BLACK);  // draw rectangle
  }
}