/*
  Main sketch
*/

///////////////////////////////////Classes stored here///////////////////////////////////
class Flasher
{
    // Class Member Variables
    // These are initialized at startup
    int ledPin;      // the number of the LED pin
    long OnTime;     // milliseconds of on-time
    long OffTime;    // milliseconds of off-time
     
    // These maintain the current state
    int ledState;             		// ledState used to set the LED
    unsigned long previousMillis;  	// will store last time LED was updated
     
  // Constructor - creates a Flasher 
  // and initializes the member variables and state
  public:
  Flasher(int pin, long on, long off)
  {
    ledPin = pin;
    pinMode(ledPin, OUTPUT);     
    	  
    OnTime = on;
    OffTime = off;
    	
    ledState = LOW; 
    previousMillis = 0;
  }
     
  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
         
    if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
        ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(ledPin, ledState);	  // Update the actual LED
    }
  }
};
///////////////////////////////////Inits stored here///////////////////////////////////
Flasher led1(13, 350, 350);

///////////////////////////////////Setup and loop stored here///////////////////////////////////
void setup()
{
  Serial.begin(9600);             // for analogReadSerial() function
}

void loop()
{ 
  led1.Update();
  analogReadSerial();
}


///////////////////////////////////Functions stored here///////////////////////////////////
void analogReadSerial()
{
  int sensorValue = analogRead(A0);
  // print out the value you read:
  Serial.println(sensorValue);
  delay(1);  // delay in between reads for stability
}

void buzzer(int speakerPin)
{
  digitalWrite(speakerPin, HIGH);
  int result = digitalRead(speakerPin);
  Serial.println(result);
}

