#include "DelayTracker.h"

DelayTracker::Update(long time) {
  _pollTime = time;
  unsigned long currentTime = millis();

  if (currentTime - _previousPoll >= _pollTime) {
    _previousPoll = currentTime;
    return true;
  } else {
    return false;
  }
}
