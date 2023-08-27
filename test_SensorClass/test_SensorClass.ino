/*
   pin outs:
    LCD___Mega___Uno___Color
    CLK----D52---D13---Red
    MISO---D50---D12---Orange
    MOSI---D51---D11---Yellow
    CS-----D10---D10---Green
    D/C----D9----D9----Blue
    YPos---A2----A2----Purple
    XNeg---A3----A3----Red
    YNeg---D8----D8----Orange
    Xpos---D9----D9----Blue

    This class will host rendering screens (for now) that the sensors can use to display
    their data on a menu. The class will also host any of the methods needed in order to 
    use multiple sensors to provide results. Essentally, all sensor readings will go thru
    this class, regardless of if it needs to be combined with other sensors or not.

    Specific sensor classes will have functions specific to their own sensor abilities.
    This class can read temp and humidity. The ultrasonic sensor can use those readings 
    to provide more preceise results but that type of functionality will need to happen
    in the SensorSuite class, not in either this class or the ultrasonic class.
*/


// --------------------------------
// Libraries & Definitions --------
// --------------------------------
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "TouchScreen.h"
#include "Button.h"
#include "Menu.h"
#include "dht.h"

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be any digital pin
#define XP 9   // can be any digital pin
#define TFT_DC 9
#define TFT_CS 10

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 281);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
dht DHT; //cant move this into tempHumid.h for some fucking reason

float xCalM = 0.0, yCalM = 0.0;  // gradients
float xCalC = 0.0, yCalC = 0.0;  // y axis crossing points
const bool touchDebug = false;
int numOfMenus = 0;  // should move into menu class as a static int

// --------------------------------
// Classes ------------------------
// --------------------------------
class DelayTracker {
  unsigned long _previousPoll;  // will store last time temp/humid reading was updated
  long _pollTime;               // how often in milliseconds to poll the temp sensor
  long _duration;

public:
  Update(long time) {
    _pollTime = time;
    unsigned long currentTime = millis();

    if (currentTime - _previousPoll >= _pollTime) {
      _previousPoll = currentTime;
      return true;
    } else {
      return false;
    }
  }
};

// --------------------------------
// Initializations ----------------
// --------------------------------
Sensor sensorSuite;
DelayTracker menuRefresh;

Menu obj[] = {
  Menu("Main Menu", "None"),                  //0
  Menu("Enviroment", "Main Menu"),            //1
  Menu("Temp/Humid", "Enviroment", sensorSuite),           //2
  Menu("Body Profile", "Main Menu"),          //3
  Menu("Location", "Main Menu"),              //4
  Menu("GPS", "Location"),                    //5
  Menu("Barometric Pressure", "Enviroment"),  //6
  Menu("Hello World", "Main Menu"),           //7
  Menu("SubGhz", "Main Menu"),                //8
  Menu("NFC", "SubGhz"),                      //9
  Menu("RFID", "SubGhz"),                     //10
  Menu("Blutooth", "SubGhz"),                 //11
  Menu("Accelerometer", "Enviroment"),        //12
  Menu("Ultrasonic", "Main Menu", sensorSuite),    //13
};

Menu* Menu::obj = nullptr;  //initialize the static member variable
ScreenPoint sp;
Button sel(tft.width() - 50, tft.height() - 30, 50, 30, "Select");
Button back(0, tft.height() - 30, 50, 30, "Back");

int Menu::_openMenu = 0;      // these set the initial values for some of the static variables in the menu class
int Menu::_sel_menuItem = 0;  // 0 being the first entry in the _childrenArray array that's used to populate the parent menu page
int Menu::_sel_menuNum = 1;   // 1 because it corresponds with the first entry in the _childrenArray. 1 is the first child menu.
int Menu::_childYBound = 0;
int Menu::_headerYBound = 0;

// --------------------------------
// Functions ----------------------
// --------------------------------
void startup() {
  tft.begin();
  if (menuDebug == true) {
    Serial.println();
    Serial.println();
    Serial.println();
  }
  Serial.print("DEBUGGING IS SET TO ");
  Serial.println(menuDebug);
  calibrateTouchScreen();
  delay(500);
  obj[0].Draw();  // displays the starting menu (by running a lot of other methods first)

  renderNavButtons();
  delay(500);
}

bool isTouched() {
  TSPoint touchPoint = ts.getPoint();  // can this be passed in instead of calling it? any benefits?
  if (touchPoint.z > ts.pressureThreshhold) {
    return true;
  } else {
    return false;
  }
}

ScreenPoint getScreenCoords(int16_t x, int16_t y) {
  int16_t xCoord = round((x * xCalM) + xCalC);
  int16_t yCoord = round((y * yCalM) + yCalC);
  if (xCoord < 0) xCoord = 0;
  if (xCoord >= tft.width()) xCoord = tft.width() - 1;
  if (yCoord < 0) yCoord = 0;
  if (yCoord >= tft.height()) yCoord = tft.height() - 1;
  return (ScreenPoint(xCoord, yCoord));
}

