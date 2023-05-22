#include <Adafruit_ST7735.h>
#include <Arduino.h>

#define TFT_CS 10
#define TFT_DC 8
#define TFT_RST -1

#define btn_apin A0
#define RANGE 20
#define SW1 RANGE
#define SW2 155
#define SW3 340
#define SW4 515
#define SW5 760

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int defaultOption = 1;
int selectedOption = defaultOption;
bool menuOpen = false;
const uint16_t Display_Color = ST7735_BLUE;
const uint16_t Text_Color = ST7735_WHITE;


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
};

// Initializations --------------------------------------------------------------
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

// Functions -----------------------------------------------------------------------------------------------
int buttonState() {
  int state = analogRead(btn_apin);
  if (buttonDelay.Update() == true) {
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

void startupMenu() {
  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(Display_Color);
  tft.setCursor(0, 0);
  tft.setTextColor(Text_Color);
  tft.setTextSize(2);
  tft.println("Menu:");
  tft.setTextSize(1);
  tft.println("- Option 1");
  tft.println("- Option 2");
  tft.println("- Option 3");
  tft.println("- Option 4");
  //tft.drawRect(0, 15, 128, 10, ST7735_RED);
}

void option1Menu() {
  tft.fillScreen(Display_Color);
  tft.setCursor(0, 0);
  tft.setTextColor(Text_Color);
  tft.setTextSize(2);
  tft.println("Option 1:");
  tft.setTextSize(1);
  tft.println("This is some stuff from option 1 menu");
}

void option2Menu() {
  tft.fillScreen(Display_Color);
  tft.setCursor(0, 0);
  tft.setTextColor(Text_Color);
  tft.setTextSize(2);
  tft.println("Option 2:");
  tft.setTextSize(1);
  tft.println("How now brown cow?");
}

void openOption() {
  if (selectedOption == 1) {
    option1Menu();
  }
  if (selectedOption == 2) {
    option2Menu();
  }
}

void debugStuff() {
  if (bugginShiz.Update() == true) {
    Serial.print("selectedOption = ");
    Serial.println(selectedOption);

    Serial.print("menuOpen = ");
    Serial.println(menuOpen);

    Serial.print("cursorX&Y = ");
    Serial.print(tft.getCursorX());
    Serial.print(", ");
    Serial.println(tft.getCursorY());
  }
}

// Loops --------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  tft.initR(INITR_144GREENTAB);
  objects[0].draw(objects, numOfMenus);
  delay(2000);
  objects[1].draw(objects, numOfMenus);
  delay(2000);
  objects[2].draw(objects, numOfMenus);
}

void loop() {

}




/*  tft.println("- Option 1");
    tft.println("- Option 2");
    tft.println("- Option 3");
    tft.println("- Option 4");
    tft.drawRect(0, 15, 128, 10, ST7735_RED);
    */