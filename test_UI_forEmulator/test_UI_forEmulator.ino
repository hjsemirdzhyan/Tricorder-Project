/* Emulator can be found at:
    https://wokwi.com/projects/308024602434470466
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

int openMenu = 0;                    // alternative to menuOpen variable that records which menu is open rather than the state of a menu.
int startingPosition = 15;           // y position in pixels of the first item in a menus list
int selectionInterval = 8;           // number of pixels between selection options
int defaultOption = 1;               // starting slection in a menus list
int selectedOption = defaultOption;  // keeps track of which menu item is selected
int startingMenu = 0; // the menu (in terms of array address) to load on startup


// Classes ------------------------------------------------------------------------------------------------------------
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
  String menuName = "";    // may be redundant if i have to use an array to name all the menus.
  int menuNumber = 0;      // menu number
  int belongsTo = 0;       // if it's a submenu, to which menu it belongs
  int position = 0;        // if it's a submenu, where in the list of its parent menu does this menu appear
  bool openState = false;  // true when open
  int numOfItems = 0;      // number of other menus or funcitons inside this one (is this even necessary anymore?)
  bool menuOpen = false;   // might be unnecessary with openMenu variable


public:
  Menu(int menuNumber, int belongsTo, int position, String menuName) {
    this->menuNumber = menuNumber;
    this->belongsTo = belongsTo;
    this->position = position;
    this->menuName = menuName;
  }

  void draw(Menu* objects, int arraySize) {  //the size of the object array (arraySize) is also passed to ensure you're accessing valid elements within the array bounds.
    tft.fillScreen(Display_Color);
    tft.setCursor(0, 0);
    tft.setTextColor(Text_Color);
    tft.setTextSize(2);
    for (int i = 0; i < arraySize; i++) {
      if (objects[i].menuNumber == menuNumber) {
        tft.println(objects[i].menuName);
        openMenu = objects[i].menuNumber;
        belongsTo = objects[i].belongsTo;
        position = objects[i].position;
        menuName = objects[i].menuName;
        for (int j = 1; j < arraySize; j++) {  // starting at 1 in order to ignore parent menu from being displayed as a list item inside itself.
          tft.setTextSize(1);
          if (objects[j].belongsTo == menuNumber) {
            tft.println(objects[j].menuName);
          }
        }
      }
    }
    // this is debug stuff that should accurately tell you which menu is open in plain english
    tft.print("This is menu number ");
    tft.print(openMenu);
    tft.print(" which is named ");
    tft.print(menuName);
    tft.print(". ");
    tft.print("It belongs to menu number ");
    tft.print(belongsTo);
    tft.print(", aka ");
    tft.print(objects[belongsTo].menuName);
    tft.print(".");
  }

  int listSize(Menu* objects, int menuNum, int arraySize) {
    int sum = 0;
    for (int i = 1; i < arraySize; i++) { // i = 1 instead of zero because we want to avoid counting the parent menu as a list item
      if (objects[i].belongsTo == menuNum) {
        sum++;
      }
    }
    Serial.println(sum);
    return(sum);
  }

  void goDown(int arraySize) {
    //clear screen via the draw method
    //call last known selection
    //move selection down by one
    //save new selection 
  }
};


// Initializations -------------------------------------------------------------------------------------------
Menu objects[] = { // note that "someNumber" in objects[someNumber] is an array address, not the menu number. 
  Menu(0, 0, 0, "Main Menu"),
  Menu(1, 0, 1, "Enviro"),
  Menu(2, 0, 2, "Measure"),
  Menu(6, 7, 1, "GPS"),
  Menu(3, 1, 1, "Temp"),
  Menu(4, 2, 1, "Ultrasonic"),
  Menu(5, 1, 2, "CO2"),
  Menu(7, 0, 3, "Location"),
};

int numOfMenus = sizeof(objects) / sizeof(objects[0]);  // Some funky voodoo math going on here. ChatGPT gave this example to dynamically set the number of menus. It's working.


DelayTimer bugginShiz(250);
DelayTimer buttonDelay(500);

// Functions -----------------------------------------------------------------------------------------------

void debugStuff() {
  if (bugginShiz.Update() == true) {
    //Serial.print("selectedOption = ");
    //Serial.println(selectedOption);

    //Serial.print("menuOpen = ");
    //Serial.println(menuOpen);

    Serial.print("cursorX&Y = ");
    Serial.print(tft.getCursorX());
    Serial.print(", ");
    Serial.println(tft.getCursorY());
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

void startUp() {
  tft.begin();
  objects[startingMenu].draw(objects, numOfMenus);  // Edit "startingMenu" to change which menu appears on screen
  tft.drawRect(0, startingPosition, 128, 10, Sel_Color);
  int listSize = objects[startingMenu].listSize(objects, startingMenu, numOfMenus);
  objects[startingMenu].goDown(listSize); // does nothing yet. Not even the right place to call it. 
}

// Loops ----------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  startUp();
}

void loop() {
  if (buttonState() == 3) {
    //Serial.println("Down(SW3)");
  }
  if (buttonState() == 2) {
    //Serial.println("Up(SW2)");
  }
  if (buttonState() == 4) {
    //Serial.println("Right(SW4)");
  }
  if (buttonState() == 1) {
    //Serial.println("Left(SW1)");
  }
}