/* 
  Emulator can be found at: https://wokwi.com/projects/308024602434470466
  C++ tutorial/examples: https://www.w3schools.com/cpp/cpp_oop.asp


  In this refactor of the UI code, I'm getting rid of the hard coded menu numbers and positions. 
  The menu numbers will be calculated on the fly depending on the number of menus and where in the array they're added.
  Similarily, I'll rely on where in the array a menu is added to determine its order on its parent menus page.
  I think this will GREATLY simplify the code.  
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
  int _numOfSubMenus = 0;
  static Menu* obj;
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
    drawSubMenus();
  }

  void drawSubMenus() {
    _numOfSubMenus = calcNumOfSubMenus();
    tft.setTextSize(1);
    for (int i = 1; i < numOfMenus; i++) {
      if (obj[i]._childOf == _menuName) {
        tft.println(obj[i]._menuName);
        if (debug == true) {
          Serial.println("Function, drawSubMenus");
          Serial.print("    Submenu ");
          Serial.print(i);
          Serial.print(" cursor x and y are ");
          Serial.print(tft.getCursorX());
          Serial.println(tft.getCursorY());
        }
      }
    }
  }

  int calcNumOfSubMenus() {
    int numOfSubMenus = 0;
    for (int i = 0; i < numOfMenus; i++) {
      if (obj[i]._childOf == _menuName) {
        numOfSubMenus++;
        if (debug == true) {
          Serial.println("calcNumOfSubMenus Function");
          Serial.print("    Number of submenus ");
          Serial.println(numOfSubMenus);
          Serial.print("    Menu # ");
          Serial.print(i);
          Serial.print(" ");
          Serial.print(obj[i]._menuName);
          Serial.print(" is the child of ");
          Serial.println(obj[i]._childOf);
        }
      }
    }
    return numOfSubMenus;
  }

  String getMenuName() {
    return _menuName;
  }

  String getChildOf() {
    return _childOf;
  }

  static void setObj(Menu* _obj) {
    obj = _obj;
  }
};

class Navigation {
  int sel_y;                                            //  y position of selected submenu
  int sel_menuNum;                                      //  menu number of seleted submenu
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
  obj[0].draw();
  testing();
}

void testing() {
}

// -----------------------------------------------------------------------------------------------------------
// Loops -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void setup() {
  numOfMenus = sizeof(obj) / sizeof(obj[0]);
  Menu::setObj(obj);
  Serial.begin(9600);
  startup();
}

void loop() {
  // put your main code here, to run repeatedly:
}
