const int pingPin = 7;  // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6;  // Echo Pin of Ultrasonic Sensor

///////////////////////////////////Classes stored here///////////////////////////////////

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
pollSensor sensor2PollDelay(100);
pollSensor sensor2ClearTime(2);
pollSensor sensor2BurstTime(10);

///////////////////////////////////Setup and loop stored here///////////////////////////////////
void setup() {
  Serial.begin(9600);
}
void loop() {
  sonarResults();
}
///////////////////////////////////Functions stored here///////////////////////////////////
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