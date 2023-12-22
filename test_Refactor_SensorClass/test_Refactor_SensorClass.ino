/*
Notes: 12/22/23
Due to the growing complexity of using individuallized .h and .cpp files I've decided
to only use this method for the menu class. Starting with the Sensor class, I need to move 
the remainder of the classes back into this main sketch. I'll decide whether 
to leave anything else out of the main sketch at a later date. But for now I need
to move Sensor, Ultrasonic, and TempHumid back into here. 
*/

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

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be any digital pin
#define XP 9   // can be any digital pin

float xCalM = 0.0, yCalM = 0.0;  // gradients
float xCalC = 0.0, yCalC = 0.0;  // y axis crossing points

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 281);

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

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

class TempHumid {
  unsigned long _previousPoll;  // will store last time temp/humid reading was updated
  long _pollTime;               // how often in milliseconds to poll the temp sensor
  long _duration;
  double _temp;
  double _humid;

public:
  void PrintTempData() {
    //CalcTempHumid(); make independend call before menu class.
    tft.setCursor(0, 80);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
    tft.print("Temp: ");
    tft.print(_temp);
    tft.print("C ");
    tft.print((_temp * 1.800) + 32.00);
    tft.println("F ");
    tft.print("Humid: ");
    tft.print(_humid);
    tft.println("%");
  }
  double CalcTempHumid() {
    long delay = 2000;

    if (Update(delay) == true) {
      DHT.read11(dht_apin);  // remember to limit poll frequency
      _temp = DHT.temperature;
      _humid = DHT.humidity;
    }
  }
};

class Sensor { //currently in progress.
  TempHumid tempHumid1;
  Ultrasonic ultrasonic1;
  int _message = 0;
public:

};

// --------------------------------
// Initializations ----------------
// --------------------------------
DelayTracker dt;

Menu obj[] = {
  Menu("Main Menu", "None"),                  //0
  Menu("Enviroment", "Main Menu"),            //1
  Menu("Temperature", "Enviroment"),          //2
  Menu("Body Profile", "Main Menu"),          //3
  Menu("Humidity", "Enviroment"),             //4
  Menu("Location", "Main Menu"),              //5
  Menu("GPS", "Location"),                    //6
  Menu("Barometric Pressure", "Enviroment"),  //7
  Menu("Hello World", "Main Menu"),           //8
  Menu("SubGhz", "Main Menu"),                //9
  Menu("NFC", "SubGhz"),                      //10
  Menu("RFID", "SubGhz"),                     //11
  Menu("Blutooth", "SubGhz"),                 //12
  Menu("Accelerometer", "Enviroment"),        //13
  Menu("Integrated Circuit", "Main Menu"),    //14
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

  //sel.initButton(tft.width() - 50, tft.height() - 30, 50, 30, "Select");
  //back.initButton(0, tft.height() - 30, 50, 30, "Back");
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

void menuRefresh() {
  int openMenu = Menu::GetOpenMenu();
  if (openMenu == 13) {
    //start ultrasonic polling
  }

  if (dt.Update(500) == true) {
    //call draw function of the openMenu
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
}