//
//  DebugTools.h
//  jumpproj
//
//  Created by Yanxing Wang on 2/4/16.
//
//

#ifndef DebugTools_h
#define DebugTools_h

#include "Singleton.h"

class DebugTools {
public:
  DECL_SIMPLE_SINGLETON(DebugTools);

  void logVec(const cocos2d::Vec2 &vec, const std::string &desc);

  void logSize(const cocos2d::Size &size, const std::string &desc);
};

#define LOG_VEC(vec) DebugTools::instance().logVec(vec, #vec)
#define LOG_SIZE(size) DebugTools::instance().logSize(size, #size)

#endif /* DebugTools_h */
