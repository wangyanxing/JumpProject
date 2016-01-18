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

CollisionInfo PhysicsManager::generateCollisionInfo(PhysicsComponent *objA,
                                                    PhysicsComponent *objB) {
  CollisionInfo info;
  info.component = objB;
  
  auto shapeA = objA->getShape();
  auto shapeB = objB->getShape();
  auto posA = shapeA->getPosition();
  auto posB = shapeB->getPosition();
  auto lastPosA = shapeA->getLastPosition();
  auto lastPosB = shapeB->getLastPosition();
  
  CC_ASSERT(shapeA->getType() == PHYSICS_SHAPE_RECT);
  if (shapeB->getType() == PHYSICS_SHAPE_CIRCLE) {
    info.normal = posB - posA;
    info.normal.normalize();
    return info;
  }
  
  // Just assume the size didn't change, works for now.
  auto sizeA = static_cast<RectPhysicsShape*>(shapeA)->getSize();
  auto sizeB = static_cast<RectPhysicsShape*>(shapeB)->getSize();

  bool interX = fabs(posA.x - posB.x) < 0.5f * (sizeA.width + sizeB.width);
  bool interY = fabs(posA.y - posB.y) < 0.5f * (sizeA.height + sizeB.height);
  bool interXold = fabs(lastPosA.x - lastPosB.x) < 0.5f * (sizeA.width + sizeB.width);
  bool interYold = fabs(lastPosA.y - lastPosB.y) < 0.5f * (sizeA.height + sizeB.height);
  
  if (interXold && interX && interYold && interY) {
    info.normal = lastPosA - posA;
    info.normal.normalize();
  } else if (interXold && interX && !interYold && interY) {
    info.normal.set(0, posA.y > posB.y ? 1 : -1);
  } else {
    info.normal.set(posA.x > posB.x ? 1 : -1, 0);
  }
  return info;
}

void PhysicsManager::detectCollision() {
  // Dynamic with static
  for (auto dynamicA : mDynamicPhysicsObjects) {
    for (auto staticB : mStaticPhysicsObjects) {
      if (dynamicA->getShape()->intersectsTest(staticB->getShape())) {
        dynamicA->onCollisionDetected(generateCollisionInfo(dynamicA, staticB));
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
        dynamicA->onCollisionDetected(generateCollisionInfo(dynamicA, dynamicB));
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