void calibrateTouchScreen() {
  TSPoint touchPoint;  //  x and y of detected calibration inputs
  int16_t x1;
  int16_t y1;
  int16_t x2;
  int16_t y2;

  tft.fillScreen(Blank_Color);
  // wait for no touch
  while (isTouched())
    ;
  tft.drawFastHLine(calInLay - (crosshairSize / 2), calInLay, crosshairSize, Sel_Color);  //  draws first position. cross hairs intersect at x=20, y=20
  tft.drawFastVLine(calInLay, calInLay - (crosshairSize / 2), crosshairSize, Sel_Color);
  while (!isTouched())
    ;
  delay(50);
  touchPoint = ts.getPoint();
  x1 = touchPoint.x;
  y1 = touchPoint.y;
  tft.drawFastHLine(calInLay - (crosshairSize / 2), calInLay, crosshairSize, Blank_Color);  //  draws first position. cross hairs intersect at x=20, y=20
  tft.drawFastVLine(calInLay, calInLay - (crosshairSize / 2), crosshairSize, Blank_Color);
  delay(500);
  while (isTouched())
    ;
  tft.drawFastHLine(tft.width() - crosshairSize - calInLay + crosshairSize / 2, tft.height() - calInLay, crosshairSize, Sel_Color);  // define this better
  tft.drawFastVLine(tft.width() - calInLay, tft.height() - calInLay - crosshairSize / 2, crosshairSize, Sel_Color);
  while (!isTouched())
    ;
  delay(50);
  touchPoint = ts.getPoint();
  x2 = touchPoint.x;
  y2 = touchPoint.y;
  tft.drawFastHLine(tft.width() - crosshairSize - calInLay + crosshairSize / 2, tft.height() - calInLay, crosshairSize, Blank_Color);  // define this better
  tft.drawFastVLine(tft.width() - calInLay, tft.height() - calInLay - crosshairSize / 2, crosshairSize, Blank_Color);

  int16_t xDist = tft.width() - (2 * calInLay);   // the 40 is the sum of the inlay point from both points
  int16_t yDist = tft.height() - (2 * calInLay);  // y distance of the two calibration points that the TS SHOULD match with

  // translate in form pos = m x val + c
  // x
  xCalM = (float)xDist / (float)(x2 - x1);
  xCalC = calInLay - ((float)x1 * xCalM);
  // y
  yCalM = (float)yDist / (float)(y2 - y1);
  yCalC = calInLay - ((float)y1 * yCalM);  // I THINK THE 20 IS WHERE the cross hairs are meant to point

  if (touchDebug == true) {
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
}

void renderNavButtons() {
  sel.render();
  back.render();
  //openMenuVars();
}

void touchDetect() {
  if (isTouched()) {  // only runs when touch is detected
    TSPoint touchPoint = ts.getPoint();
    sp = getScreenCoords(touchPoint.x, touchPoint.y);

    if (sel.isClicked(sp) == false && back.isClicked(sp) == false) {  // this happens when something on the screen is touched other than the buttons
      Menu::SetSelMenu1(sp);
      renderNavButtons();
    } else if (sel.isClicked(sp) == true) {  // this happens when select is clicked
      Menu::OpenSelected();
      renderNavButtons();
    } else if (back.isClicked(sp) == true) {  // this happens when back is clicked
      Menu::GoBack();
      renderNavButtons();
    }

    if (touchDebug == true) {
      Serial.print("    Raw X: ");
      Serial.print(touchPoint.x);
      Serial.print("\tRaw Y: ");
      Serial.println(touchPoint.y);
    }
  }
}

void openMenuVars() {
  Serial.println("OPEN MENU VARIABLES REPORT");
  Serial.print("    Menu Number: ");
  Serial.println(Menu::GetOpenMenu());
  Serial.print("    Menu Name: ");
  Serial.println(obj[Menu::GetOpenMenu()].GetMenuName());
  Serial.print("    Number of Children: ");
  Serial.println(obj[Menu::GetOpenMenu()].GetNumOfChildren());
  Serial.print("    Children Array: ");
  for (int i = 0; i < obj[Menu::GetOpenMenu()].GetNumOfChildren(); i++) {
    Serial.print(obj[Menu::GetOpenMenu()].GetChildrenArray()[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("    Parent Menu: ");
  Serial.println(obj[Menu::GetOpenMenu()].GetChildOf());
  Serial.print("    Parent Menu Number: ");
  Serial.println(Menu::GetMenuNum(obj[Menu::GetOpenMenu()].GetChildOf()));
  Serial.print("    Selected Menu: ");
  Serial.println(obj[Menu::GetSelMenuNum()].GetMenuName());
  Serial.print("    Selected Menu Item: ");
  Serial.println(Menu::GetSelMenuItem());
  Serial.print("    Selected Menu Number: ");
  Serial.println(Menu::GetSelMenuNum());
}

void sensorPoll () {
  if (menuRefresh.Update(500) == true) {
    int openMenu = Menu::GetOpenMenu();
    if (obj[openMenu].HasSensor() == true) {
      obj[openMenu].CallSensor();
    } else {
      return;
    }
  }
}

// --------------------------------
// Loops --------------------------
// --------------------------------
void setup() {
  Serial.begin(9600);
  numOfMenus = sizeof(obj) / sizeof(obj[0]);
  Menu::SetObj(obj);
  startup();
}

void loop() {
  touchDetect();
  sensorPoll();
}