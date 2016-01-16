//
//  Parameter.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#ifndef Parameter_h
#define Parameter_h

#include "Any.h"

enum ParamType {
  PARAM_POS,          // cocos2d::Vec2
  PARAM_SIZE,         // cocos2d::Size
  PARAM_COLOR,        // cocos2d::Color3B
  PARAM_COLOR_INDEX,  // int
  PARAM_IMAGE,        // std::string
  PARAM_RENDERER,     // RendererType
  PARAM_PHYSICS_TYPE, // PhysicsType
  PARAM_SHAPE_TYPE,   // PhysicsShapeType
  PARAM_BLOCK_KIND    // BlockKind
};

/**
 * Simple parameter class for object initialization.
 */
class Parameter {
public:
  typedef std::map<ParamType, Any> MapT;

  bool has(ParamType type) {
    return mData.count(type);
  }

  template<typename T>
  T get(ParamType type) {
    CC_ASSERT(has(type));
    return any_cast<T>(mData[type]);
  }

  template<typename T>
  T get(ParamType type, const T &defaultValue) {
    return has(type) ? any_cast<T>(mData[type]) : defaultValue;
  }

  template<typename T>
  Parameter &set(ParamType type, const T &value) {
    mData[type] = value;
    return *this;
  }

private:
  MapT mData;
};

#endif /* Parameter_h */
