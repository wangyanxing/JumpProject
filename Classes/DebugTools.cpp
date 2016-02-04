//
//  DebugTools.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 2/4/16.
//
//

#include "DebugTools.h"

void DebugTools::logVec(const cocos2d::Vec2 &vec, const std::string &desc) {
  CCLOG("%s: (%g, %g)", desc.c_str(), vec.x, vec.y);
}

void DebugTools::logSize(const cocos2d::Size &size, const std::string &desc) {
  CCLOG("%s: [%g, %g]", desc.c_str(), size.width, size.height);
}
