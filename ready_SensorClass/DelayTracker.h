class DelayTracker {
  unsigned long _previousPoll;  // will store last time temp/humid reading was updated
  long _pollTime;               // how often in milliseconds to poll the temp sensor
  long _duration;

public:
  // Updated by Codex on 2025-06-03: function returns bool
  bool Update(long time);
};
