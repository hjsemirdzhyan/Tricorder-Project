#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

const int pingPin = 7;
const int echoPin = 6;



class delayTracker {
  long pollTime;
  unsigned long previousPoll;

public:
  delayTracker(long time) {
    pollTime = time;
    previousPoll = 0;
  }

  bool Update() {
    unsigned long currentTime = millis();

    if (currentTime - previousPoll >= pollTime) {
      previousPoll = currentTime;
      return true;
    } else {
      return false;
    }
  }
};

delayTracker ussPollDelay(100);
delayTracker ussClearTime(2);
delayTracker ussBurstTime(10);
delayTracker screenRefresh(500);

void DrawTitle() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.println("Ultrasonic Test");
}

void ultrasonicSensor() {
  if (ussPollDelay.Update() == true) {
    long duration;
    long inches;
    long cm;
    pinMode(pingPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(pingPin, LOW);
    if (ussClearTime.Update() == true) {
      digitalWrite(pingPin, HIGH);
    }
    if (ussBurstTime.Update() == true) {
      digitalWrite(pingPin, LOW);
    }
    duration = pulseIn(echoPin, HIGH);
    inches = duration / 74 / 2;
    cm = duration / 29 / 2;
    Serial.print(duration);
    Serial.print("ms, ");
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
  }
}

void RedrawScreen() {

}

void setup() {
  delay(1000);
  Serial.begin(9600);
  tft.begin();
  delay(1000);
  DrawTitle();

}

void loop() {
  ultrasonicSensor();

}
