#include "DisplayStuff.h"

float xCalM = 0.0, yCalM = 0.0;
float xCalC = 0.0, yCalC = 0.0;

bool DisplayStuff::IsTouched() {
  TSPoint touchPoint = ts.getPoint();
  if (touchPoint.z > ts.pressureThreshhold) {
    return true;
  } else {
    return false;
  }
}

void DisplayStuff::CalibrateTouch() {
  TSPoint touchPoint;
  int16_t x1;
  int16_t y1;
  int16_t x2;
  int16_t y2;

  const int calInLay = 20;
  const int crosshairSize = 20;

  const uint16_t Sel_Color = ILI9341_RED;
  const uint16_t Blank_Color = ILI9341_BLACK;

  tft.fillScreen(Blank_Color);

  while (IsTouched())
    ;
  tft.drawFastHLine(calInLay - (crosshairSize / 2), calInLay, crosshairSize, Sel_Color);
  tft.drawFastVLine(calInLay, calInLay - (crosshairSize / 2), crosshairSize, Sel_Color);
  
  while (!IsTouched())
    ;
  delay(50);
 
  touchPoint = ts.getPoint();
  x1 = touchPoint.x;
  y1 = touchPoint.y;

  tft.drawFastHLine(calInLay - (crosshairSize / 2), calInLay, crosshairSize, Blank_Color);
  tft.drawFastVLine(calInLay, calInLay - (crosshairSize / 2), crosshairSize, Blank_Color);
  delay(500);
  
  while (IsTouched())
    ;
  tft.drawFastHLine(tft.width() - crosshairSize - calInLay + crosshairSize / 2, tft.height() - calInLay, crosshairSize, Sel_Color);
  tft.drawFastVLine(tft.width() - calInLay, tft.height() - calInLay - crosshairSize / 2, crosshairSize, Sel_Color);
  
  while (!IsTouched())
    ;
  delay(50);
  
  touchPoint = ts.getPoint();
  x2 = touchPoint.x;
  y2 = touchPoint.y;
  
  tft.drawFastHLine(tft.width() - crosshairSize - calInLay + crosshairSize / 2, tft.height() - calInLay, crosshairSize, Blank_Color);
  tft.drawFastVLine(tft.width() - calInLay, tft.height() - calInLay - crosshairSize / 2, crosshairSize, Blank_Color);

  int16_t xDist = tft.width() - (2 * calInLay);
  int16_t yDist = tft.height() - (2 * calInLay);

  xCalM = (float)xDist / (float)(x2 - x1);
  xCalC = calInLay - ((float)x1 * xCalM);

  yCalM = (float)yDist / (float)(y2 - y1);
  yCalC = calInLay - ((float)y1 * yCalM);

}
