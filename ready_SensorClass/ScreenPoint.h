#pragma once
#include "SPI.h"

class ScreenPoint {
public:
  int16_t x;
  int16_t y;

  ScreenPoint();
  ScreenPoint(int16_t xIn, int16_t yIn);
};