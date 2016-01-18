//
//  PhysicsComponent.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef PhysicsComponent_h
#define PhysicsComponent_h

#include "PhysicsManager.h"
#include "GameComponent.h"

#define DEFAULT_LINEAR_DAMPING 5.0f
#define DEFAULT_GRAVITY -1100

/**
 * Physics component.
 */
class PhysicsComponent : public GameComponent {
public:
  PhysicsComponent(GameObject *parent);

  virtual ~PhysicsComponent();
  
  DECLARE_COMP_TYPE(COMPONENT_PHYSICS);

public:
  virtual void update(float dt) override;

  BasePhysicsShape *setShape(PhysicsShapeType type);

  BasePhysicsShape *getShape() {
    return mShape;
  }

  PhysicsType getPhysicsType() const {
    return mPhysicsType;
  }
  
  PhysicsComponent *setPhysicsType(PhysicsType type);
  
  void onCollisionDetected(const CollisionInfo &info);
  
  void callCollisionEvents(GameObject *other);

protected:
  PhysicsType mPhysicsType{PHYSICS_NONE};

  BasePhysicsShape *mShape{nullptr};
  
  cocos2d::Vec2 mVelocity;
  
  float mDamping{DEFAULT_LINEAR_DAMPING};
  
  float mGravity{DEFAULT_GRAVITY};
  
  bool mEnableGravity{false};

  std::vector<std::string> mCollisionEvents;
};

#endif /* PhysicsComponent_h */
