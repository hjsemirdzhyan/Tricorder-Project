#include "TouchScreen.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"

extern TouchScreen ts;
extern Adafruit_ILI9341 tft;

extern float xCalM, yCalM;
extern float xCalC, yCalC;

class DisplayStuff {
public:
  static bool IsTouched();
  static void CalibrateTouch();
};