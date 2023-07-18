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
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define btn_apin A0
#define RANGE 10
#define SW1 RANGE
#define SW2 155
#define SW3 340
#define SW4 515
#define SW5 760

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
  int* _childrenArray;     //  array of addresses to children menu numbers
  static Menu* obj;
  static int _openMenu;      //  variable that stores currently open menu number
  int _sel_yPos = 15;        //  y position of selected submenu
  static int _sel_menuItem;  //  number in child list of where selector is
  static int _sel_menuNum;   //  menu number of seleted submenu
  char _menuName[30];
  char _childOf[30];

public:
  Menu(char menuName[], char childOf[])
    : _menuName(menuName), _childOf(childOf) {
}

void
Draw() {
  DrawMenu();
  DrawChildren();
}

void DrawMenu() {
  tft.fillScreen(Display_Color);
  tft.setCursor(0, 0);
  tft.setTextColor(Text_Color);
  tft.setTextSize(2);
  tft.println(_menuName);
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
    Serial.println("Skipped, GenerateChildren");
    Serial.println("    _childrenArray is already populated");
  }
  int a = _numOfChildren;
  tft.setTextSize(1);
  if (debug == true) {
    Serial.println("Method, DrawChildren");
    Serial.print("    Output of var a: ");
    Serial.println(a);
  }
  if (a < 1) {
    if (debug == true) {
      Serial.println("    No Children");
    }
    return;  //sus
  } else {
    for (int i = 0; i < a; i++) {
      if (i == _sel_menuItem) {
        tft.setTextColor(Sel_Color);
        tft.println(obj[_childrenArray[i]]._menuName);
      } else {
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

String GetMenuName() {  // returns the menu name of a provided object
  return _menuName;
}

String GetChildOf() {
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

static void SetOpenMenu(String menuName) {  //  sets array index number (menuNumber) of menu given the menus name of menu that's meant to be open.
  for (int i = 0; i < numOfMenus; i++) {    //  need to convert a menu name to it's array index value
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
}
;

// -----------------------------------------------------------------------------------------------------------
// Initializations -------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
Menu obj[] = {
  Menu("Main Menu", "None"),
  Menu("Enviro", "Main Menu"),
  Menu("Temp", "Enviro"),
  Menu("Body", "Main Menu"),
  Menu("Humidity", "Enviro"),
  Menu("Location", "Main Menu"),
  Menu("GPS", "Location"),
  Menu("Baro", "Enviro"),
  Menu("Hello", "Main Menu"),
  Menu("SubGhz", "Main Menu"),
  Menu("NFC", "SubGhz"),
  Menu("RFID", "SubGhz"),
  Menu("Blu", "SubGhz"),
  Menu("Accel", "Enviro"),
  Menu("IC", "Main Menu"),
};

Menu* Menu::obj = nullptr;  //initialize the static member variable

int Menu::_openMenu = 0;  //these set the initial values for some of the static variables in the menu class
int Menu::_sel_menuItem = 0;
int Menu::_sel_menuNum = 0;

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
  delay(500);
  Menu::GoDown();  // attempt at moving navigation into menu class as static members. might be easier than separate class.
}

// -----------------------------------------------------------------------------------------------------------
// Loops -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  numOfMenus = sizeof(obj) / sizeof(obj[0]);
  Menu::SetObj(obj);
  startup();
  testing();  // this won't be in setup once testing is complete.
}

void loop() {
  // put your main code here, to run repeatedly:
}