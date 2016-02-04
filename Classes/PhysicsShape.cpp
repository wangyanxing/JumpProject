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

static const int CIRCLE_SEG_NUM = 12;

void BasePhysicsShape::onPositionSet(const cocos2d::Vec2 &position) {
  mLastPosition = mPosition;
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

Rect RectPhysicsShape::getBounds() {
  auto size = Size(mSize.width * mScale.x, mSize.height * mScale.y);
  return Rect(mPosition.x - size.width * 0.5f,
              mPosition.y - size.height * 0.5f,
              mSize.width,
              mSize.height);
}

void RectPhysicsShape::debugDraw(cocos2d::DrawNode *node,
                                 const cocos2d::Color4F &fillColor,
                                 const cocos2d::Color4F &lineColor) {
  auto rect = getBounds();
  Vec2 seg[4] = {
    {-rect.size.width * 0.5f,  rect.size.height * 0.5f},
    { rect.size.width * 0.5f,  rect.size.height * 0.5f},
    { rect.size.width * 0.5f, -rect.size.height * 0.5f},
    {-rect.size.width * 0.5f, -rect.size.height * 0.5f}
  };
  node->drawPolygon(seg, 4, fillColor, 1, lineColor);
}

bool RectPhysicsShape::intersectsTest(BasePhysicsShape *other) {
  return other->intersectsTest(getBounds());
}

bool RectPhysicsShape::intersectsTest(const cocos2d::Rect &rect) {
  return getBounds().intersectsRect(rect);
}

bool RectPhysicsShape::intersectsTest(const cocos2d::Vec2 &pos, float radius) {
  return getBounds().intersectsCircle(pos, radius);
}

bool RectPhysicsShape::containsPoint(const cocos2d::Vec2 &pos) {
  return getBounds().containsPoint(pos);
}

void CirclePhysicsShape::updateShape(PhysicsComponent *component) {
}

void CirclePhysicsShape::onSizeSet(const cocos2d::Size &size) {
  mRadius = sqrtf(size.width * size.width + size.height * size.height) * 0.5f;
}

void CirclePhysicsShape::debugDraw(cocos2d::DrawNode *node,
                                   const cocos2d::Color4F &fillColor,
                                   const cocos2d::Color4F &lineColor) {
  Vec2 seg[CIRCLE_SEG_NUM] = {};
  float radius = mRadius * mScale;
  for (int i = 0; i < CIRCLE_SEG_NUM; ++i) {
    float angle = (float)i * M_PI / (float)CIRCLE_SEG_NUM * 2.0f;
    Vec2 d(radius * cosf(angle), radius * sinf(angle));
    seg[i] = d;
  }
  node->drawPolygon(seg, CIRCLE_SEG_NUM, fillColor, 1, lineColor);
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

bool CirclePhysicsShape::containsPoint(const cocos2d::Vec2 &pos) {
  return pos.distance(getPosition()) <= getRealRadius();
}

Rect CirclePhysicsShape::getBounds() {
  float radius = getRealRadius();
  return Rect(mPosition.x - radius,
              mPosition.y - radius,
              radius * 2,
              radius * 2);
}
