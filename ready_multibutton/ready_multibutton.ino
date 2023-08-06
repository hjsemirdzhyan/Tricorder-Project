/*
  Analog Read. Seeing what I can do with the 5 button controller I got from RAB.
  SW1 0-10
  SW2 140-150
  SW3 325-335
  SW4 500-510
  SW5 738-748
  NA 1018-1024
*/
#include "dht.h"

#define dht_apin A1
#define btn_apin A0
#define RANGE 5
#define SW1 RANGE
#define SW2 142
#define SW3 327
#define SW4 504
#define SW5 741
dht DHT;

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

DelayTimer tempSensor(1000);

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (buttonState() == 1) {
    tempResults();
  }
  if (buttonState() == 2) {
    Serial.println("2");
  }
}

int buttonState() {
  int state = 0;
  state = analogRead(btn_apin);
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

void tempResults() {
  if (tempSensor.Update() == true) {
    DHT.read11(dht_apin);
    Serial.print("humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature);
    Serial.println("C  ");
  }
}
