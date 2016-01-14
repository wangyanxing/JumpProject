//
//  PhysicsComponent.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef PhysicsComponent_h
#define PhysicsComponent_h

#include "GameComponent.h"

class PhysicsComponent : public GameComponent {
public:
  PhysicsComponent(GameObject *parent);

  virtual ~PhysicsComponent();

public:
  virtual void update(float dt) override;

  BasePhysicsShape *setShape(PhysicsShapeType type);

  static ComponentType getType() {
    return COMPONENT_PHYSICS;
  }

  BasePhysicsShape *getShape() {
    return mShape;
  }

  PhysicsType getPhysicsType() {
    return mPhysicsType;
  }

protected:
  void updatePhysicsDebugDraw();

protected:
  PhysicsType mPhysicsType{PHYSICS_NONE};

  BasePhysicsShape *mShape{nullptr};

  cocos2d::DrawNode* mDebugDrawNode{nullptr};
};

#endif /* PhysicsComponent_h */
