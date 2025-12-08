#pragma once

// Global frame counter maintained by the legacy runtime.
extern long Frame;

class TCountDownTimerClass {
 public:
  explicit TCountDownTimerClass(long set = 0) { Set(set); }
  ~TCountDownTimerClass() = default;

  operator long() const { return Time(); }

  void Set(long set) {
    Started = Frame;
    DelayTime = set;
  }

  void Clear() {
    Started = -1;
    DelayTime = 0;
  }

  long Get_Start() const { return Started; }
  long Get_Delay() const { return DelayTime; }
  bool Active() const { return Started != -1; }
  int Expired() const { return Time() == 0; }

  long Time() const {
    if (!Active()) {
      return 0;
    }
    long remain = DelayTime - (Frame - Started);
    return remain > 0 ? remain : 0;
  }

 protected:
  long Started = -1;
  long DelayTime = 0;
};
