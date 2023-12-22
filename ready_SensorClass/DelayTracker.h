#include "SPI.h"

class DelayTracker {
  unsigned long _previousPoll;  // will store last time temp/humid reading was updated
  long _pollTime;               // how often in milliseconds to poll the temp sensor
  long _duration;

public:
  Update(long time);
};