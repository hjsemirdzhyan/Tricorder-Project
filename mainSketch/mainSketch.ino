/*
  This is the mother sketch.

  Current progress:
  - LED Blink on pin 13
  - Temp/Humidity sensor on pin A0
  - Ultrasonic Sensor on pins 6 and 7
  - Multibutton module on pin A1
*/

#include "dht.h"

#define dht_apin A0
#define btn_apin A1
#define RANGE 5
#define SW1 RANGE
#define SW2 142
#define SW3 327
#define SW4 504
#define SW5 741
dht DHT;

const int pingPin = 7;  // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6;  // Echo Pin of Ultrasonic Sensor

///////////////////////////////////Classes stored here///////////////////////////////////
class Flasher {
  int ledPin;
  long OnTime;
  long OffTime;

  int ledState;
  unsigned long previousMillis;

public:
  Flasher(int pin, long on, long off) {
    ledPin = pin;
    pinMode(ledPin, OUTPUT);

    OnTime = on;
    OffTime = off;

    ledState = LOW;
    previousMillis = 0;
  }

  void Update() {
    unsigned long currentMillis = millis();

    if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime)) {
      ledState = LOW;
      previousMillis = currentMillis;
      digitalWrite(ledPin, ledState);
      Serial.print("LED State = ");
      Serial.print(ledState);
      Serial.println(" ");
    } else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime)) {
      ledState = HIGH;
      previousMillis = currentMillis;
      digitalWrite(ledPin, ledState);
      Serial.print("LED State = ");
      Serial.print(ledState);
      Serial.println(" ");
    }
  }
};

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

///////////////////////////////////Inits stored here///////////////////////////////////
Flasher led1(13, 1000, 1000);
DelayTimer tempSensor(3000);
DelayTimer sensor2PollDelay(100);
DelayTimer sensor2ClearTime(2);
DelayTimer sensor2BurstTime(10);

///////////////////////////////////Setup and loop stored here///////////////////////////////////
void setup() {
  Serial.begin(9600);
}

void loop() {
  if (buttonState() == 1) {
    led1.Update();
  }
  if (buttonState() == 2) {
    tempResults();
  }
  if (buttonState() == 3) {
    sonarResults();
  }
}

///////////////////////////////////Functions stored here///////////////////////////////////
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

void sonarResults() {
  if (sensor2PollDelay.Update() == true) {
    long duration;
    long inches;
    long cm;
    pinMode(pingPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(pingPin, LOW);
    if (sensor2ClearTime.Update() == true) {
      digitalWrite(pingPin, HIGH);
    }
    if (sensor2BurstTime.Update() == true) {
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