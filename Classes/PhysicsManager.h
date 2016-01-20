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

struct CollisionInfo {
  cocos2d::Vec2 normal;
  PhysicsComponent *component{nullptr};
};

/**
 * Physics manager class.
 */
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

  void beforeRender(float dt);

  BasePhysicsShape *createShape(PhysicsShapeType type);

  void removeShape(BasePhysicsShape *shape);
  
  void onSetPhysicsType(PhysicsComponent *component, PhysicsType oldType);
  
  void onDeletePhysicsComponent(PhysicsComponent *component);

private:
  void updatePhysicsDebugDraw();
  
  void detectCollision();
  
  CollisionInfo generateCollisionInfo(PhysicsComponent *objA, PhysicsComponent *objB);

private:
  bool mPhysicsDebugDraw{false};

  cocos2d::DrawNode* mDebugDrawNode{nullptr};

  std::set<BasePhysicsShape*> mShapes;
  
  std::set<PhysicsComponent*> mStaticPhysicsObjects;
  
  std::set<PhysicsComponent*> mDynamicPhysicsObjects;
};

#endif /* PhysicsManager_h */
