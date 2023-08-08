#include "Menu.h"

Menu::Menu(const char menuName[], const char childOf[])
  : _menuName(menuName), _childOf(childOf) {
}

void Menu::Draw() {
  DrawMenu();
  DrawChildren();
  CalcTouchBounds();
}

void Menu::DrawMenu() {
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

void Menu::DrawChildren() {
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

static void Menu::CalcTouchBounds() {
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

static void Menu::DrawTouchBounds() {  // not for actual use. Only for debuging purposes
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

void Menu::GenerateChildren() {
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

void Menu::CalcNumOfChildren() {
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

char* Menu::GetMenuName() {  // returns the menu name of a provided object
  return _menuName;
}

static int Menu::GetMenuNum(char* menuName) {  // takes in a name and returns its number
  for (int i = 0; i < numOfMenus; i++) {
    if (obj[i]._menuName == menuName) {
      return i;
    }
  }
}

char* Menu::GetChildOf() {  //  returns name of parent menu
  return _childOf;
}

static int Menu::GetOpenMenu() {  // returns the index number of the openMenu, not its name
  return _openMenu;
}

static void Menu::SetOpenMenu(char menuName) {  //  sets array index number (menuNumber) of menu given the menus name of menu that's meant to be open.
  for (int i = 0; i < numOfMenus; i++) {        //  need to convert a menu name to it's array index value
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

static void Menu::SetObj(Menu* _obj) {
  obj = _obj;
}

static void Menu::SetSelMenu(int menuItem) {  //  takes in the menuItem and sets menuNumber

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

static void Menu::SetSelMenu1(ScreenPoint sp) {
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

static int Menu::GetSelMenuItem() {
  return _sel_menuItem;
}

static int Menu::GetSelMenuNum() {
  return _sel_menuNum;
}

int Menu::GetNumOfChildren() {
  return _numOfChildren;
}

int* Menu::GetChildrenArray() {
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

static void Menu::OpenSelected() {  // needs to get the selected menu number and then pass that menu number into the draw method.
  _openMenu = _sel_menuNum;
  ;
  SetSelMenu(0);
  obj[_openMenu].Draw();
}

static void Menu::GoBack() {
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

static void Menu::BlueScreenOfDeath() {
  tft.fillScreen(Blank_Color);
  tft.setCursor(0, tft.height() / 2);
  tft.setTextColor(Sel_Color);
  tft.setTextSize(_headerFont);
  tft.println("Oopse, things happened   =/");
}