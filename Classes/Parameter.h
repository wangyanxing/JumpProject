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
  PARAM_POS,            // cocos2d::Vec2
  PARAM_SIZE,           // cocos2d::Size
  PARAM_COLOR,          // cocos2d::Color3B
  PARAM_COLOR_INDEX,    // int
  PARAM_IMAGE,          // std::string
  PARAM_RENDERER,       // RendererType
  PARAM_PHYSICS_TYPE,   // PhysicsType
  PARAM_SHAPE_TYPE,     // PhysicsShapeType
  PARAM_BLOCK_KIND,     // BlockKind
  PARAM_INPUT,          // InputType
  PARAM_INPUT_STATUS,   // bool
  PARAM_COLLISION_INFO, // CollisionInfo
  PARAM_REMOVABLE,      // bool
  PARAM_RESUME_PATH,    // bool
  PARAM_EDITOR_COMMAND, // EditorCommand
  PARAM_MOUSE_MOVEMENT, // cocos2d::Vec2
  PARAM_SIZE_DELTA,     // cocos2d::Vec2
  PARAM_FIRST_SELECTION,// bool
};

/**
 * Simple parameter class for object initialization.
 */
class Parameter {
public:
  typedef std::unordered_map<ParamType, Any, EnumClassHash> MapT;

  Parameter() = default;

  Parameter(std::initializer_list<MapT::value_type> il) : mData(il) {}

  bool has(ParamType type) const {
    return mData.count(type);
  }

  template<typename T>
  T get(ParamType type) const {
    CC_ASSERT(has(type));
    return any_cast<T>(mData.at(type));
  }

  template<typename T>
  T get(ParamType type, const T &defaultValue) const {
    return has(type) ? any_cast<T>(mData.at(type)) : defaultValue;
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
