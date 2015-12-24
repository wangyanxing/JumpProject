//
//  Path.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__Path__
#define __JumpEdt__Path__

#include "Defines.h"
#include "cocos2d.h"

class Path {
public:
  Path() {
#if EDITOR_MODE
    mHelperNode = cocos2d::Node::create();
    mHelperNode->setPosition(0, 0);
    mSegmentNode = cocos2d::DrawNode::create();
    mHelperNode->addChild(mSegmentNode, 90);
    mHelperNode->setCameraMask((unsigned short) cocos2d::CameraFlag::USER2);
#endif
  }

  virtual ~Path() {
    clear();
#if EDITOR_MODE
    mSegmentNode->removeFromParent();
    mHelperNode->removeFromParent();
#endif
  }

  struct PathPoint {
    PathPoint(const cocos2d::Vec2 &p, float wt, float w, float h) {
      pt = p;
      waitTime = wt;
      width = w;
      height = h;
    }

    cocos2d::Vec2 pt;

    float waitTime{-1};

    float width{1};

    float height{1};
  };

  int nextPoint();

  void reset() {
    mDirection = true;
    mCurPt = 0;
    mCurDist = 0;
    mWaitingTimer = 0;
    mPathWaitingTimer = 0;
  }

  void cloneFrom(const Path &rsh, const cocos2d::Vec2 &posBias);

  size_t getNumPoints() {
    return mPoints.size();
  }

  const PathPoint &getPoint(size_t i) const {
    return mPoints[i];
  }

  void update(float dt, cocos2d::Vec2 &out, cocos2d::Vec2 &outsize);

  bool empty() {
    return mPoints.empty();
  }

  void clear() {
    while (!empty()) {
      pop();
    }
  }

  void pop();

  cocos2d::Vec2 getBackPos() {
    auto back = mPoints.back();
    return back.pt;
  }

  void setBackPos(const cocos2d::Vec2 &pos) {
    mPoints.back().pt = pos;
#if EDITOR_MODE
    auto node = mHelperNode->getChildByTag(mPoints.size() - 1);
    node->setPosition(pos);
#endif
  }

  void translatePoints(const cocos2d::Vec2 &d) {
    for (size_t i = 0; i < mPoints.size(); ++i) {
      mPoints[i].pt += d;
#if EDITOR_MODE
      auto node = mHelperNode->getChildByTag(i);
      node->setPosition(mPoints[i].pt);
#endif
    }
  }

  void push(const cocos2d::Vec2 &pos, float waitTime = -1, float width = 1, float height = 1);

#if EDITOR_MODE

  void setSegmentNodeScale(float scale) {
    for (size_t i = 0; i < mPoints.size(); ++i) {
      auto node = mHelperNode->getChildByTag(i);
      node->setScale(scale);
    }
  }

  void updateHelper();

#endif

  bool mPingPong{true};

  float mSpeed{50};

  float mPathWaitTime{-1};

  bool mDirection{true};

  int mCurPt{0};

  float mCurDist{0};

  float mWaitingTimer{0};

  float mPathWaitingTimer{0};

  bool mDisable{true};

  bool mPause{false};

  bool mOriginalPause{false};

#if EDITOR_MODE
  cocos2d::DrawNode *mSegmentNode{nullptr};

  cocos2d::Node *mHelperNode{nullptr};
#endif
private:

  std::vector <PathPoint> mPoints;
};


#endif /* defined(__JumpEdt__Path__) */
