#ifdef ARDUINO
#include <Arduino.h>
#else
#include <chrono>
// Added by Codex on 2025-06-03: provide a millis() stub when not using
// Arduino so DelayTracker can compile in a standard environment.
static auto delaytracker_start = std::chrono::steady_clock::now();
static unsigned long millis() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now() - delaytracker_start)
      .count();
}
#endif
#include "DelayTracker.h"

// Updated by Codex on 2025-06-03: method now explicitly returns bool
bool DelayTracker::Update(long time) {
  _pollTime = time;
  unsigned long currentTime = millis();

  if (currentTime - _previousPoll >= _pollTime) {
    _previousPoll = currentTime;
    return true;
  } else {
    return false;
  }
}
