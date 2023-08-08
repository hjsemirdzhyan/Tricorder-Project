#pragma once
#include "Adafruit_ILI9341.h"
#include "ScreenPoint.h"

extern Adafruit_ILI9341 tft;
const uint16_t Button_Color = ILI9341_GREEN;
const uint16_t ButtonText_Color = ILI9341_WHITE;

class Button {
  const int _buttonFont = 1;
public:
  int x;
  int y;
  int width;
  int height;
  char* text;

  //Button();
  Button(int xPos, int yPos, int butWidth, int butHeight, char* buttonText);
  void render();
  bool isClicked(ScreenPoint sp);
};