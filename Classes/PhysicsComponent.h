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

#define DEFAULT_LINEAR_DAMPING 8.0f
#define DEFAULT_GRAVITY -1100

/**
 * Physics component.
 */
class PhysicsComponent : public GameComponent {
public:
  PhysicsComponent(GameObject *parent);

  virtual ~PhysicsComponent();
  
  DECLARE_COMP_TYPE(COMPONENT_PHYSICS);

  enum Status {
    STATIC,
    ON_PLATFORM,
    FALLING
  };

public:
  virtual void update(float dt) override;
  
  virtual void beforeRender(float dt) override;

  virtual void reset() override;

  virtual void load(JsonValueT &json) override;

  virtual void save(JsWriter &writer) override;

  virtual void clone(GameComponent *other) override;

  void updateDynamics(float dt);

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

  Status getStatus() const {
    return mStatus;
  }
  
  cocos2d::Vec2 getVelocity() const {
    return mVelocity;
  }
  
  void addVelocity(const cocos2d::Vec2 &vel) {
    mVelocity += vel;
  }
  
  void setVelocity(const cocos2d::Vec2 &vel) {
    mVelocity = vel;
  }
  
  cocos2d::Vec2 getAcceleration() {
    return mAcceleration;
  }
  
  void setAccelerationX(float val) {
    mAcceleration.x = val;
  }
  
  void setAccelerationY(float val) {
    mAcceleration.y = val;
  }
  
  void markSleep() {
    mSleep = true;
  }

  void updateHelpers() override;

  void initHelpers() override;

  void releaseHelpers();

  void setCollisionEvents(const std::vector<std::string> &events) {
    mCollisionEvents = events;
  }

protected:
  void clearStates();

protected:
  PhysicsType mPhysicsType{PHYSICS_NONE};

  BasePhysicsShape *mShape{nullptr};
  
  cocos2d::Vec2 mVelocity;
  
  cocos2d::Vec2 mAcceleration;

  float mAccelerationResistance{1};
  
  float mDamping{DEFAULT_LINEAR_DAMPING};
  
  float mGravity{DEFAULT_GRAVITY};
  
  bool mEnableGravity{false};

  bool mSleep{false};

  std::vector<std::string> mCollisionEvents;

  Status mStatus{FALLING};

  cocos2d::DrawNode *mHelperNode{nullptr};
};

#endif /* PhysicsComponent_h */
