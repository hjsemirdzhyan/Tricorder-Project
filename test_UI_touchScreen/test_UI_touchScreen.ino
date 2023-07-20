/*
  Emulator can be found at: https://wokwi.com/projects/308024602434470466
  C++ tutorial/examples: https://www.w3schools.com/cpp/cpp_oop.asp


  In this refactor of the UI code, I'm getting rid of the hard coded menu numbers and positions.
  The menu numbers will be calculated on the fly depending on the number of menus and where in the array they're added.
  Similarily, I'll rely on where in the array a menu is added to determine its order on its parent menus page.
  I think this will GREATLY simplify the code.

  Would I rather calculate the list of submenus for each menu on the fly or do it once and store it in memory?
  Arduino Uno Flash Memory: 32 KB = 32,768 bytes. Array memory is 4 bytes per element.

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

//  calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//  LCD stuff
#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

const uint16_t Display_Color = ILI9341_BLUE;
const uint16_t Text_Color = ILI9341_WHITE;
const uint16_t Sel_Color = ILI9341_RED;

bool debug = true;
int numOfMenus = 0;  // should move into menu class as a static int

int y_Cursor1 = 8;
int y_Cursor2 = 15;




// -----------------------------------------------------------------------------------------------------------
// Classes ---------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
class Menu {
  int _numOfChildren = 0;  //  wont need anymore if array is storing children of parent menu...maybe
  int* _childrenArray;     //  a pointer to an array of addresses of children menu numbers (basically, index numbers)
  static Menu* obj;
  static int _openMenu;      //  variable that stores currently open menu number
  int _sel_yPos = 15;        //  y position of selected submenu
  static int _sel_menuItem;  //  number in child list of where selector is
  static int _sel_menuNum;   //  menu number of seleted submenu
  const char* _menuName;     //  this is a pointer to a character array designated by the constructor of this class
  const char* _childOf;
  const int _ySpacing = 4;

public:
  Menu(const char menuName[], const char childOf[])
    : _menuName(menuName), _childOf(childOf) {
  }

  void
  Draw() {
    DrawMenu();
    DrawChildren();
  }

  void DrawMenu() {
    int a = 0;
    
    tft.fillScreen(Display_Color);
    tft.setCursor(0, a);
    tft.setTextColor(Text_Color);
    tft.setTextSize(4);
    tft.println(_menuName);
    a = tft.getCursorY() + _ySpacing;
    tft.setCursor(0, a);
    if (debug == true) {
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
      if (debug == true) {
        Serial.println("Skipped, GenerateChildren");
        Serial.println("    _childrenArray is already populated");
      }
    }
    int a = tft.getCursorY();
    int b = _numOfChildren;

    tft.setTextSize(2);
    if (debug == true) {
      Serial.println("Method, DrawChildren");
      Serial.print("    Output of var a: ");
      Serial.println(a);
    }
    if (b < 1) {
      if (debug == true) {
        Serial.println("    No Children");
      }
      return;
    } else {
      for (int i = 0; i < b; i++) {
        if (i == _sel_menuItem) { //  this if statement accounts for which child menu item is currently selected. It highlights it with a unique color
          tft.setTextColor(Sel_Color);
          tft.println(obj[_childrenArray[i]]._menuName);
          a = a + _ySpacing;
          tft.setCursor(0, a);
        } else { // the else statement draws all the other non-selected child menus with its standard writing color.
          tft.setTextColor(Text_Color);
          tft.println(obj[_childrenArray[i]]._menuName);
        }
        if (debug == true) {
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

    if (debug == true) {
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
    if (debug == true) {
      Serial.println("Method, CalcNumOfChildren");
      Serial.print("    numOfMenus: ");
      Serial.println(numOfMenus);
    }
    for (int i = 0; i < numOfMenus; i++) {
      if (obj[i]._childOf == _menuName) {
        numOfChildren++;
        if (debug == true) {
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
        if (debug == true) {
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
    if (debug == true) {
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
    if (debug == true) {
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

  static void SetSelMenuItem(int menuItem) {
    int a = menuItem;
    _sel_menuItem = a;
    if (debug == true) {
      Serial.println("Method, SetSelMenuItem");
      Serial.print("    Selected menu item #: ");
      Serial.println(_sel_menuItem);
    }
  }

  static void SetSelMenuNum(int menuItem) {  //  takes in the menuItem and sets menuNumber
    int a = obj[GetOpenMenu()]._childrenArray[menuItem];
    _sel_menuNum = a;
    if (debug == true) {
      Serial.println("Method, SetSelMenuNum");
      Serial.print("    Selected menu #: ");
      Serial.println(_sel_menuNum);
      Serial.print("    childrenArray[menuNum] equals: ");
      Serial.println(a);
    }
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
    if (debug == true) {
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

    if (debug == true) {
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

    if (debug == true) {
      Serial.println("Method, OpenSelected");
      Serial.print("    _sel_menuNum is ");
      Serial.print(a);
      Serial.print(" which is named ");
      Serial.println(obj[a]._menuName);
    }
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

int Menu::_openMenu = 0;      //these set the initial values for some of the static variables in the menu class
int Menu::_sel_menuItem = 0;  // 0 being the first entry in the _childrenArray array that's used to populate the parent menu page
int Menu::_sel_menuNum = 1;   //  1 because it corresponds with the first entry in the _childrenArray but since menuNumber is the index number of the obj array, 0 is main menu. 1 is the first child menu. This only holds up if the first menu to be displayed is Main Menu.

// -----------------------------------------------------------------------------------------------------------
// Functions -------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void startup() {
  tft.begin();
  if (debug == true) {
    Serial.println();
    Serial.println();
    Serial.println();
  }
  Serial.print("DEBUGGING IS SET TO ");
  Serial.println(debug);
  obj[0].Draw();  // displays the starting menu (by running a lot of other methods first)
  delay(500);
}

void testing() {
  TSPoint p = ts.getPoint();
  if (p.z > ts.pressureThreshhold) {
    Serial.print("X = ");
    Serial.print(p.x);
    Serial.print("\tY = ");
    Serial.print(p.y);
    Serial.print("\tPressure = ");
    Serial.println(p.z);
  }
  delay(100);
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