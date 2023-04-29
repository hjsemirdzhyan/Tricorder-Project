class Sonar {
  long sonicBurstTime;
  long sonicClearTime;
  long pollFrequencyTime;

  int sonarState;
  unsigned long previousPoll;

public:
  Sonar(long burstTime, long clearTime, long freqTime) {
    sonicBurstTime = burstTime;
    sonicClearTime = clearTime;
    pollFrequencyTime = freqTime;
    previousPoll = 0;
    pinMode(pingPin, OUTPUT);
    pinMode(echoPin, INPUT);
  }
  bool Update() {
    unsigned long currentTime = millis();
    if (currentTime - previousPoll >= pollFrequencyTime) {
      previousPoll = currentTime;
      return true;
    } else {
      return false;
    }
  }
};



