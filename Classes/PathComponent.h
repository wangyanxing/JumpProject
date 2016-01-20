//
//  PathComponent.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/20/16.
//
//

#ifndef PathComponent_h
#define PathComponent_h

#include "Prerequisites.h"
#include "GameComponent.h"

class PathComponent : public GameComponent {
public:
  DECLARE_COMP_TYPE(COMPONENT_PATH);

  PathComponent(GameObject *parent);

  virtual ~PathComponent();

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

public:
  void update(float dt) override;

  void load(JsonValueT &json) override;

  int nextPoint();

  void update(float dt, cocos2d::Vec2 &out, cocos2d::Vec2 &outsize);

  void reset() override;

  size_t getNumPoints();

  const PathPoint &getPoint(size_t i) const;

  bool empty();

  void clear();

  cocos2d::Vec2 getBackPos();

  void setBackPos(const cocos2d::Vec2 &pos);

  void translatePoints(const cocos2d::Vec2 &d);

  void push(const cocos2d::Vec2 &pos, float waitTime = -1, float width = 1, float height = 1);

  void pop();

private:
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

  std::vector <PathPoint> mPoints;
};

#endif /* PathComponent_h */
