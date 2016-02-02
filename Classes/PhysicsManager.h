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
  
  PhysicsComponent *obj1{nullptr};
  
  PhysicsComponent *obj2{nullptr};
};

/**
 * Physics manager class.
 */
class PhysicsManager {
public:
  PhysicsManager();

  ~PhysicsManager();

  void cleanUp();

  void update(float dt);

  void beforeRender(float dt);

  BasePhysicsShape *createShape(PhysicsShapeType type);
  
  void onSetPhysicsType(PhysicsComponent *component, PhysicsType oldType);
  
  void onDeletePhysicsComponent(PhysicsComponent *component);

private:
  void detectCollision();
  
  CollisionInfo generateCollisionInfo(PhysicsComponent *objA, PhysicsComponent *objB);

private:
  std::set<PhysicsComponent*> mStaticPhysicsObjects;
  
  std::set<PhysicsComponent*> mDynamicPhysicsObjects;
};

#endif /* PhysicsManager_h */
