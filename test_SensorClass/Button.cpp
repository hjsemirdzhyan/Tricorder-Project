#include "Button.h"

Button::Button() {}
Button::Button(int xPos, int yPos, int butWidth, int butHeight, char* buttonText) {
  x = xPos;
  y = yPos;
  width = butWidth;
  height = butHeight;
  text = buttonText;
}

void Button::render() {
  tft.fillRect(x, y, width, height, Button_Color);  // draw rectangle
  tft.setCursor(x + 5, y + 5);
  tft.setTextSize(_buttonFont);
  tft.setTextColor(ButtonText_Color);
  tft.print(text);
}

bool Button::isClicked(ScreenPoint sp) {
  if ((sp.x >= x) && (sp.x <= (x + width)) && (sp.y >= y) && (sp.y <= (y + height))) {
    return true;
  } else {
    return false;
  }
}