//
//  TimeEvent.h
//  JumpEdt
//
//  Created by Yongfeng on 11/17/14.
//
//

#ifndef __JumpEdt__TimeEvent__
#define __JumpEdt__TimeEvent__

class TimeEvent {
public:
  TimeEvent();

  ~TimeEvent();

  struct TimeEventPoint {
    TimeEventPoint(float wt) {
      waitTime = wt;
    }

    ~TimeEventPoint() {
      mEvents.clear();
    }

    void trigger();

    void push(std::string event) {
      mEvents.push_back(event);
    }

    float waitTime{-1};

    std::vector <std::string> mEvents;
  };

  void reset();

  void update(float dt);

  bool mLoop{false};

  bool mEnalbe{true};

  float mInitDelay{-1};

  int mCurEventIndex{0};

  float mWaitingTimer{0};

  float mEventWaitingTimer{0};

  std::vector <TimeEventPoint> mEventPoints;
};

#endif /* #define(__JumpEdt__TimeEvent__) */
