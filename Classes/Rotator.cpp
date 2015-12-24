//
//  Rotator.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "Rotator.h"

void Rotator::update(float dt, float &out, cocos2d::Vec2 &outsize) {
  if (mPoints.empty()) {
    return;
  }

  if (mPause) {
    dt = 0;
  }

  if (mDisable) {
    out = mPoints[0].rotation;

  } else {

    // process path timer
    if (mPathWaitingTimer > mPathWaitTime) {
      int nextPt = nextPoint();

      if (mWaitingTimer > mPoints[mCurPt].waitTime) {
        auto pt = mPoints[mCurPt];
        auto ptNext = mPoints[nextPt];
        float dist = std::abs(pt.rotation - ptNext.rotation);

        mCurDist += mSpeed * dt;
        if (mCurDist >= dist) {
          // move to next point
          mCurDist -= dist;
          mWaitingTimer = 0;

          if (mDirection) {
            if (nextPt == mPoints.size() - 1 && mPingPong) {
              mDirection = !mDirection;
            }
          } else {
            if (nextPt == 0 && mPingPong) {
              mDirection = !mDirection;
            }
          }
          mCurPt = nextPt;
          nextPt = nextPoint();

          pt = mPoints[mCurPt];
          ptNext = mPoints[nextPt];
        }

        float dt = ptNext.rotation - pt.rotation;
        out = pt.rotation + dt * mCurDist;

        float ratio = mCurDist / dist;
        outsize.x = pt.width * (1 - ratio) + ptNext.width * ratio;
        outsize.y = pt.height * (1 - ratio) + ptNext.height * ratio;

      } else {
        mWaitingTimer += dt;
      }

    } else {
      mPathWaitingTimer += dt;
    }
  }
}

void Rotator::pop() {
  if (empty()) return;
  mPoints.pop_back();
}

void Rotator::push(float r, float waitTime, float width, float height) {
  RotatorPoint pt(r, waitTime, width, height);
  mPoints.push_back(pt);
}

void Rotator::cloneFrom(const Rotator &rsh, float bias) {
  reset();
  for (size_t i = 0; i < rsh.mPoints.size(); ++i) {
    const auto &p = rsh.mPoints[i];
    push(p.rotation + bias, p.waitTime, p.width, p.height);
  }
  mSpeed = rsh.mSpeed;
  mPingPong = rsh.mPingPong;
  mPause = rsh.mPause;
  mPathWaitTime = rsh.mPathWaitTime;
}

int Rotator::nextPoint() {
  int nextPt;
  if (mPingPong) {
    nextPt = mDirection ? mCurPt + 1 : mCurPt - 1;
    nextPt %= mPoints.size();
  } else {
    nextPt = (mCurPt + 1) % mPoints.size();
  }
  return nextPt;
}