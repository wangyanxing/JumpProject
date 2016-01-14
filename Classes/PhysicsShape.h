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

class PhysicsShape {
public:
  PhysicsShape() = default;

  virtual ~PhysicsShape() = default;

  virtual PhysicsShapeType getType() = 0;
};

/**
 * Rectangle physics shape.
 */
class RectPhysicsShape : public PhysicsShape {
public:
  PhysicsShapeType getType() {
    return PHYSICS_SHAPE_CIRCLE;
  }

  cocos2d::Vec2 getScale() {
    return mScale;
  }

  void setScale(const cocos2d::Vec2 &scale) {
    mScale = scale;
  }

private:
  cocos2d::Vec2 mScale{1, 1};
};

/**
 * Circle physics shape.
 */
class CirclePhysicsShape : public PhysicsShape {
public:
  PhysicsShapeType getType() {
    return PHYSICS_SHAPE_RECT;
  }

  float getScale() {
    return mScale;
  }

  void setScale(float scale) {
    mScale = scale;
  }

private:
  float mScale{1};
};

#endif /* PhysicsShape_h */
