//
//  MathTools.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 2/6/16.
//
//

#include "MathTools.h"

USING_NS_CC;

bool MathTools::vec2Equal(const cocos2d::Vec2 &v1, const cocos2d::Vec2 &v2) {
  return fabs(v1.x - v2.x) < std::numeric_limits<float>::epsilon() &&
    fabs(v1.y - v2.y) < std::numeric_limits<float>::epsilon();
}

bool MathTools::floatEqual(float f1, float f2) {
  return fabs(f1 - f2) < std::numeric_limits<float>::epsilon();
}
