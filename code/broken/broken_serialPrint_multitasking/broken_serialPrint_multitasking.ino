/*
  The objective of this sketch is to make the serial print calls into a class or function that can be called given some interval input. Rather than it being called each time in every
  function, I want to just call it once. It may need to be passed the displaying data too. It won't always be the same data type though so how do I make it modular enough to work
  for any sensor? :thinking_face:
*/

#include "dht.h"
#define dht_apin A0
dht DHT;
const int pingPin = 7;
const int echoPin = 6;

///////////////////////////////////Classes stored here///////////////////////////////////

class pollSensor {
  long pollTime;
  unsigned long previousPoll;
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
pollSensor led1OnTime(5000);
pollSensor led1OffTIme(5000);
pollSensor led1PollDelay(5000);
pollSensor sensor1PollDelay(5000);
pollSensor sensor2PollDelay(5000);
pollSensor sensor2ClearTime(2);
pollSensor sensor2BurstTime(10);

///////////////////////////////////Setup and loop stored here///////////////////////////////////
void setup() {
  Serial.begin(9600);
}

void loop() {
  //ledResults();
  //tempResults();
  //sonarResults();
}

///////////////////////////////////Functions stored here///////////////////////////////////
void tempResults() {
  if (sensor1PollDelay.Update() == true) {
    DHT.read11(dht_apin);
    Serial.print("Humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("Temperature = ");
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
    Serial.print("Range: ");
    Serial.print(duration);
    Serial.print("ms, ");
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
  }
}

// void ledResults() {
//   int ledPin;
//   long OnTime;
//   long OffTime;
//   int ledState;
//   if (led1PollDelay.Update() == true) {
//     if ((ledState == HIGH) && (led1OnTime.Update() == true)) {
//       ledState = LOW;
//       digitalWrite(ledPin, ledState);
//     } else if ((ledState == LOW) && (led1OffTIme.Update() == true)) {
//       ledState = HIGH;
//       digitalWrite(ledPin, ledState);
//     }
//     Serial.print("LED State = ");
//     Serial.print(ledState);
//     Serial.println(" ");
//   }
// }