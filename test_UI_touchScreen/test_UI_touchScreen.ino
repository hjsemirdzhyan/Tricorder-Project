/*
   pin outs:
    LCD___Mega___Uno
    CLK----D52---D13
    MISO---D50---D12
    MOSI---D51---D11
    CS-----D10---D10
    D/C----D9----D9
    YPos---A2----A2
    XNeg---A3----A3
    YNeg---D8----D8
    Xpos---D9----D9
*/

// -----------------------------------------------------------------------------------------------------------
// Libraries & Definitions -----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "TouchScreen.h"

//  Touch Screen stuff
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be any digital pin
#define XP 9   // can be any digital pin

// calibration values
float xCalM = 0.0, yCalM = 0.0;  // gradients
float xCalC = 0.0, yCalC = 0.0;  // y axis crossing points

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 281);

//  LCD stuff
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


// -----------------------------------------------------------------------------------------------------------
// Classes ---------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
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
    int a = 0;
    int b = _numOfChildren;

    tft.setTextSize(_childFont);
    if (menuDebug == true) {
      Serial.println("Method, DrawChildren");
      Serial.print("    Output of var a: ");
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

    if (menuDebug == true) {
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

    if (menuDebug == true) {
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

  char GetMenuName() {  // returns the menu name of a provided object
    return _menuName;
  }

  char GetChildOf() {
    return _childOf;
  }

  static int GetOpenMenu() {  // returns the index number of the openMenu, not its name
    if (false == true) {
      Serial.println("Method, GetOpenMenu");
      Serial.print("    Open menu #: ");
      Serial.println(_openMenu);
    }
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

  static void SetSelMenu(int menuItem) {
    SetSelMenuItem(menuItem);
    SetSelMenuNum(menuItem);
  }

  static void SetSelMenuItem(int menuItem) {  // i think this is now obsolete for touchscreen use
    int a = menuItem;
    _sel_menuItem = a;
    if (menuDebug == true) {
      Serial.println("Method, SetSelMenuItem");
      Serial.print("    Selected menu item #: ");
      Serial.println(_sel_menuItem);
    }
  }

  static void SetSelMenuNum(int menuItem) {  //  takes in the menuItem and sets menuNumber
    int a = obj[GetOpenMenu()]._childrenArray[menuItem];
    _sel_menuNum = a;
    if (true == true) {
      Serial.println("Method, SetSelMenuNum");
      Serial.print("    Selected menu #: ");
      Serial.println(_sel_menuNum);
      Serial.print("    childrenArray[menuNum] equals: ");
      Serial.println(a);
    }
  }

  static void SetSelMenu1(ScreenPoint sp) {
    char a = obj[_sel_menuNum]._menuName;  //  grabbing menuy name so i can clear out its selection color before new selection is made

    if (true == true) {
      Serial.println("Method, SetSelMenu1");
      Serial.print("    LCD X: ");
      Serial.print(sp.x);
      Serial.print("\tLCD Y: ");
      Serial.println(sp.y);
      delay(500);
    }

    if (sp.y <= _headerYBound + (_childYBound * 0)) {  // wont account for menu lengths greater than 6
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
    // first, set new active color
    // next, clear out old color
    //_headerYBound + (_childYBound * i)
    //tft.setCursor(50, 100);??
    //tft.setTextSize(_childFont);
    //tft.setTextColor(Text_Color);
    // tft.print(a);
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
    if (menuDebug == true) {
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

  static void GoDown() {  // attempt at moving the godown member into the menu class in order to make coding easier for myself.
    int a = _sel_menuItem;
    int b = a + 1;
    int c = obj[_openMenu]._numOfChildren;  //  get the number of children menus of the open menu
    if (b > c) {                            //  if we try to navigate passed the last child menu, reset to the top
      b = 0;
    }
    SetSelMenu(b);
    obj[_openMenu].Draw();

    if (menuDebug == true) {
      Serial.println("Method, GoDown");
      Serial.print("    _openMenu is ");
      Serial.println(obj[_openMenu]._menuName);
      Serial.print("    Retrieved selected menu item # is (starts from zero): ");
      Serial.println(a);
      Serial.print("    Selected menu item # is now: ");
      Serial.println(_sel_menuItem);
      Serial.print("    Which is named ");
      Serial.println(obj[_sel_menuNum]._menuName);
      Serial.print("    Retrieved number of children is (doesnt start from zero): ");
      Serial.println(c);
    }
  }

  static void OpenSelected() {  // needs to get the selected menu number and then pass that menu number into the draw method.
    int a = _sel_menuNum;
    obj[a].Draw();

    if (menuDebug == true) {
      Serial.println("Method, OpenSelected");
      Serial.print("    _sel_menuNum is ");
      Serial.print(a);
      Serial.print(" which is named ");
      Serial.println(obj[a]._menuName);
    }
  }

  static void GoBack() {
    int a = obj[GetOpenMenu()].GetChildOf();
    obj[a].Draw();
  }
};

// -----------------------------------------------------------------------------------------------------------
// Initializations -------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
Menu obj[] = {
  Menu("Main Menu", "None"),
  Menu("Enviroment", "Main Menu"),
  Menu("Temperature", "Enviroment"),
  Menu("Body Profile", "Main Menu"),
  Menu("Humidity", "Enviroment"),
  Menu("Location", "Main Menu"),
  Menu("GPS", "Location"),
  Menu("Barometric Pressure", "Enviroment"),
  Menu("Hello World", "Main Menu"),
  Menu("SubGhz", "Main Menu"),
  Menu("NFC", "SubGhz"),
  Menu("RFID", "SubGhz"),
  Menu("Blutooth", "SubGhz"),
  Menu("Accelerometer", "Enviroment"),
  Menu("Integrated Circuit", "Main Menu"),
};

Menu* Menu::obj = nullptr;  //initialize the static member variable

ScreenPoint sp;

Button sel;
Button back;

int Menu::_openMenu = 0;      //these set the initial values for some of the static variables in the menu class
int Menu::_sel_menuItem = 0;  // 0 being the first entry in the _childrenArray array that's used to populate the parent menu page
int Menu::_sel_menuNum = 1;   //  1 because it corresponds with the first entry in the _childrenArray but since menuNumber is the index number of the obj array, 0 is main menu. 1 is the first child menu. This only holds up if the first menu to be displayed is Main Menu.
int Menu::_childYBound = 0;
int Menu::_headerYBound = 0;

// -----------------------------------------------------------------------------------------------------------
// Functions -------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
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
}

void testing() {
  if (isTouched()) {
    TSPoint touchPoint = ts.getPoint();
    sp = getScreenCoords(touchPoint.x, touchPoint.y);

    Menu::SetSelMenu1(sp);
    renderNavButtons();

    if (sel.isClicked(sp) == true) {
      Menu::OpenSelected();

    }
    if (back.isClicked(sp) == true) {
      Menu::GoBack();
    }

    if (touchDebug == true) {
      Serial.print("    Raw X: ");
      Serial.print(touchPoint.x);
      Serial.print("\tRaw Y: ");
      Serial.println(touchPoint.y);
    }
  }
}

// -----------------------------------------------------------------------------------------------------------
// Loops -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  numOfMenus = sizeof(obj) / sizeof(obj[0]);
  Menu::SetObj(obj);
  startup();
}

void loop() {
  testing();
}