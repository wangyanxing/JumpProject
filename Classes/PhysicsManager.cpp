//
//  PhysicsManager.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#include "PhysicsManager.h"
#include "Defines.h"
#include "GameLevel.h"
#include "GameObject.h"
#include "PhysicsShape.h"
#include "PhysicsComponent.h"

USING_NS_CC;

PhysicsManager::PhysicsManager() {
  mDebugDrawNode = DrawNode::create();
  mDebugDrawNode->setCameraMask((unsigned short) CameraFlag::USER2);
  mDebugDrawNode->retain();
}

PhysicsManager::~PhysicsManager() {
  mDebugDrawNode->removeFromParent();
  mDebugDrawNode->release();

  for (auto shape : mShapes) {
    CC_SAFE_DELETE(shape);
  }
  mShapes.clear();
}

void PhysicsManager::updatePhysicsDebugDraw() {
  if (!mDebugDrawNode->getParent()) {
    Director::getInstance()->getRunningScene()->addChild(mDebugDrawNode, ZORDER_PHYSICS_DEBUG);
  }

  mDebugDrawNode->clear();
  if (mPhysicsDebugDraw) {
    for (auto shape : mShapes) {
      shape->debugDraw(mDebugDrawNode);
    }
  }
}

void PhysicsManager::update(float dt) {
  detectCollision();
  updatePhysicsDebugDraw();
}

void PhysicsManager::detectCollision() {
  // Dynamic with static
  for (auto dynamicA : mDynamicPhysicsObjects) {
    for (auto staticB : mStaticPhysicsObjects) {
      if (dynamicA->getShape()->intersectsTest(staticB->getShape())) {
        dynamicA->onCollisionDetected(staticB);
      }
    }
  }
  
  // Dynamic with dynamic
  for (auto dynamicA : mDynamicPhysicsObjects) {
    for (auto dynamicB : mDynamicPhysicsObjects) {
      auto objA = dynamicA->getParent();
      auto objB = dynamicB->getParent();
      
      if (objA->getID() < objB->getID() &&
          dynamicA->getShape()->intersectsTest(dynamicB->getShape())) {
        dynamicA->onCollisionDetected(dynamicB);
      }
    }
  }
}

void PhysicsManager::onSetPhysicsType(PhysicsComponent *component, PhysicsType oldType) {
  if (oldType == PHYSICS_DYNAMIC) {
    CC_ASSERT(mDynamicPhysicsObjects.count(component));
    mDynamicPhysicsObjects.erase(component);
  } else if (oldType != PHYSICS_NONE) {
    CC_ASSERT(mStaticPhysicsObjects.count(component));
    mStaticPhysicsObjects.erase(component);
  }
  
  if (component->getPhysicsType() == PHYSICS_DYNAMIC) {
    CC_ASSERT(!mDynamicPhysicsObjects.count(component));
    mDynamicPhysicsObjects.insert(component);
  } else if (component->getPhysicsType() != PHYSICS_NONE) {
    CC_ASSERT(!mStaticPhysicsObjects.count(component));
    mStaticPhysicsObjects.insert(component);
  }
}

void PhysicsManager::onDeletePhysicsComponent(PhysicsComponent *component) {
  if (component->getPhysicsType() == PHYSICS_DYNAMIC) {
    CC_ASSERT(mDynamicPhysicsObjects.count(component));
    mDynamicPhysicsObjects.erase(component);
  } else if (component->getPhysicsType() != PHYSICS_NONE) {
    CC_ASSERT(mStaticPhysicsObjects.count(component));
    mStaticPhysicsObjects.erase(component);
  }
}

BasePhysicsShape *PhysicsManager::createShape(PhysicsShapeType type) {
  BasePhysicsShape *ret = nullptr;
  if (type == PHYSICS_SHAPE_CIRCLE) {
    ret = new CirclePhysicsShape();
  } else {
    ret = new RectPhysicsShape();
  }
  mShapes.insert(ret);
  return ret;
}

void PhysicsManager::removeShape(BasePhysicsShape *shape) {
  if (shape) {
    mShapes.erase(shape);
    delete shape;
  }
}
