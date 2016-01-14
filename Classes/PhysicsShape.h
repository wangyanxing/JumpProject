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

  virtual void debugDraw(cocos2d::DrawNode *node) = 0;

  cocos2d::Vec2 getPosition() {
    return mPosition;
  }

  virtual void onPositionSet(const cocos2d::Vec2 &position);

  virtual void onRotationSet(float rotation);

  virtual void onSizeSet(const cocos2d::Size &size) = 0;

  float getRotation() {
    return mRotation;
  }

protected:
  cocos2d::Vec2 mPosition;

  float mRotation{0};
};

/**
 * Rectangle physics shape.
 */
class RectPhysicsShape : public BasePhysicsShape {
public:
  PhysicsShapeType getType() override {
    return PHYSICS_SHAPE_RECT;
  }

  cocos2d::Vec2 getScale() {
    return mScale;
  }

  void setScale(const cocos2d::Vec2 &scale) {
    mScale = scale;
  }

  cocos2d::Size getSize() {
    return mSize;
  }

  void onSizeSet(const cocos2d::Size &size) override;

  void updateShape(PhysicsComponent *component) override;

  void debugDraw(cocos2d::DrawNode *node) override;

protected:
  cocos2d::Vec2 mScale{1, 1};

  cocos2d::Size mSize;
};

/**
 * Circle physics shape.
 */
class CirclePhysicsShape : public BasePhysicsShape {
public:
  PhysicsShapeType getType() override {
    return PHYSICS_SHAPE_CIRCLE;
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

  void onSizeSet(const cocos2d::Size &size) override;

  void updateShape(PhysicsComponent *component) override;

  void debugDraw(cocos2d::DrawNode *node) override;  

protected:
  float mScale{1};

  float mRadius{0};
};

#endif /* PhysicsShape_h */
