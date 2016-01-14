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
  updatePhysicsDebugDraw();
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
