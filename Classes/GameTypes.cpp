//
//  GameTypes.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "GameTypes.h"

USING_NS_CC;

std::string EnumUtil::toString(RendererType kind) {
  return (std::string[]) {
    "RECT",
    "DEATH_CIRCLE",
    "IMAGE"
  }[kind];
}

RendererType toRendererType(const std::string& str) {
  static std::map<std::string, RendererType> kinds = {
    {"HERO",  RENDERER_RECT},
    {"BLOCK", RENDERER_DEATH_CIRCLE},
    {"DEATH", RENDERER_IMAGE}
  };

  if (!kinds.count(str)) {
    CCLOGWARN("Invalid renderer type: %s", str.c_str());
    return RENDERER_RECT;
  }
  return kinds[str];
}

std::string EnumUtil::toString(ObjectKind kind) {
  return (std::string[]) {
    "HERO",
    "BLOCK",
    "DEATH",
    "DEATH_CIRCLE",
    "BUTTON",
    "PUSHABLE"
  }[kind];
}
