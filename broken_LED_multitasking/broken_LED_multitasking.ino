/*
  This sketch is non-functional. The ledResults() function doesn't work as expectd. Unless the polling time matches the on/off time of the LED, the sketch won't function as expected. 
  The shorter polling time of the parent if statement seems to override the time tracking of the chield if statements within it. So even if the LED is meant to stay on for 1 second,
  the class being called to track that is overwritten every polling interval. So the On time matches the poll time. Probably going to run into this issue a lot moving forward. It
  would be best to find a solution sooner rather than later. Afterall, I may not want to display data as fast as it's coming in.

  I think this is due to the fact I pulled out the state monitoring code from the flasher class...then deleted the class. I think moving forward
  for any situation requiring knowing the current state of some activity, it will have to be done in a class specific to that activity. Like how
  the flasher class took care of the led flashing. It required knowing its current state and for how long its been in that state.  
*/

#include "dht.h"
#define dht_apin A0
dht DHT;
const int pingPin = 7;  // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6;  // Echo Pin of Ultrasonic Sensor
int ledState = LOW;

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
      Serial.print("Dur: ");
      Serial.println(currentTime - previousPoll);
      previousPoll = currentTime;
      return true;
    } else {
      return false;
    }
  }
};

///////////////////////////////////Inits stored here///////////////////////////////////
pollSensor led1onTime(2000);
pollSensor led1offTime(1000);
pollSensor led1PollDelay(102);
pollSensor sensor1(5000);
pollSensor sensor2PollDelay(100);
pollSensor sensor2ClearTime(2);
pollSensor sensor2BurstTime(10);

///////////////////////////////////Setup and loop stored here///////////////////////////////////
void setup() {
  Serial.begin(9600);
}

void loop() {
  ledResults();
  //tempResults();
  //sonarResults();
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

void ledResults() {
  int ledPin = 13;
  pinMode(ledPin, OUTPUT);
  if (led1PollDelay.Update() == true) {
    if ((ledState == HIGH) && (led1onTime.Update() == true)) {
      ledState = LOW;                  // Turn it off
      digitalWrite(ledPin, ledState);  // Update the actual LED
    } else if ((ledState == LOW) && (led1offTime.Update() == true)) {
      ledState = HIGH;                 // turn it on
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    Serial.print("LED State ");
    Serial.print(ledState);
    Serial.println(" ");
  }
}