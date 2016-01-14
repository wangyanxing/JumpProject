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

USING_NS_CC;

PhysicsComponent::PhysicsComponent(GameObject *parent) : GameComponent(parent) {
  mDebugDrawNode = DrawNode::create();
  Director::getInstance()->getRunningScene()->addChild(mDebugDrawNode, ZORDER_PHYSICS_DEBUG);
}

PhysicsComponent::~PhysicsComponent() {
  mDebugDrawNode->removeFromParent();
}

void PhysicsComponent::update(float dt) {
  if (mShape) {
    mShape->updateShape(this);
  }
  updatePhysicsDebugDraw();
}

BasePhysicsShape *PhysicsComponent::setShape(PhysicsShapeType type) {
  CC_SAFE_DELETE(mShape);
  if (type == PHYSICS_SHAPE_CIRCLE) {
    mShape = new CirclePhysicsShape();
  } else {
    mShape = new RectPhysicsShape();
  }
  return mShape;
}

void PhysicsComponent::updatePhysicsDebugDraw() {
  mDebugDrawNode->clear();
  if (GameLevel::instance().getPhysicsDebugDraw() && mShape) {
    if (mShape->getType() == PHYSICS_SHAPE_CIRCLE) {

    } else {

    }
  }
}
