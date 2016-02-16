//
//  TimeEvent.cpp
//  JumpEdt
//
//  Created by Yongfeng on 11/17/14.
//
//

#include "TimeEvent.h"
#include "GameEvents.h"

TimeEvent::TimeEventPoint::TimeEventPoint(float wt) : waitTime(wt) {
}

TimeEvent::TimeEventPoint::~TimeEventPoint() {
  mEvents.clear();
}

void TimeEvent::TimeEventPoint::push(std::string event) {
  mEvents.push_back(event);
}

void TimeEvent::TimeEventPoint::trigger() {
  if (mEvents.empty()) {
    return;
  }

  for (auto it = mEvents.begin(); it != mEvents.end(); ++it) {
    GameEvents::instance().callSingleEvent(*it, nullptr);
  }
}

void TimeEvent::reset() {
  mEnalbe = true;
  mCurEventIndex = 0;
  mWaitingTimer = 0.0f;
  mEventWaitingTimer = mInitDelay;
}

TimeEvent::TimeEvent() {
}

TimeEvent::~TimeEvent() {
  mEventPoints.clear();
}

void TimeEvent::update(float dt) {
  auto eventPonitNum = mEventPoints.size();
  if (mCurEventIndex >= eventPonitNum && !mLoop) {
    mEnalbe = false;
  }
  if (mEventPoints.empty() || !mEnalbe) {
    return;
  }

  if (mWaitingTimer > mEventWaitingTimer) {
    mEventPoints[mCurEventIndex].trigger();
    mEventWaitingTimer = mEventPoints[mCurEventIndex].waitTime;
    mWaitingTimer = 0.0f;
    mCurEventIndex++;

    if (mCurEventIndex >= eventPonitNum) {
      if (mLoop) {
        mCurEventIndex = 0;
      } else {
        mEnalbe = false;
      }
    }
  }
  mWaitingTimer += dt;
}
