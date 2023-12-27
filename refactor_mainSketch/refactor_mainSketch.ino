// --------------------------------
// Libraries & Definitions --------
// --------------------------------
#include "TouchScreen.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"

#include "DisplayStuff.h"

#define YP A2
#define XM A3
#define YM 8
#define XP 9
#define TFT_DC 9
#define TFT_CS 10

// --------------------------------
// Classes ------------------------
// --------------------------------




// --------------------------------
// Initializations ----------------
// --------------------------------
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 281);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);



// --------------------------------
// Functions ----------------------
// --------------------------------
void startup() {
  DisplayStuff::CalibrateTouch();
}

void setup() {
  Serial.begin(9600);
  tft.begin();

  startup();
}

// --------------------------------
// Loops --------------------------
// --------------------------------
void loop() {

}
