//
//  PhysicsShape.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef PhysicsShape_h
#define PhysicsShape_h

#include "Prerequisites.h"

class BasePhysicsShape {
public:
  friend class PhysicsComponent;

  BasePhysicsShape() = default;

  virtual ~BasePhysicsShape() = default;

  virtual PhysicsShapeType getType() = 0;

  virtual void updateShape(PhysicsComponent *component) = 0;
};

/**
 * Rectangle physics shape.
 */
class RectPhysicsShape : public BasePhysicsShape {
public:
  PhysicsShapeType getType() override {
    return PHYSICS_SHAPE_CIRCLE;
  }

  cocos2d::Vec2 getScale() {
    return mScale;
  }

  void setScale(const cocos2d::Vec2 &scale) {
    mScale = scale;
  }

  cocos2d::Rect getRect() {
    return mRect;
  }

  void updateShape(PhysicsComponent *component) override;

protected:
  cocos2d::Vec2 mScale{1, 1};

  cocos2d::Rect mRect;
};

/**
 * Circle physics shape.
 */
class CirclePhysicsShape : public BasePhysicsShape {
public:
  PhysicsShapeType getType() override {
    return PHYSICS_SHAPE_RECT;
  }

  float getScale() {
    return mScale;
  }

  void setScale(float scale) {
    mScale = scale;
  }

  float getRadius() {
    return mRadius;
  }

  cocos2d::Vec2 getOrigin() {
    return mOrigin;
  }

  void updateShape(PhysicsComponent *component) override;

protected:
  float mScale{1};

  float mRadius{0};

  cocos2d::Vec2 mOrigin;
};

#endif /* PhysicsShape_h */
