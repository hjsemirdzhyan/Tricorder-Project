//https://wokwi.com/projects/308024602434470466


/*
  Simple "Hello World" for ILI9341 LCD

  https://wokwi.com/arduino/projects/308024602434470466
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

int defaultOption = 1;
int selectedOption = defaultOption;
const uint16_t Display_Color = ILI9341_BLUE;
const uint16_t Text_Color = ILI9341_WHITE;
const uint16_t Sel_Color = ILI9341_RED;


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
  bool menuOpen = false;

public:
  Menu(int num, int belongs, int order, String name) {
    menuNumber = num;
    belongsTo = belongs;
    position = order;
    menuName = name;
  }

  void draw(Menu* objects, int arraySize) {  //the size of the object array (arraySize) is also passed to ensure you're accessing valid elements within the array bounds.
    tft.fillScreen(Display_Color);
    tft.setCursor(0, 0);
    tft.setTextColor(Text_Color);
    tft.setTextSize(2);
    for (int i = 0; i < arraySize; i++) {
      if (objects[i].menuNumber == menuNumber) {
        tft.println(objects[i].menuName);
        for (int j = 1; j < arraySize; j++) {  // starting at 1 in order to ignore parent menu from being displayed as a list item inside itself.
          tft.setTextSize(1);
          if (objects[j].belongsTo == menuNumber) {
            tft.println(objects[j].menuName);
          }
        }
      }
    }
  }

  void select(int menuNumber, int position)
  this->
};

class Navigate {
  int selectedItem = 0;
  int y_pos;
  int newSelection;

public:
  Navigate() {
  }

  void goBack() {
  }

  void goForward() {  //aka 'select'
  }

  void goUp() {
  }

  void goDown() {
    y_pos = (selectedItem * 10) + 22;
    tft.drawRect(0, y_pos, 128, 10, Sel_Color);
    selectedItem++;
  }
};

// Initializations -------------------------------------------------------------------------------------------
Menu objects[] = {
  Menu(0, 0, 0, "Main Menu"),
  Menu(1, 0, 1, "Enviro"),
  Menu(2, 0, 2, "Measure"),
  Menu(3, 1, 1, "Temp"),
  Menu(4, 2, 1, "Ultrasonic"),
  Menu(5, 1, 2, "CO2"),
};

int numOfMenus = sizeof(objects) / sizeof(objects[0]);  // Some funky voodoo math going on here. ChatGPT gave this example to dynamically set the number of menus. It's working.

DelayTimer bugginShiz(250);
DelayTimer buttonDelay(500);

// Functions -----------------------------------------------------------------------------------------------
void debugStuff() {
  if (bugginShiz.Update() == true) {
    Serial.print("selectedOption = ");
    Serial.println(selectedOption);

    //Serial.print("menuOpen = ");
    //Serial.println(menuOpen);

    Serial.print("cursorX&Y = ");
    Serial.print(tft.getCursorX());
    Serial.print(", ");
    Serial.println(tft.getCursorY());
  }
}

int buttonState() { //should this be a class?
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

// Loops ----------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  tft.begin();
  objects[0].draw(objects, numOfMenus);
  //Menu of object[0], draw at position 1 which is at 15 pixels in the +y axis.

}

void loop() {
  /*
  if (buttonState() == 3) {
    selectedOption++;
  }
  if (buttonState() == 2) {
    selectedOption--;
  }
  if (buttonState() == 4) {
    openOption();
    menuOpen = true;
  }
  if (buttonState() == 1) {
    if (menuOpen == true) {
      startupMenu();
      menuOpen = false;
    } else {
      selectedOption = defaultOption;
    }
  }
  if (selectedOption > numOptions) {
    selectedOption = defaultOption;
  }
  if (selectedOption < defaultOption) {
    selectedOption = numOptions;
  }
  debugStuff();
  */
}
