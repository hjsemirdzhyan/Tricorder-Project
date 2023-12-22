#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "TouchScreen.h"
#include "Button.h"

extern int numOfMenus;

const uint16_t Display_Color = ILI9341_BLUE;
const uint16_t Text_Color = ILI9341_WHITE;
const uint16_t Sel_Color = ILI9341_RED;
const uint16_t Blank_Color = ILI9341_BLACK;
const int calInLay = 20;
const int crosshairSize = 20;
const bool menuDebug = false;

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
  Sensor& _sensor;

public:
  Menu(const char menuName[], const char childOf[]);
  Menu(const char menuName[], const char childOf[], Sensor& sensor);
  void CallSensor();
  void Draw();
  void DrawMenu();
  void DrawChildren();
  static void CalcTouchBounds();
  static void DrawTouchBounds();
  void GenerateChildren();
  void CalcNumOfChildren();
  char* GetMenuName();
  static int GetMenuNum(char* menuName);
  char* GetChildOf();
  static int GetOpenMenu();
  static void SetOpenMenu(char menuName);
  static void SetObj(Menu* _obj);
  static void SetSelMenu(int menuItem);
  static void SetSelMenu1(ScreenPoint sp);
  static int GetSelMenuItem();
  static int GetSelMenuNum();
  int GetNumOfChildren();
  int* GetChildrenArray();
  static void OpenSelected();
  bool HasSensor();
  static void GoBack();
  static void BlueScreenOfDeath();
};