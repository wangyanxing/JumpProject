#ifndef __GAME_UTILS_H__
#define __GAME_UTILS_H__

#include "cocos-ext.h"

class SpriteUV;

struct GameUtils {
  static void logVec2(const cocos2d::Vec2 &vec2);

  static bool vec2Equal(const cocos2d::Vec2 &v1, const cocos2d::Vec2 &v2);

  static void setPixelStyleTexture(cocos2d::Sprite *sp);

  static SpriteUV *createRect(cocos2d::Rect rect,
                              cocos2d::Color3B col = cocos2d::Color3B::WHITE,
                              bool setCameraMask = true);

  static SpriteUV *createRect(cocos2d::Size size,
                              cocos2d::Vec2 pos,
                              cocos2d::Color3B col = cocos2d::Color3B::WHITE,
                              bool setCameraMask = true);
};

#define LOG_VEC(v) GameUtils::logVec2(v)

#endif