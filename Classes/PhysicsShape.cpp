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

static const Color4F fillColor(1.0f, 0.0f, 0.0f, 0.3f);
static const Color4F outlineColor(1.0f, 0.0f, 0.0f, 1.0f);
static const int CIRCLE_SEG_NUM = 12;

void BasePhysicsShape::onPositionSet(const cocos2d::Vec2 &position) {
  mPosition = position;
}

void BasePhysicsShape::onRotationSet(float rotation) {
  mRotation = rotation;
}

void RectPhysicsShape::updateShape(PhysicsComponent *component) {
}

void RectPhysicsShape::onSizeSet(const cocos2d::Size &size) {
  mSize = size;
}

Rect RectPhysicsShape::getRect() const {
  auto size = Size(mSize.width * mScale.x, mSize.height * mScale.y);
  return Rect(mPosition.x - size.width * 0.5f,
              mPosition.y - size.height * 0.5f,
              mSize.width,
              mSize.height);
}

void RectPhysicsShape::debugDraw(cocos2d::DrawNode *node) {
  auto rect = getRect();
  Vec2 seg[4] = {
    {mPosition.x - rect.size.width * 0.5f, mPosition.y + rect.size.height * 0.5f},
    {mPosition.x + rect.size.width * 0.5f, mPosition.y + rect.size.height * 0.5f},
    {mPosition.x + rect.size.width * 0.5f, mPosition.y - rect.size.height * 0.5f},
    {mPosition.x - rect.size.width * 0.5f, mPosition.y - rect.size.height * 0.5f}
  };
  node->drawPolygon(seg, 4, fillColor, 1, outlineColor);
}

bool RectPhysicsShape::intersectsTest(BasePhysicsShape *other) {
  return other->intersectsTest(getRect());
}

bool RectPhysicsShape::intersectsTest(const cocos2d::Rect &rect) {
  return getRect().intersectsRect(rect);
}

bool RectPhysicsShape::intersectsTest(const cocos2d::Vec2 &pos, float radius) {
  return getRect().intersectsCircle(pos, radius);
}

void CirclePhysicsShape::updateShape(PhysicsComponent *component) {
}

void CirclePhysicsShape::onSizeSet(const cocos2d::Size &size) {
  mRadius = sqrtf(size.width * size.width + size.height * size.height) * 0.5f;
}

void CirclePhysicsShape::debugDraw(cocos2d::DrawNode *node) {
  Vec2 seg[CIRCLE_SEG_NUM] = {};
  float radius = mRadius * mScale;
  for (int i = 0; i < CIRCLE_SEG_NUM; ++i) {
    float angle = (float)i * M_PI / (float)CIRCLE_SEG_NUM * 2.0f;
    Vec2 d(radius * cosf(angle), radius * sinf(angle));
    seg[i] = mPosition + d;
  }
  node->drawPolygon(seg, CIRCLE_SEG_NUM, fillColor, 1, outlineColor);
}

bool CirclePhysicsShape::intersectsTest(BasePhysicsShape *other) {
  return other->intersectsTest(getPosition(), getRealRadius());
}

bool CirclePhysicsShape::intersectsTest(const cocos2d::Rect &rect) {
  return rect.intersectsCircle(getPosition(), getRealRadius());
}

bool CirclePhysicsShape::intersectsTest(const cocos2d::Vec2 &pos, float radius) {
  return pos.distance(getPosition()) <= getRealRadius() + radius;
}
