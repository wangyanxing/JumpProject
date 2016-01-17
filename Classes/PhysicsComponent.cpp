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
#include "PhysicsManager.h"
#include "GameLevel.h"
#include "GameObject.h"
#include "GameRenderer.h"

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
  
  auto newpos = mShape->getPosition();
  newpos.y += mVelocity.y * dt;
  newpos.x += mVelocity.x * dt;
  
  // Set back.
  mParent->getRenderer()->setPosition(newpos);
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
  }
  return this;
}

void PhysicsComponent::onCollisionDetected(PhysicsComponent *other) {
  // CCLOG("Collision detected: %d -> %d", mParent->getID(), other->getParent()->getID());
  CC_ASSERT(mPhysicsType == PHYSICS_DYNAMIC);
}
