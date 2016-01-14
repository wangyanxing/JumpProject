//
//  PhysicsShape.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "PhysicsShape.h"
#include "PhysicsComponent.h"
#include "GameObject.h"
#include "GameRenderer.h"

USING_NS_CC;

void RectPhysicsShape::updateShape(PhysicsComponent *component) {
  auto obj = component->getParent();
  auto renderer = obj->getRenderer();

  mRect = renderer->getBoundingBox();
  auto newSize = Size(mRect.size.width * mScale.x, mRect.size.height * mScale.y);
  mRect.origin.x -= (newSize.width - mRect.size.width) * 0.5f;
  mRect.origin.y -= (newSize.height - mRect.size.height) * 0.5f;
  mRect.size = newSize;
}

void CirclePhysicsShape::updateShape(PhysicsComponent *component) {
  auto obj = component->getParent();
  auto renderer = obj->getRenderer();

  auto size = renderer->getBoundingBox().size;
  mOrigin = renderer->getPosition();
  mRadius = sqrtf(size.width * size.width + size.height * size.height) * mScale;
}