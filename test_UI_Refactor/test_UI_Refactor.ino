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
  String _menuName = "";
  String _childOf = "";
  int _numOfChildren = 0;  //  wont need anymore if array is storing children of parent menu...maybe
  int* _childrenArray;     //  array of children menu numbers
  static Menu* obj;
  static int _openMenu;      //  variable that stores currently open menu number
  int _sel_yPos = 15;        //  y position of selected submenu
  static int _sel_menuItem;  //  number in child list of where selector is
  static int _sel_menuNum;   //  menu number of seleted submenu

public:
  Menu(String menuName, String childOf) {
    this->_menuName = menuName;
    this->_childOf = childOf;
  }

  void Draw() {
    SetOpenMenu(_menuName);  //  _openMenu should be set any time a menu is drawn. In case other parts of code foregts to set it from a user interaction.
    tft.fillScreen(Display_Color);
    tft.setCursor(0, 0);
    tft.setTextColor(Text_Color);
    tft.setTextSize(2);
    tft.println(_menuName);
    if (debug == true) {
      Serial.print("Method, Draw: ");
      Serial.println(_menuName);
      Serial.print("    Heading cursor x,y are ");
      Serial.print(tft.getCursorX());
      Serial.println(tft.getCursorY());
    }
    DrawChildren();
  }

  void DrawChildren() {
    if (_childrenArray == nullptr) {
      GenerateChildren();  //  Generate children array only if it hasn't been populated yet (in order to save processing time)
    } else {
      Serial.println("Skipped, GenerateChildren");
      Serial.println("    _childrenArray is already populated");
    }
    int a = _numOfChildren;
    String b = "";
    tft.setTextSize(1);
    if (debug == true) {
      Serial.println("Method, DrawChildren");
    }
    if (a < 1) {
      Serial.println("    No Children");
    } else {
      for (int i = 0; i < a; i++) {
        b = obj[_childrenArray[i]]._menuName;  //  menu name of child menus
        if (i == _sel_menuItem) {
          tft.setTextColor(Sel_Color);
          tft.println(b);
        } else {
          tft.setTextColor(Text_Color);
          tft.println(b);
        }
        if (debug == true) {
          Serial.print("    Child menu ");
          Serial.print(_childrenArray[i]);
          Serial.print(" ");
          Serial.print(b);
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
    CalcNumOfChildren();  //  generate value for number of children
    int a = _numOfChildren;
    _childrenArray = new int[a];              //  initialize empty array of child list length
    if (a > 0) {                              //  only scan thru matching menus if menu has at least one child
      int index = 0;                          //  stores where the next child menu number will be stored in the index
      for (int i = 0; i < numOfMenus; i++) {  //  check all menus
        if (obj[i]._childOf == _menuName) {   //  if a menus childOf equals current menus name
          _childrenArray[index] = i;          //  no menu other than zero has been accessed or generated yet so menuNum is zero for all
          index++;
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
    }
    for (int i = 0; i < numOfMenus; i++) {
      if (obj[i]._childOf == _menuName) {
        numOfChildren++;
        if (debug == true) {
          Serial.print("    Number of children ");
          Serial.println(numOfChildren);
          Serial.print("    Menu # ");
          Serial.print(i);
          Serial.print(" ");
          Serial.print(obj[i]._menuName);
          Serial.print(" is the child of ");
          Serial.println(obj[i]._childOf);
        }
      }
    }
    _numOfChildren = numOfChildren;
    if (debug == true) {
      Serial.print("    _numOfChildren = ");
      Serial.println(_numOfChildren);
    }
  }

  String GetMenuName() {
    return _menuName;
  }

  String GetChildOf() {
    return _childOf;
  }

  static int GetOpenMenu() {
    if (debug == true) {
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

  static void SetSelMenuItem(int menuItem) {
    _sel_menuItem = menuItem;
  }

  static void SetSelMenuNum(int menuNum) {
    _sel_menuNum = menuNum;
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
};

class Nav {
public:
  void GoDown(Menu& menuObj) {  // the specific instance(object) of menu is being passed in as a parameter. Ex obj[openMenu] is the intended parameter
    int a = Menu::GetSelMenuItem();
    int b = a + 1;
    int c = menuObj.GetNumOfChildren();  //  get the number of children menus of the open menu
    if (b > a) {                         //  if we try to navigate passed the last child menu, reset to the top
      b = 0;
    }
    Menu::SetSelMenuItem(b);  //  calls the setter of the selected menu item variable
  }
};
// -----------------------------------------------------------------------------------------------------------
// Initializations -------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
Menu* Menu::obj = nullptr;  // Initialize the static member variable

Menu obj[] = {
  Menu("Main Menu", "None"),
  Menu("Enviro", "Main Menu"),
  Menu("Temp", "Enviro"),
  Menu("Humidity", "Enviro"),
  Menu("Location", "Main Menu"),
  Menu("GPS", "Location"),
  Menu("Baro Pressure", "Enviro"),
  Menu("SubGhz", "Main Menu"),
  Menu("NFC", "SubGhz"),
  Menu("RFID", "SubGhz"),
  Menu("Blutooth", "SubGhz"),
};

Nav menuInterface;

int Menu::_openMenu = 0;
int Menu::_sel_menuItem = 0;
int Menu::_sel_menuNum = 0;

// -----------------------------------------------------------------------------------------------------------
// Functions -------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void startup() {
  tft.begin();
  obj[0].Draw();
}

void testing() {
  delay(2000);
  obj[1].Draw();
  delay(2000);
  menuInterface.GoDown(obj[Menu::GetOpenMenu()]);
  delay(2000);
  menuInterface.GoDown(obj[Menu::GetOpenMenu()]);
  obj[1].Draw();
}

// -----------------------------------------------------------------------------------------------------------
// Loops -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void setup() {
  numOfMenus = sizeof(obj) / sizeof(obj[0]);
  Menu::SetObj(obj);
  Serial.begin(9600);
  startup();
  testing();
}

void loop() {
  // put your main code here, to run repeatedly:
}