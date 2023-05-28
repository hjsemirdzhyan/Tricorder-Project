/* 
  Emulator can be found at: https://wokwi.com/projects/308024602434470466
  C++ tutorial/examples: https://www.w3schools.com/cpp/cpp_oop.asp
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

int selectionStartingPosition = 15;  // y position in pixels of the first item in a menus list
int selectionInterval = 8;           // number of pixels between selection options
int startingMenu = 0;                // the menu (in terms of menu number) to load on startup
int openMenu = startingMenu;         // stores the menu number of the currently open menu
int numOfMenus = 0;

// -----------------------------------------------------------------------------------------------------------
// Classes ---------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
class DelayTimer {
  long delayTime;
  unsigned long previousPoll;

public:
  DelayTimer(long time) {
    delayTime = time;
    previousPoll = 0;
  }

  bool Update() {
    unsigned long currentTime = millis();

    if (currentTime - previousPoll >= delayTime) {
      previousPoll = currentTime;
      return true;
    } else {
      return false;
    }
  }
};

class Menu {
  String menuName = "";                                      // may be redundant if i have to use an array to name all the menus.
  int menuNumber = 0;                                        // menu number
  int belongsTo = 0;                                         // if it's a submenu, to which menu it belongs
  int position = 0;                                          // if it's a submenu, where in the list of its parent menu does this menu appear
  int numOfItems = 0;                                        // number of other menus or funcitons inside this one (is this even necessary anymore?)
  int currentSelectionPosition = selectionStartingPosition;  // keeps track of which menu item is selected
  int endOfList = 0;                                         // y_position after the last item on a menu
  int selectedItem = 1;                                      // numerical item currently selected
  int numberOfListItems = 0;                                 // number of items on menu
public:
  Menu(int menuNumber, int belongsTo, int position, String menuName) {
    this->menuNumber = menuNumber;
    this->belongsTo = belongsTo;
    this->position = position;
    this->menuName = menuName;
  }

  void draw(Menu* objects) {  //the size of the object array (arraySize) is also passed to ensure you're accessing valid elements within the array bounds.
    tft.fillScreen(Display_Color);
    tft.setCursor(0, 0);
    tft.setTextColor(Text_Color);
    tft.setTextSize(2);
    for (int i = 0; i < numOfMenus; i++) {
      if (objects[i].menuNumber == openMenu) {
        tft.println(objects[i].menuName);
        int count = 1;
        for (int j = 1; j < numOfMenus; j++) {  // starting at 1 in order to ignore parent menu from being displayed as a list item inside itself. Need to replace numOfMenus with length of item list.
          tft.setTextSize(1);
          if (objects[j].belongsTo == menuNumber) {
            tft.println(objects[j].menuName);
            count++;
          }
        }
        numberOfListItems = count;
      }
    }
    endOfList = tft.getCursorY();
  }

  int getNumberOfListItems(Menu* objects, int menuNum) {
    return objects[getAddress(objects, menuNum)].numberOfListItems;
  }

  int listSize(Menu* objects, int menuNum) {  // redundant. Will delete and use the get instead.
    int sum = 0;
    for (int i = 1; i < numOfMenus; i++) {  // i = 1 instead of zero because we want to avoid counting the parent menu as a list item
      if (objects[i].belongsTo == menuNum) {
        sum++;
      }
    }
    return (sum);
  }

  String getMenuName(Menu* objects, int menuNum) {
    for (int i = 0; i < 10; i++) {
      if (objects[i].menuNumber == menuNum) {
        return objects[i].menuName;
      }
    }
  }

  int getMenuNumber(Menu* objects, int address) {
    return objects[address].menuNumber;
  }

  int getMenuBelongsTo(Menu* objects, int menuNum) {
    for (int i = 0; i < 10; i++) {
      if (objects[i].menuNumber == menuNum) {
        return objects[i].belongsTo;
      }
    }
  }

  int getSelectedMenu(Menu* objects) {                              //  returns the menu number of the currently selected menu item
    for (int i = 1; i < numOfMenus; i++) {                          //  iterate over all menus
      if (objects[i].belongsTo == getAddress(objects, openMenu)) {  //  first filter for belongsTo of current menu
        if (objects[i].position == selectedItem) {                  //  Find where the position of the belongs to matches the position of the selected item.
          return objects[i].menuNumber;                             //  return that that items menu number
        }
      }
    }
  }

  int getSelectedItem(Menu* objects) {  //  returns the position in terms of the list lenght of the open menu
    int a = getSelectedMenu(objects);   //  finds menu number of selectedMenu
    int b = getAddress(objects, a);     //  find address of selectedMenu
    return objects[b].position;         //  returns position number of selectedMenu
  }

  int getAddress(Menu* objects, int menuNum) {  // this is only ever going to be used inside this method. Since you need to know the address already in order to call this method outside the class
    for (int i = 0; i < numOfMenus; i++) {
      if (objects[i].menuNumber == menuNum) {
        return i;
      }
    }
  }

  void cursorToEndOfList() {
    tft.setCursor(0, endOfList);
  }

  void goDown(Menu* objects) {
    draw(objects);
    currentSelectionPosition = currentSelectionPosition + selectionInterval;
    tft.drawRect(0, currentSelectionPosition, 128, 10, Sel_Color);
    selectedItem++;
    Serial.print("selected item is ");
    Serial.println(selectedItem);
  }

  void select(Menu* objects) { // sets the new open menu based on the selectedMenu
    int a = getSelectedMenu(objects); //  gets the menu number of the selectedMenu
    int b = getAddress(objects, a);
    openMenu = objects[b].menuNumber;
    draw(objects);
  }
};

// -----------------------------------------------------------------------------------------------------------
// Initializations -------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
Menu objects[] = {
  Menu(0, 0, 0, "Main Menu"),
  Menu(1, 0, 1, "Enviro"),
  Menu(2, 0, 2, "Measure"),
  Menu(6, 7, 1, "GPS"),
  Menu(3, 1, 1, "Temp"),
  Menu(4, 2, 1, "Ultrasonic"),
  Menu(5, 1, 2, "CO2"),
  Menu(7, 0, 3, "Location"),
  Menu(8, 0, 4, "Blutooth"),
  Menu(9, 0, 5, "RFID"),
  Menu(10, 0, 6, "NFC"),
};

DelayTimer bugginShiz(250);
DelayTimer buttonDelay(500);

// -----------------------------------------------------------------------------------------------------------
// Functions -------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void debugStuff() {
  Serial.println("We're in the debugger");
  if (bugginShiz.Update() == true) {

    int x = openMenu;                                        // shows currently active menu
    int y = menuToAddress(objects, openMenu);                // converts menu number to its address
    String a = objects[y].getMenuName(objects, openMenu);    // gets menu name
    int b = objects[y].getMenuBelongsTo(objects, openMenu);  // gets menu number of parent menu
    String c = objects[b].getMenuName(objects, b);           // gets name of parent menu
    int d = objects[y].getNumberOfListItems(objects, y);     // gets count of listed items
    int e = objects[y].getSelectedItem(objects);             // gets position of selected item

    objects[y].cursorToEndOfList();

    tft.print("This is menu number ");
    tft.print(x);
    tft.print(" which is named ");
    tft.print(a);
    tft.print(". It belongs to menu number ");
    tft.print(b);
    tft.print(", also known as ");
    tft.print(c);
    tft.print(". There are ");
    tft.print(d);
    tft.print(" item(s) on this menu. Item number ");
    tft.print(e);
    tft.print(" is currently selected. The address of this menu is ");
    tft.print(y);
    tft.print(".");
  }
}

int buttonState() {  //should this be a class?
  int state = analogRead(btn_apin);
  if (buttonDelay.Update() == true) {  // supposed to be a debounce...but not very effective.
    if (SW1 - RANGE <= state && state < RANGE + SW1) {
      return 1;
    }
    if (SW2 - RANGE < state && state < RANGE + SW2) {
      return 2;
    }
    if (SW3 - RANGE < state && state < RANGE + SW3) {
      return 3;
    }
    if (SW4 - RANGE < state && state < RANGE + SW4) {
      return 4;
    }
    if (SW5 - RANGE < state && state < RANGE + SW5) {
      return 5;
    } else {
      return 0;
    }
  }
}

int menuToAddress(Menu* objects, int menuNum) {  // this function is needed in order to get the unknown address of a known menu number. The class memeber getAddress requires you already to know the address.
  for (int i = 0; i < numOfMenus; i++) {
    if (objects[i].getMenuNumber(objects, i) == menuNum) {
      return i;
    }
  }
}

void testingActions() {  // temporary until I get my buttons to work.
  int a = menuToAddress(objects, openMenu); //  address of open menu
  int length = objects[a].getNumberOfListItems(objects, openMenu);
  for (int i = 0; i < length - 1; i++) {  // simulates moving the cursor down the length of the current menus list
    objects[a].goDown(objects);
    delay(250);
  }
  Serial.println("We're done moving");
  delay(1000);
  objects[a].select(objects); //
}

void startUp() {
  tft.begin();
  objects[startingMenu].draw(objects);                             // Edit "startingMenu" to change which menu appears on screen
  tft.drawRect(0, selectionStartingPosition, 128, 10, Sel_Color);  // Draws selection rectangle on startingPosition of the screen
}

// -----------------------------------------------------------------------------------------------------------
// Loops -----------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
void setup() {
  numOfMenus = sizeof(objects) / sizeof(objects[0]);  // Some funky voodoo math going on here. ChatGPT gave this example to dynamically set the number of menus. It's working.
  Serial.begin(9600);
  startUp();
  testingActions();  // For now, will be simulating button presses via hard coded functions
  debugStuff();
}

void loop() {
}