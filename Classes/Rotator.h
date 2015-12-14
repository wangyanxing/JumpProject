//
//  Rotator.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__Rotator__
#define __JumpEdt__Rotator__

#include "Defines.h"
#include "cocos2d.h"

class Rotator {
public:

  Rotator() {
  }

  virtual ~Rotator() {
    clear();
  }

  struct RotatorPoint {
    RotatorPoint(float p, float wt, float w, float h) {
      rotation = p;
      waitTime = wt;
      width = w;
      height = h;
    }

    float rotation{ 0 };
    float waitTime{ -1 };
    float width{ 1 };
    float height{ 1 };
  };

  int nextPoint();

  void reset() {
    mDirection = true;
    mCurPt = 0;
    mCurDist = 0;
    mWaitingTimer = 0;
    mPathWaitingTimer = 0;
  }

  void cloneFrom(const Rotator& rsh, float bias);

  size_t getNumPoints() {
    return mPoints.size();
  }

  const RotatorPoint& getPoint(size_t i) const {
    return mPoints[i];
  }

  void update(float dt, float& out, cocos2d::Vec2& outsize);

  bool empty() {
    return mPoints.empty();
  }

  void clear() {
    while(!empty()) pop();
  }

  void pop();

  float getBackRotation() {
    auto back = mPoints.back();
    return back.rotation;
  }

  void setBackRotation(float r) {
    mPoints.back().rotation = r;
  }

  void addRotations(float d) {
    for(size_t i = 0; i < mPoints.size(); ++i) {
      mPoints[i].rotation += d;
    }
  }

  void push(float r, float waitTime = -1, float width = 1, float height = 1);

  bool mPingPong{ true };
  float mSpeed{ 1 };
  float mPathWaitTime{ -1 };
  bool mDirection{ true };
  int mCurPt{ 0 };
  float mCurDist{ 0 };
  float mWaitingTimer{ 0 };
  float mPathWaitingTimer{ 0 };
  bool mDisable{ true };
  bool mPause{ false };

private:

  std::vector<RotatorPoint> mPoints;
};


#endif /* defined(__JumpEdt__Rotator__) */
