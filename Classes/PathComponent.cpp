//
//  PathComponent.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/20/16.
//
//

#include "PathComponent.h"
#include "JsonFormat.h"
#include "GameObject.h"
#include "GameRenderer.h"

USING_NS_CC;

PathComponent::PathComponent(GameObject *parent) : GameComponent(parent) {
}

PathComponent::~PathComponent() {
  clear();
}

void PathComponent::update(float dt) {
  auto renderer = mParent->getRenderer();
  Vec2 pos = renderer->getPosition();
  Size size = renderer->getSize();
  Vec2 scale = Vec2::ONE;

  update(dt, pos, scale);

  renderer->setPosition(pos);
  renderer->setSize({size.width * scale.x, size.height * scale.y});
}

void PathComponent::load(JsonValueT &json) {
  clear();

  mSpeed = json[PATH_SPEED].GetDouble();
  mPingPong = json[PATH_PINGPONG].GetBool();
  mPause = json[PATH_PAUSE].GetBool();
  mPathWaitTime = json[PATH_WAIT_TIME].GetDouble();

  mOriginalPause = mPause;

  auto size = json[PATH_ARRAY].Size();
  for (auto i = 0; i < size; ++i) {
    auto &pa = json[PATH_ARRAY][i];

    Vec2 pos = pa[PATH_PT_POSITION].GetVec2();
    float waittime = pa[PATH_PT_WAIT_TIME].GetDouble();
    float width = pa[PATH_PT_WIDTH].GetDouble();
    float height = pa[PATH_PT_HEIGHT].GetDouble();

    push(pos, waittime, width, height);
  }
}

void PathComponent::update(float dt, Vec2 &out, Vec2 &outScale) {
  if (mPoints.size() <= 1) {
    return;
  }

  dt = mPause ? 0 : dt;

  // Process path timer.
  if (mPathWaitingTimer > mPathWaitTime) {
    int nextPt = nextPoint();

    if (mWaitingTimer > mPoints[mCurPt].waitTime) {
      auto pt = mPoints[mCurPt];
      auto ptNext = mPoints[nextPt];
      float dist = pt.pt.distance(ptNext.pt);

      mCurDist += mSpeed * dt;
      if (mCurDist >= dist) {
        // Move to next point.
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
      outScale.x = pt.width * (1 - ratio) + ptNext.width * ratio;
      outScale.y = pt.height * (1 - ratio) + ptNext.height * ratio;

    } else {
      out = mPoints[mCurPt].pt;
      mWaitingTimer += dt;
    }
  } else {
    mPathWaitingTimer += dt;
  }
}

void PathComponent::reset() {
  mDirection = true;
  mCurPt = 0;
  mCurDist = 0;
  mWaitingTimer = 0;
  mPathWaitingTimer = 0;
  mPause = mOriginalPause;
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