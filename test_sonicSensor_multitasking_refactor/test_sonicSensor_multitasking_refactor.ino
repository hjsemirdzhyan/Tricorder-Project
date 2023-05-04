/*
  Example code and description of how this sensor works:
    https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/
 
  Note: 
    In the example code they use the delay function but that's not going to work for my needs. Hence the rewrite with the if statements.
    I can't have my code come to a screeching hault every time the ultrasonic sensor is polled. 

  From URL:
    "In order to generate the ultrasound we need to set the Trig pin on a High State for 10 µs. 
    That will send out an 8 cycle ultrasonic burst which will travel at the speed of sound. 
    The Echo pins goes high right away after that 8 cycle ultrasonic burst is sent, 
    and it starts listening or waiting for that wave to be reflected from an object.

    If there is no object or reflected pulse, the Echo pin will time-out after 38ms 
    and get back to low state.

    If we receive a reflected pulse, the Echo pin will go down sooner than those 38ms. 
    According to the amount of time the Echo pin was HIGH, we can determine the distance 
    the sound wave traveled, thus the distance from the sensor to the object."
*/

const int pingPin = 7;  // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6;  // Echo Pin of Ultrasonic Sensor

///////////////////////////////////Classes stored here///////////////////////////////////
class SonicSensor {
private:
  long duration;
  long inches;
  long cm;
  long delayTime;
  long clearTime;
  long burstTime;
  unsigned long previousClear; // Do I really need one variable for each time interval being tracked? Would be better to use just one right? No need for 3? 
  unsigned long previousBurst;
  unsigned long previousDelay;

public:
  SonicSensor(int delay, int clear, int burst) {
    delayTime = delay;
    clearTime = clear;
    burstTime = burst;
    pinMode(pingPin, OUTPUT);
    pinMode(echoPin, INPUT);
    previousClear = 0;
    previousBurst = 0;
    previousDelay = 0;
  }

  void Update() { // For some reason the serial print is displaying one set of outputs thats all zero and another set of outputs with correct distance.
    unsigned long currentMillis = millis();
    digitalWrite(pingPin, LOW);
    if (currentMillis - previousClear >= clearTime) {
      digitalWrite(pingPin, HIGH);
      previousClear = currentMillis;
    }
    if (currentMillis - previousBurst >= burstTime) {
      digitalWrite(pingPin, LOW);
      previousBurst = currentMillis;
    }
    if (currentMillis - previousDelay >= delayTime) {
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
};

///////////////////////////////////Inits stored here///////////////////////////////////
SonicSensor sensor1(100, 2, 10);

///////////////////////////////////Setup and loop stored here///////////////////////////////////
void setup() {
  Serial.begin(9600);
}
void loop() {
  sensor1.Update();
}
///////////////////////////////////Functions stored here///////////////////////////////////
