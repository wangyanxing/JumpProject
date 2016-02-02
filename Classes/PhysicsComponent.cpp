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
#include "JsonFormat.h"
#include "GameLevel.h"
#include "GameObject.h"
#include "GameRenderer.h"
#include "GameEvents.h"
#include "GameUtils.h"
#include "GameConfig.h"

USING_NS_CC;

PhysicsComponent::PhysicsComponent(GameObject *parent) : GameComponent(parent) {
}

PhysicsComponent::~PhysicsComponent() {
  GameLevel::instance().getPhysicsManager()->onDeletePhysicsComponent(this);
  CC_SAFE_DELETE(mShape);
}

void PhysicsComponent::update(float dt) {
  if (mPhysicsType == PHYSICS_DYNAMIC) {
    updateDynamics(dt);
    clearStates();
  }
}

void PhysicsComponent::beforeRender(float dt) {
  mParent->getRenderer()->setPosition(mShape->mPosition);
}

void PhysicsComponent::updateDynamics(float dt) {
  if (mStatus == ON_PLATFORM) {
    mVelocity.y = 0;
  } else if (mStatus == FALLING) {
    mAcceleration.y = 0;
    if (mPhysicsType == PHYSICS_DYNAMIC && mEnableGravity) {
      mVelocity.y += dt * mGravity;
    }
  }

  mAcceleration.x *= mAccelerationResistance;
  mVelocity += mAcceleration * dt;
  mVelocity.x *= std::min(std::max(1.0f - dt * mDamping, 0.0f), 1.0f);

  // Adjust the velocity value.
  static Vec2 maxVelocity{600.0f, 1000.0f}, minVelocity{-600.0f, -1000.0f};
  mVelocity.clamp(minVelocity, maxVelocity);

  mShape->updateShape(this);
  mShape->mLastPosition = mShape->mPosition;
  mShape->mPosition.y += mVelocity.y * dt;
  mShape->mPosition.x += mVelocity.x * dt;
}

void PhysicsComponent::reset() {
  GameComponent::reset();
  clearStates();
}

void PhysicsComponent::load(JsonValueT &json) {
  if (json.HasMember(PHYSICS_EVENTS)) {
    mCollisionEvents = json[PHYSICS_EVENTS].getStringVec();
  }
}

void PhysicsComponent::save(JsWriter &writer) {
  if (!mCollisionEvents.empty()) {
    writer.String(PHYSICS_EVENTS);
    writer.StartArray();
    for (auto &str : mCollisionEvents) {
      writer.String(str);
    }
    writer.EndArray();
  }
}

BasePhysicsShape *PhysicsComponent::setShape(PhysicsShapeType type) {
  CC_SAFE_DELETE(mShape);
  mShape = GameLevel::instance().getPhysicsManager()->createShape(type);

  // Force update position and size.
  mShape->onPositionSet(mParent->getRenderer()->getPosition());
  mShape->onRotationSet(mParent->getRenderer()->getRotation());
  mShape->onSizeSet(mParent->getRenderer()->getSize());

  return mShape;
}

void PhysicsComponent::clearStates() {
  mSleep = mPhysicsType != PHYSICS_DYNAMIC;
  mAcceleration.setZero();
  mAccelerationResistance = 1;
  mStatus = FALLING;
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
  clearStates();
  return this;
}

void PhysicsComponent::onCollisionDetected(const CollisionInfo &info) {
  CC_ASSERT(mPhysicsType == PHYSICS_DYNAMIC);

  auto other = info.obj2;
  // Execute event only.
  if (other->getPhysicsType() == PHYSICS_COLLISION_ONLY) {
    if (other->getParent()->hasCommand(COMMAND_COLLISION)) {
      other->getParent()->runCommand(COMMAND_COLLISION, {{PARAM_COLLISION_INFO, Any(&info)}});
    }
    other->callCollisionEvents(other->getParent());
    return;
  }

  float halfHeight = 0.5f * (getShape()->getBounds().size.height +
                             info.obj2->getShape()->getBounds().size.height);
  float deltaHeight = fabs(getShape()->getPosition().y -
                           info.obj2->getShape()->getPosition().y);
  float halfWidth = 0.5f * (getShape()->getBounds().size.width +
                            info.obj2->getShape()->getBounds().size.width);
  float deltaWidth = fabs(getShape()->getPosition().x -
                          info.obj2->getShape()->getPosition().x);

  if (GameUtils::vec2Equal(Vec2::UNIT_Y, info.normal)) {
    mStatus = ON_PLATFORM;
    mShape->mPosition.y += halfHeight - deltaHeight;

    // If the paltform moved in this frame, update it.
    mShape->mPosition += other->mShape->getMovement();
  } else if (GameUtils::vec2Equal(-Vec2::UNIT_Y, info.normal)) {
    mVelocity.y = 0;
    mShape->mPosition.y -= halfHeight - deltaHeight;
  } else if (GameUtils::vec2Equal(Vec2::UNIT_X, info.normal) ||
             GameUtils::vec2Equal(-Vec2::UNIT_X, info.normal)) {
    if (other->getPhysicsType() == PHYSICS_DYNAMIC && !other->mSleep) {
      // Push it.
      mAccelerationResistance = GameConfig::instance().AccelerationResistance;
      other->mShape->mPosition.x -= (halfWidth - deltaWidth) * info.normal.x;
    } else {
      mSleep = true;
      mVelocity.x = 0;
      mShape->mPosition.x += (halfWidth - deltaWidth) * info.normal.x;
    }
  }
}

void PhysicsComponent::callCollisionEvents(GameObject *other) {
  for (auto &str : mCollisionEvents) {
    GameEvents::instance().callSingleEvent(str, mParent);
  }
}

void PhysicsComponent::updateHelpers() {
  mHelperNode->clear();
  if (mShape) {
    mShape->debugDraw(mHelperNode);
  }
}

void PhysicsComponent::initHelpers() {
  mHelperNode = DrawNode::create();
  mHelperNode->setCameraMask((unsigned short) CameraFlag::USER2);
  mParent->getHelperNode()->addChild(mHelperNode, ZORDER_EDT_HELPER_PHYSICS);
}
