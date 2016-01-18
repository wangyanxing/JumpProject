//
//  PhysicsComponent.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "Defines.h"
#include "PhysicsComponent.h"
#include "PhysicsShape.h"
#include "GameLevel.h"
#include "GameObject.h"
#include "GameRenderer.h"
#include "GameEvents.h"

USING_NS_CC;

PhysicsComponent::PhysicsComponent(GameObject *parent) : GameComponent(parent) {
}

PhysicsComponent::~PhysicsComponent() {
  GameLevel::instance().getPhysicsManager()->onDeletePhysicsComponent(this);
}

void PhysicsComponent::update(float dt) {
  if (mPhysicsType == PHYSICS_NONE) {
    return;
  }
  
  if (mPhysicsType == PHYSICS_DYNAMIC && mEnableGravity) {
    mVelocity.y += dt * mGravity;
  }
  
  // Adjust the velocity value.
  static Vec2 maxVelocity{600.0f, 1000.0f}, minVelocity{-600.0f, -1000.0f};
  mVelocity.clamp(minVelocity, maxVelocity);
  
  if (mShape) {
    mShape->updateShape(this);
  }
  
  mShape->mLastPosition = mShape->mPosition;
  mShape->mPosition.y += mVelocity.y * dt;
  mShape->mPosition.x += mVelocity.x * dt;
  
  // Set back.
  mParent->getRenderer()->setPosition(mShape->mPosition);
}

BasePhysicsShape *PhysicsComponent::setShape(PhysicsShapeType type) {
  GameLevel::instance().getPhysicsManager()->removeShape(mShape);
  mShape = GameLevel::instance().getPhysicsManager()->createShape(type);

  // Force update position and size.
  mShape->onPositionSet(mParent->getRenderer()->getPosition());
  mShape->onRotationSet(mParent->getRenderer()->getRotation());
  mShape->onSizeSet(mParent->getRenderer()->getSize());

  return mShape;
}

PhysicsComponent *PhysicsComponent::setPhysicsType(PhysicsType type) {
  auto oldType = mPhysicsType;
  mPhysicsType = type;
  
  GameLevel::instance().getPhysicsManager()->onSetPhysicsType(this, oldType);
  
  mVelocity.setZero();
  if (type == PHYSICS_DYNAMIC) {
    mEnableGravity = true;
  } else if (type != PHYSICS_NONE) {
    mEnableGravity = false;
  }
  return this;
}

void PhysicsComponent::onCollisionDetected(const CollisionInfo &info) {
  CCLOG("Collision detected: %d -> %d, normal: %g, %g",
        mParent->getID(),
        info.component->getParent()->getID(),
        info.normal.x,
        info.normal.y);
  
  CC_ASSERT(mPhysicsType == PHYSICS_DYNAMIC);

  auto other = info.component;
  // Execute event only.
  if (other->getPhysicsType() == PHYSICS_COLLISION_ONLY) {
    callCollisionEvents(other->getParent());
    return;
  }
}

void PhysicsComponent::callCollisionEvents(GameObject *other) {
  for (auto &str : mCollisionEvents) {
    GameEvents::instance().callSingleEvent(str, mParent);
  }
}
