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

const uint16_t Display_Color = ILI9341_BLUE;
const uint16_t Text_Color = ILI9341_WHITE;
const uint16_t Sel_Color = ILI9341_RED;
const uint16_t Blank_Color = ILI9341_BLACK;
const uint16_t Button_Color = ILI9341_GREEN;

const bool menuDebug = false;
const bool touchDebug = false;
int numOfMenus = 0;  // should move into menu class as a static int
const int calInLay = 20;
const int crosshairSize = 20;


// --------------------------------
// Classes ------------------------
// --------------------------------
class ScreenPoint {
public:
  int16_t x;
  int16_t y;

  ScreenPoint() {
  }

  ScreenPoint(int16_t xIn, int16_t yIn) {
    x = xIn;
    y = yIn;
  }
};

class Button {
  const int _buttonFont = 1;

public:
  int x;
  int y;
  int width;
  int height;
  char* text;

  Button() {
  }

  void initButton(int xPos, int yPos, int butWidth, int butHeight, char* buttonText) {  // why is this not a constructor?
    x = xPos;
    y = yPos;
    width = butWidth;
    height = butHeight;
    text = buttonText;
  }

  void render() {
    tft.fillRect(x, y, width, height, Button_Color);  // draw rectangle
    tft.setCursor(x + 5, y + 5);
    tft.setTextSize(_buttonFont);
    tft.setTextColor(Text_Color);
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

class Menu {
  int _numOfChildren = 0;  //  wont need anymore if array is storing children of parent menu...maybe
  int* _childrenArray;     //  a pointer to an array of addresses of children menu numbers (basically, index numbers)
  static Menu* obj;
  static int _openMenu;      //  variable that stores currently open menu number
  static int _sel_menuItem;  //  number in child list of where selector is
  static int _sel_menuNum;   //  menu number of seleted submenu
  const char* _menuName;     //  this is a pointer to a character array designated by the constructor of this class
  const char* _childOf;
  static const int _fontSpacing = 16;
  static const int _headerFont = 4;
  static const int _childFont = 2;
  static int _childYBound;
  static int _headerYBound;

public:
  Menu(const char menuName[], const char childOf[])
    : _menuName(menuName), _childOf(childOf) {
  }

  void Draw() {
    DrawMenu();
    DrawChildren();
    CalcTouchBounds();
    //DrawTouchBounds();
  }

  void DrawMenu() {
    if (_openMenu >= numOfMenus) {
      BlueScreenOfDeath();
      return;
    }
    int a = 0;
    tft.fillScreen(Display_Color);
    tft.setCursor(0, a);
    tft.setTextColor(Text_Color);
    tft.setTextSize(_headerFont);
    tft.println(_menuName);
    a = tft.getCursorY() + _fontSpacing;
    tft.setCursor(0, a);
    if (menuDebug == true) {
      Serial.print("Method, DrawMenu: ");
      Serial.println(_menuName);
      Serial.print("    Heading cursor x,y are ");
      Serial.print(tft.getCursorX());
      Serial.println(tft.getCursorY());
      Serial.print("    _openMenu: ");
      Serial.println(_openMenu);
    }
  }

  void DrawChildren() {
    if (_childrenArray == nullptr) {
      GenerateChildren();  //  Generate children array only if it hasn't been populated yet (in order to save processing time)
    } else {
      if (menuDebug == true) {
        Serial.println("Skipped, GenerateChildren");
        Serial.println("    _childrenArray is already populated");
      }
    }
    int a = 0;  // cursor related
    int b = _numOfChildren;

    tft.setTextSize(_childFont);
    if (menuDebug == true) {
      Serial.println("Method, DrawChildren");
      Serial.print("    _numOfChildren: ");
      Serial.println(b);
    }
    if (b < 1) {
      if (menuDebug == true) {
        Serial.println("    No Children");
      }
      return;
    } else {
      for (int i = 0; i < b; i++) {
        if (i == _sel_menuItem) {  //  this if statement accounts for which child menu item is currently selected. It highlights it with a unique color
          tft.setTextColor(Sel_Color);
          tft.println(obj[_childrenArray[i]]._menuName);
          a = tft.getCursorY() + _fontSpacing;
          tft.setCursor(0, a);
        } else {  // the else statement draws all the other non-selected child menus with its standard writing color.
          tft.setTextColor(Text_Color);
          tft.println(obj[_childrenArray[i]]._menuName);
          a = tft.getCursorY() + _fontSpacing;
          tft.setCursor(0, a);
        }
        if (menuDebug == true) {
          Serial.print("    Child menu ");
          Serial.print(_childrenArray[i]);
          Serial.print(" ");
          Serial.print(obj[_childrenArray[i]]._menuName);
          Serial.print(" at position ");
          Serial.print(i);
          Serial.print(" cursor x,y: ");
          Serial.print(tft.getCursorX());
          Serial.println(tft.getCursorY());
        }
      }
    }
  }

  static void CalcTouchBounds() {
    const int font1size = 8;
    const int font2size = 16;
    const int font3size = 24;
    const int font4size = 32;
    int headerYBound;
    int childYBound;

    //header bound
    if (_headerFont == 1) {
      headerYBound = font1size + (0.5 * _fontSpacing);
    } else if (_headerFont == 2) {
      headerYBound = font2size + (0.5 * _fontSpacing);
    } else if (_headerFont == 3) {
      headerYBound = font3size + (0.5 * _fontSpacing);
    } else if (_headerFont == 4) {
      headerYBound = font4size + (0.5 * _fontSpacing);
    } else {
      tft.println("Header font not supported");
    }

    _headerYBound = headerYBound;

    if (false == true) {
      Serial.println("Method, SelectorSize ");
      Serial.print("    _headerFont: ");
      Serial.println(_headerFont);
      Serial.print("    headerYBound: ");
      Serial.println(_headerYBound);
    }

    //child bounds
    if (_childFont == 1) {
      childYBound = font1size + _fontSpacing;
    } else if (_childFont == 2) {
      childYBound = font2size + _fontSpacing;
    } else if (_childFont == 3) {
      childYBound = font3size + _fontSpacing;
    } else if (_childFont == 4) {
      childYBound = font4size + _fontSpacing;
    } else {
      tft.println("Child font not supported");
    }

    _childYBound = childYBound;

    if (false == true) {
      Serial.print("    _childFont: ");
      Serial.println(_childFont);
      Serial.print("    childYBound: ");
      Serial.println(_childYBound);
    }
  }

  static void DrawTouchBounds() {  // not for actual use. Only for debuging purposes
    int a = obj[_openMenu]._numOfChildren;

    tft.drawRect(0, 0, tft.width(), _headerYBound, Sel_Color);  // draws bounds around the header menu
    for (int i = 0; i < a; i++) {                               //  draws bounds around child menus
      tft.drawRect(0, _headerYBound + (_childYBound * i), tft.width(), _childYBound, Sel_Color);
    }

    if (menuDebug == true) {
      Serial.println("Method, DrawTouchBounds ");
      Serial.print("    Screen width: ");
      Serial.println(tft.width());
      Serial.print("    Screen height: ");
      Serial.println(tft.height());
    }
  }

  void GenerateChildren() {
    CalcNumOfChildren();  //  generate value for numOfChildren var
    int a = _numOfChildren;
    _childrenArray = new int[a];              //  initialize empty array of child list length. Not sure what this does, not really.
    if (a > 0) {                              //  only scan thru matching menus if menu has at least one child
      int index = 0;                          //  stores where the next child menu number will be stored in the index
      for (int i = 0; i < numOfMenus; i++) {  //  check all menus
        if (obj[i]._childOf == _menuName) {   //  if a menus childOf equals current menus name
          _childrenArray[index] = i;          //  no menu other than zero has been accessed or generated yet so menuNum is zero for all
          index++;                            //  only changes the number if the current if-statement is entered. Used for keeping track of iterations.
        }
      }
    }

    if (menuDebug == true) {
      Serial.println("Method, GenerateChildren");
      Serial.print("    Children of ");
      Serial.print(_menuName);
      Serial.print("{");
      if (a < 1) {
        Serial.println(" }");
      } else {
        for (int i = 0; i < a; i++) {
          Serial.print(_childrenArray[i]);
          if (i < a - 1) {
            Serial.print(", ");
          } else {
            Serial.println("}");
          }
        }
      }
    }
  }

  void CalcNumOfChildren() {
    int numOfChildren = 0;
    if (menuDebug == true) {
      Serial.println("Method, CalcNumOfChildren");
      Serial.print("    numOfMenus: ");
      Serial.println(numOfMenus);
    }
    for (int i = 0; i < numOfMenus; i++) {
      if (obj[i]._childOf == _menuName) {
        numOfChildren++;
        if (menuDebug == true) {
          Serial.print("    Children Count: ");
          Serial.print(numOfChildren);
          Serial.print("    Menu # ");
          Serial.print(i);
          Serial.print(" ");
          Serial.print(obj[i]._menuName);
          Serial.print(" IS the child of ");
          Serial.println(_menuName);
        }
      } else {
        if (menuDebug == true) {
          Serial.print("    Children Count: ");
          Serial.print(numOfChildren);
          Serial.print("    Menu # ");
          Serial.print(i);
          Serial.print(" ");
          Serial.print(obj[i]._menuName);
          Serial.print(" is NOT a child of ");
          Serial.println(_menuName);
        }
      }
    }
    _numOfChildren = numOfChildren;
    if (menuDebug == true) {
      Serial.print("    _numOfChildren = ");
      Serial.println(_numOfChildren);
    }
  }

  char* GetMenuName() {  // returns the menu name of a provided object
    return _menuName;
  }

  static int GetMenuNum(char* menuName) {  // takes in a name and returns its number
    for (int i = 0; i < numOfMenus; i++) {
      if (obj[i]._menuName == menuName) {
        return i;
      }
    }
  }

  char* GetChildOf() {  //  returns name of parent menu
    return _childOf;
  }

  static int GetOpenMenu() {  // returns the index number of the openMenu, not its name
    return _openMenu;
  }

  static void SetOpenMenu(char menuName) {  //  sets array index number (menuNumber) of menu given the menus name of menu that's meant to be open.
    for (int i = 0; i < numOfMenus; i++) {  //  need to convert a menu name to it's array index value
      if (obj[i]._menuName == menuName) {
        _openMenu = i;
      }
    }
    if (menuDebug == true) {
      Serial.println("Method, SetOpenMenu");
      Serial.print("    Open menu #: ");
      Serial.println(_openMenu);
    }
  }

  static void SetObj(Menu* _obj) {
    obj = _obj;
  }

  static void SetSelMenu(int menuItem) {  //  takes in the menuItem and sets menuNumber
   
    if (obj[_openMenu]._numOfChildren == 0) {
      _sel_menuNum = _openMenu;
    } else {
      _sel_menuNum = obj[_openMenu]._childrenArray[menuItem];
    }
    _sel_menuItem = menuItem;

    if (menuDebug == true) {
      Serial.println("Method, SetSelMenu");
      Serial.print("    _openMenu: ");
      Serial.println(_openMenu);
      Serial.print("    Open Menu Name: ");
      Serial.println(obj[_openMenu]._menuName);
      Serial.print("    obj[_openMenu]._numOfChildren: ");
      Serial.println(obj[_openMenu]._numOfChildren);
      Serial.print("    _sel_MenuNum: ");
      Serial.println(_sel_menuNum);
      Serial.print("    _sel_menuItem: ");
      Serial.println(_sel_menuItem);
    }
  }

  static void SetSelMenu1(ScreenPoint sp) {
    if (menuDebug == true) {
      Serial.println("Method, SetSelMenu1");
      Serial.print("    LCD X: ");
      Serial.print(sp.x);
      Serial.print("\tLCD Y: ");
      Serial.println(sp.y);
      delay(500);
    }
    if (sp.y <= _headerYBound + (_childYBound * 0)) {  // wont account for menu lengths greater than 6. wont account for less than 6 either.
      return;
    } else if (sp.y <= _headerYBound + (_childYBound * 1)) {  // env
      SetSelMenu(0);
    } else if (sp.y <= _headerYBound + (_childYBound * 2)) {
      SetSelMenu(1);
    } else if (sp.y <= _headerYBound + (_childYBound * 3)) {
      SetSelMenu(2);
    } else if (sp.y <= _headerYBound + (_childYBound * 4)) {
      SetSelMenu(3);
    } else if (sp.y <= _headerYBound + (_childYBound * 5)) {
      SetSelMenu(4);
    } else if (sp.y <= _headerYBound + (_childYBound * 6)) {
      SetSelMenu(5);
    }
    obj[_openMenu].Draw();
  }

  static int GetSelMenuItem() {
    return _sel_menuItem;
  }

  static int GetSelMenuNum() {
    return _sel_menuNum;
  }

  int GetNumOfChildren() {
    return _numOfChildren;
  }

  int* GetChildrenArray() {
    if (false == true) {
      Serial.println("Method, GetChildrenArray");
      Serial.print("    Returned: ");
      for (int i = 0; i < _numOfChildren; i++) {
        Serial.print(_childrenArray[i]);
        Serial.print(" ");
      }
      Serial.println(" ");
    }
    return _childrenArray;
  }

  static void OpenSelected() {  // needs to get the selected menu number and then pass that menu number into the draw method.
    _openMenu = _sel_menuNum;
    ;
    SetSelMenu(0);
    obj[_openMenu].Draw();
  }

  static void GoBack() {
    int a = GetMenuNum(obj[_openMenu].GetChildOf());
    _openMenu = a;
    if (menuDebug == true) {
      Serial.println("Method, GoBack");
      Serial.print("    _openMenu: ");
      Serial.println(_openMenu);
    }
    SetSelMenu(0);
    obj[_openMenu].Draw();
  }

  static void BlueScreenOfDeath() {
    tft.fillScreen(Blank_Color);
    tft.setCursor(0, tft.height() / 2);
    tft.setTextColor(Sel_Color);
    tft.setTextSize(_headerFont);
    tft.println("Oopse, things happened   =/");
  }
};

// --------------------------------
// Initializations ----------------
// --------------------------------
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

Button sel;
Button back;

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

  sel.initButton(tft.width() - 50, tft.height() - 30, 50, 30, "Select");
  back.initButton(0, tft.height() - 30, 50, 30, "Back");
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