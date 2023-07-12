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
  int _numOfChildren = 0;     //  wont need anymore if array is storing children of parent menu...maybe
  int* _childrenArray;        //  array of addresses to children menu numbers
  static int* childrenArray;  //  same as _childrenArray except it's only used in static members so as to avoid having to pass along _childrenArray as a parameter for every damn function related to the menu number assignment process
  static Menu* obj;
  static int _openMenu;      //  variable that stores currently open menu number
  int _sel_yPos = 15;        //  y position of selected submenu
  static int _sel_menuItem;  //  number in child list of where selector is
  static int _sel_menuNum;   //  menu number of seleted submenu

public:
  Menu(String menuName, String childOf)
    : _menuName(menuName), _childOf(childOf) {
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
    int numOfChildren = 0;  //sus
    if (debug == true) {
      Serial.println("Method, CalcNumOfChildren");
      Serial.print("    numOfMenus: ");
      Serial.println(numOfMenus);
    }
    for (int i = 0; i < numOfMenus; i++) {
      if (obj[i]._childOf == _menuName) {
        numOfChildren++;
        if (debug == true) {
          Serial.print("    Count of children ");
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
    int a = menuItem;
    _sel_menuItem = menuItem;
    if (debug == true) {
      Serial.println("Method, SetSelMenuItem");
      Serial.print("    Selected menu item #: ");
      Serial.println(a);
    }
    SetSelMenuNum(a);  //  move to nav class under open method, eventaully. So this isn't called until actually needed.
  }

  static void SetSelMenuNum(int menuItem) {  //  takes in the menuItem and sets menuNumber
    int a = childrenArray[menuItem];
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

  static int GetSelMenuNum() {  // Needs converting to get the num
    return _sel_menuNum;
  }

  int GetNumOfChildren() {
    return _numOfChildren;
  }

  static void SetChildrenArray(int* _childrenArray) {
    int a = obj[GetOpenMenu()].GetNumOfChildren();  //   will need to change the zero to a variable.
    childrenArray = new int[a];
    if (a > 0) {
      for (int i = 0; i < a; i++) {
        childrenArray[i] = _childrenArray[i];
      }
    }

    if (debug == true) {
      Serial.println("Method, SetChildrenArray");
      Serial.print("    var a returns: ");
      Serial.println(a);
      Serial.print("    childrenArray returns: ");
      for (int i = 0; i < a; i++) {
        Serial.print(childrenArray[i]);
        Serial.print(" ");
      }
      Serial.println();
    }
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
};

class Nav {
  Menu& _currentMenu;  // Member variable to store the current menu object aka should always be storing openMenu
  Menu* _allMenus;

public:
  Nav(Menu& currentMenu, Menu* allMenus)
    : _currentMenu(currentMenu), _allMenus(allMenus) {}  // need a setter for current menu!!!!!!

  void GoDown() {
    int a = Menu::GetSelMenuItem();
    int b = a + 1;
    int c = _currentMenu.GetNumOfChildren();  //  get the number of children menus of the open menu
    if (b > c) {                              //  if we try to navigate passed the last child menu, reset to the top
      b = 0;
    }
    Menu::SetSelMenuItem(b);  // Somethings dun fucked up here. calls the setter of the selected menu item variable
    _currentMenu.Draw();

    if (debug == true) {
      Serial.println("Method, GoDown");
      Serial.print("    _currentMenu is ");
      Serial.println(_currentMenu.GetMenuName());
      Serial.print("    Retrieved selected menu item # is (starts from zero): ");
      Serial.println(a);
      Serial.print("    Menu item # should now be: ");
      Serial.println(b);
      Serial.print("    Retrieved number of children is (doesnt start from zero): ");
      Serial.println(c);
    }
  }

  void OpenSelected() {
    int a = Menu::GetSelMenuNum();
    //_allMenus[a].Draw();

    if (debug == true) {
      Serial.println("Method, OpenSelected");
      Serial.print("    Retrieved selected menu # is: ");
      Serial.println(a);
    }
  }

  void GetCurrentMenu() {
    if (debug == true) {
      Serial.println("Method, GetCurrentMenu");
      Serial.println("    Nothing should be happening here");
    }
  }
};
// -----------------------------------------------------------------------------------------------------------
// Initializations -------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
Menu* Menu::obj = nullptr;           //  initialize the static member variable
int* Menu::childrenArray = nullptr;  //  initializes the childrenArray array

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
  Menu("Body Profile", "Main Menu"),
  Menu("Accelerometer", "Enviro"),
};

int Menu::_openMenu = 0;  // these set the initial values for some of the static variables in the menu class
int Menu::_sel_menuItem = 0;
int Menu::_sel_menuNum = 0;

Nav menuInterface(obj[Menu::GetOpenMenu()], obj);  // initializes the menuInterface object of the nav class with the current open menu (default is main menu)

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
  obj[Menu::GetOpenMenu()].Draw();  // displays the starting menu (by running a lot of other methods first)
  delay(500);
  Menu::SetChildrenArray(obj[Menu::GetOpenMenu()].GetChildrenArray());  //   calling the SetChildrenArray so that the _childrenArray can be accessable to static members (via childrenArray).
}

void testing() {
  delay(1000);
  menuInterface.GoDown(); // problem code
  // delay(1000);
  // menuInterface.GoDown();
  // delay(1000);
  // menuInterface.OpenSelected();
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