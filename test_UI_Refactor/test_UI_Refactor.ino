/* 
  Emulator can be found at: https://wokwi.com/projects/308024602434470466
  C++ tutorial/examples: https://www.w3schools.com/cpp/cpp_oop.asp


  In this refactor of the UI code, I'm getting rid of the hard coded menu numbers and positions. 
  The menu numbers will be calculated on the fly depending on the number of menus and where in the array they're added.
  Similarily, I'll rely on where in the array a menu is added to determine its order on its parent menus page.
  I think this will GREATLY simplify the code.

  Would I rather calculate the list of submenus for each menu on the fly or do it once and store it in memory? 
*/

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define btn_apin A0
#define RANGE 20
#define SW1 RANGE
#define SW2 155
#define SW3 340
#define SW4 515
#define SW5 760

const uint16_t Display_Color = ILI9341_BLUE;
const uint16_t Text_Color = ILI9341_WHITE;
const uint16_t Sel_Color = ILI9341_RED;

bool debug = false;
int numOfMenus = 0;
int openMenu = 0;

int y_Cursor1 = 8;
int y_Cursor2 = 15;

// -----------------------------------------------------------------------------------------------------------
// Classes ---------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
class Menu {
  String _menuName = "";
  int _menuNum = 0;
  String _childOf = "";
  int _numOfChildren = 0;                          //  wont need anymore if array is storing children of parent menu
  static Menu* obj;
  int* _childrenArray;                             // Array of children menu numbers

public:
  Menu(String _menuName, String _childOf) {
    this->_menuName = _menuName;
    this->_childOf = _childOf;
  }

  void draw() {
    tft.fillScreen(Display_Color);
    tft.setCursor(0, 0);
    tft.setTextColor(Text_Color);
    tft.setTextSize(2);
    tft.println(_menuName);
    if (debug == true) {
      Serial.println("Function, Draw");
      Serial.print("    Heading cursor x and y are ");
      Serial.print(tft.getCursorX());
      Serial.println(tft.getCursorY());
    }
    drawChildren();
  }

  void generateChildren() {
    calcNumOfChildren();                            //  generate value for number of children
    int a = _numOfChildren;
    _childrenArray = new int[a];                    //  initialize empty array of child list length
    int index = 0;                                  //  stores where the next child menu number will be stored in the index
    for (int i = 0; i < numOfMenus; i++) {          //  check all menus
      if (obj[i]._childOf == _menuName) {           //  if a menus childOf equals current menus name
        _childrenArray[index] = i;                  //  no menu other than zero has been accessed or generated yet so menuNum is zero for all
        index++;
        }
      }
    if (true == true) {
      Serial.println("Function, generateChildren");
      Serial.print("    Children of ");
      Serial.print(_menuName);
      Serial.print("{");
      for (int i = 0; i < a; i++) {
        Serial.print(_childrenArray[i]);
        if (i < a - 1) {
        Serial.print(", ");
        } else if (i == a - 1) {
          Serial.println("}");
        }
      }
    }
  }

  void drawChildren() {
    if (_childrenArray == nullptr) {
      generateChildren();                         // Generate children array only if it hasn't been populated yet
    }
    int a = _numOfChildren;
    String b = "";
    tft.setTextSize(1);
    if (true == true) {
      Serial.println("Function, drawChildren");
    }
      for (int i = 0; i < a; i++) {
        b = obj[_childrenArray[i]]._menuName;
        tft.println(b);
        if (true == true) {
          Serial.print("    Child menu ");
          Serial.print(_childrenArray[i]);
          Serial.print(" at position ");
          Serial.print(i);
          Serial.print(" cursor x,y: ");
          Serial.print(tft.getCursorX());
          Serial.println(tft.getCursorY());
        }
    }
  }

  void calcNumOfChildren() {
    int numOfChildren = 0;
    for (int i = 0; i < numOfMenus; i++) {
      if (obj[i]._childOf == _menuName) {
        numOfChildren++;
        if (debug == true) {
          Serial.println("Function, calcNumOfChildren");
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
  }

  String getMenuName() {
    return _menuName;
  }
  int getMenuNum() {                              //  cant really use cuz menu num only generates once menu is accessed
    return _menuNum;
  }

  String getChildOf() {
    return _childOf;
  }

  static void setObj(Menu* _obj) {
    obj = _obj;
  }
};

class Navigation {
  int sel_yPos;                                         //  y position of selected submenu
  int sel_menuNum;                                      //  menu number of seleted submenu
public:
  Navigation() {                                        //  I need a Navigation class in order to store the navigation state of all the menus

  }

  void goDown() {
    //  first, needs to check if selection is outside the length of the menu items
    //      if it is, start from first item on list then continue
    //      else contine
    //  second, find list of menu numbers belonging to current menu
    //      iterate the sel_yPos and sel_menuNum
    //      or if there are no items on the list, do nothing
  }

  void select() {

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

// -----------------------------------------------------------------------------------------------------------
// Functions -------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void startup() {
  tft.begin();
  //obj[0].draw();
}

void testing() {
  obj[1].generateChildren();
}

// -----------------------------------------------------------------------------------------------------------
// Loops -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void setup() {
  numOfMenus = sizeof(obj) / sizeof(obj[0]);
  Menu::setObj(obj);
  Serial.begin(9600);
  startup();
  testing();
}

void loop() {
  // put your main code here, to run repeatedly:
}
