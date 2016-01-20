//
//  PathComponent.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/20/16.
//
//

#include "PathComponent.h"

USING_NS_CC;

PathComponent::PathComponent(GameObject *parent) : GameComponent(parent) {
}

PathComponent::~PathComponent() {
  clear();
}

void PathComponent::update(float dt) {
}

void PathComponent::load(JsonValueT &json) {
  clear();

  mSpeed = json["pathSpeed"].GetDouble();
  mPingPong = json["pingpong"].GetBool();
  mPause = json["pause"].GetBool();
  mOriginalPause = mPause;
  mPathWaitTime = json["pathWaitTime"].GetDouble();

  auto size = json["pathes"].Size();
  for (auto i = 0; i < size; ++i) {
    auto &pa = json["pathes"][i];

    Vec2 pos = pa["position"].GetVec2();
    float waittime = pa["waittime"].GetDouble();
    float width = pa["width"].GetDouble();
    float height = pa["height"].GetDouble();

    push(pos, waittime, width, height);
  }
}

void PathComponent::update(float dt, Vec2 &out, Vec2 &outsize) {
  if (mPoints.empty() || mPoints.size() == 1) {
    return;
  }
  if (mPause) {
    dt = 0;
  }

  if (mDisable) {
    out = mPoints[0].pt;
  } else {
    // Process path timer
    if (mPathWaitingTimer > mPathWaitTime) {
      int nextPt = nextPoint();

      if (mWaitingTimer > mPoints[mCurPt].waitTime) {
        auto pt = mPoints[mCurPt];
        auto ptNext = mPoints[nextPt];
        float dist = pt.pt.distance(ptNext.pt);

        mCurDist += mSpeed * dt;
        if (mCurDist >= dist) {
          // Move to next point
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
        auto dir = ptNext.pt - pt.pt;
        dir.x /= dist;
        dir.y /= dist;
        out = pt.pt + dir * mCurDist;

        float ratio = mCurDist / dist;
        outsize.x = pt.width * (1 - ratio) + ptNext.width * ratio;
        outsize.y = pt.height * (1 - ratio) + ptNext.height * ratio;

      } else {
        out = mPoints[mCurPt].pt;
        mWaitingTimer += dt;
      }
    } else {
      mPathWaitingTimer += dt;
    }
  }
}

void PathComponent::reset() {
  mDirection = true;
  mCurPt = 0;
  mCurDist = 0;
  mWaitingTimer = 0;
  mPathWaitingTimer = 0;
}

size_t PathComponent::getNumPoints() {
  return mPoints.size();
}

const PathComponent::PathPoint &PathComponent::getPoint(size_t i) const {
  return mPoints[i];
}

bool PathComponent::empty() {
  return mPoints.empty();
}

void PathComponent::clear() {
  while (!empty()) {
    pop();
  }
}

cocos2d::Vec2 PathComponent::getBackPos() {
  return mPoints.back().pt;
}

void PathComponent::setBackPos(const Vec2 &pos) {
  mPoints.back().pt = pos;
}

void PathComponent::translatePoints(const Vec2 &d) {
  for (size_t i = 0; i < mPoints.size(); ++i) {
    mPoints[i].pt += d;
  }
}

void PathComponent::pop() {
  if (empty()) {
    return;
  }
  mPoints.pop_back();
}

void PathComponent::push(const Vec2 &pos, float waitTime, float width, float height) {
  PathPoint pt(pos, waitTime, width, height);
  mPoints.push_back(pt);
}

int PathComponent::nextPoint() {
  int nextPt;
  if (mPingPong) {
    nextPt = mCurPt + (mDirection ?  1 : - 1);
    nextPt %= mPoints.size();
  } else {
    nextPt = (mCurPt + 1) % mPoints.size();
  }
  return nextPt;
}
