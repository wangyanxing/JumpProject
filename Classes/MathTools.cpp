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
