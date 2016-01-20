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
}

void PhysicsManager::beforeRender(float dt) {
  updatePhysicsDebugDraw();
}

CollisionInfo PhysicsManager::generateCollisionInfo(PhysicsComponent *objA,
                                                    PhysicsComponent *objB) {
  CollisionInfo info;
  info.component = objB;

  auto shapeA = objA->getShape(), shapeB = objB->getShape();
  auto posA = shapeA->getPosition(), posB = shapeB->getPosition();
  auto boundsA = shapeA->getBounds(), boundsB = shapeB->getBounds();

  CC_ASSERT(shapeA->getType() == PHYSICS_SHAPE_RECT);
  if (shapeB->getType() == PHYSICS_SHAPE_CIRCLE) {
    info.normal = posB - posA;
    info.normal.normalize();
    return info;
  }

  float x = std::max(boundsA.origin.x, boundsB.origin.x);
  float num1 = std::min(boundsA.origin.x + boundsA.size.width,
                        boundsB.origin.x + boundsB.size.width);
  float y = std::max(boundsA.origin.y, boundsB.origin.y);
  float num2 = std::min(boundsA.origin.y + boundsA.size.height,
                        boundsB.origin.y + boundsB.size.height);
  float intersetsWidth = num1 - x, intersetsHeight = num2 - y;

  if (intersetsWidth > intersetsHeight) {
    info.normal.set(0, posA.y > posB.y ? 1 : -1);
  } else {
    info.normal.set(posA.x > posB.x ? 1 : -1, 0);
  }
  return info;
}

void PhysicsManager::detectCollision() {
  // Dynamic with static.
  for (auto dynamicA : mDynamicPhysicsObjects) {
    for (auto staticB : mStaticPhysicsObjects) {
      if (dynamicA->getShape()->intersectsTest(staticB->getShape())) {
        dynamicA->onCollisionDetected(generateCollisionInfo(dynamicA, staticB));
      }
    }
  }
  
  // Dynamic with dynamic.
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
