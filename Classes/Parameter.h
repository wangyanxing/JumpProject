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

#define CHECK_PARAM(p) CC_ASSERT(param.find(p) != param.end())
#define GET_PARAM(p, T) any_cast<T>(param[p])

enum ParamType {
  PARAM_POS,     // cocos2d::Vec2
  PARAM_SIZE,    // cocos2d::Size
  PARAM_COLOR,   // cocos2d::Color3B
  PARAM_IMAGE,   // std::string
  PARAM_RENDERER // RendererType
};

typedef std::map<ParamType, Any> Parameter;

#endif /* Parameter_h */
