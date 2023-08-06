/*
  This sketch is an attempt at running multiple sensors at once. The hope is that I can get all sensors to function at the same time without
  needing to use the delay function. Future plans will include having a specific function that displays output of sensors that obeys their polling times.
  Probably going to be its own sketch. 

  Current progress:
  - LED Blink on pin 13
  - Temp/Humidity sensor on pin A0
*/

#include "dht.h"
#define dht_apin A0
dht DHT;
const int pingPin = 7;  // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6;  // Echo Pin of Ultrasonic Sensor

///////////////////////////////////Classes stored here///////////////////////////////////
class Flasher {  // can probably reuse this for other LEDs that need to flash.
  // Class Member Variables
  // These are initialized at startup
  int ledPin;    // the number of the LED pin
  long OnTime;   // milliseconds of on-time
  long OffTime;  // milliseconds of off-time

  // These maintain the current state
  int ledState;                  // ledState used to set the LED
  unsigned long previousMillis;  // will store last time LED was updated

  // Constructor - creates a Flasher
  // and initializes the member variables and state
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
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();

    if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime)) {
      ledState = LOW;                  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);        // Update the actual LED
      Serial.print("LED State = ");
      Serial.print(ledState);
      Serial.print(" ");
    } else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime)) {
      ledState = HIGH;                 // turn it on
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);        // Update the actual LED
      Serial.print("LED State = ");
      Serial.print(ledState);
      Serial.print(" ");
    }
  }
};

// ATTEMPTING to make this class a generic polling class that can be used for any sensor. Right now it works on the temp sensor. 
class pollSensor {
  long pollTime;               // how often in milliseconds to poll the temp sensor
  unsigned long previousPoll;  // will store last time temp/humid reading was updated

public:
  pollSensor(long time) {
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

///////////////////////////////////Inits stored here///////////////////////////////////
Flasher led1(13, 5000, 5000);
pollSensor sensor1(5000);
pollSensor sensor2PollDelay(100);
pollSensor sensor2ClearTime(2);
pollSensor sensor2BurstTime(10);

///////////////////////////////////Setup and loop stored here///////////////////////////////////
void setup() {
  Serial.begin(9600);
}

void loop() {
  led1.Update();
  tempResults();
  sonarResults();
}

///////////////////////////////////Functions stored here///////////////////////////////////
void tempResults() {
  if (sensor1.Update() == true) {
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
// void flashResults(int pin, int state) {
//   digitalWrite(pin, state);
//   Serial.print("LED State = ");
//   Serial.print(state);
//   Serial.print(" ");
// }
