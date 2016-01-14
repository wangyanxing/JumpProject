//
//  PhysicsManager.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#ifndef PhysicsManager_h
#define PhysicsManager_h

#include "Prerequisites.h"

class PhysicsManager {
public:
  PhysicsManager();

  ~PhysicsManager();

  void setPhysicsDebugDraw(bool enable) {
    mPhysicsDebugDraw = enable;
  }

  bool getPhysicsDebugDraw() {
    return mPhysicsDebugDraw;
  }

  void update(float dt);

  BasePhysicsShape *createShape(PhysicsShapeType type);

  void removeShape(BasePhysicsShape *shape);

private:
  void updatePhysicsDebugDraw();

private:
  bool mPhysicsDebugDraw{true};

  cocos2d::DrawNode* mDebugDrawNode{nullptr};

  std::set<BasePhysicsShape*> mShapes;
};

#endif /* PhysicsManager_h */
