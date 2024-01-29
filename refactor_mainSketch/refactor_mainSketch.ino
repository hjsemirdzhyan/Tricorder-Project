// --------------------------------
// Libraries & Definitions --------
// --------------------------------
#include "TouchScreen.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"

#include "DisplayStuff.h"
#include "UserInterface.h"

#define YP A2
#define XM A3
#define YM 8
#define XP 9
#define TFT_DC 9
#define TFT_CS 10

// --------------------------------
// Initializations ----------------
// --------------------------------
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 281);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
/*UserInterface menu[] = {
  UserInterface("Main Menu", "None"),
  UserInterface("Environment", "Main Menu"),
  UserInterface("Temp/Humid", "Environment"),
  UserInterface("Body Profile", "Main Menu"),
  UserInterface("Location", "Main Menu"),
  UserInterface("GPS", "Location"),
  UserInterface("Barometric Pressure", "Environment"),
  UserInterface("Hello World", "Main Menu"),
  UserInterface("SubGhz", "Main Menu"),
  UserInterface("NFC", "SubGhz"),
  UserInterface("RFID", "SubGhz"),
  UserInterface("Blutooth", "SubGhz"),
  UserInterface("Accelerometer", "Environment"),
  UserInterface("Ultrasonic", "Main Menu"),
};*/

// --------------------------------
// Classes ------------------------
// --------------------------------






// --------------------------------
// Functions ----------------------
// --------------------------------
void setup() {
  Serial.begin(9600);
  tft.begin();
  DisplayStuff::CalibrateTouch();
  numOfMenus = sizeof(menu) / sizeof(menu[0]); //<--- consider moving this into a separate .cpp file as a standalone function. 
  UserInterface::SetObj(menu);

}

// --------------------------------
// Loops --------------------------
// --------------------------------
void loop() {

}
