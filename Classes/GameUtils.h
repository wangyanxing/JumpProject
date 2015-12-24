#ifndef __GAME_UTILS_H__
#define __GAME_UTILS_H__

#include "cocos2d.h"
#include "cocos-ext.h"

class SpriteUV;

struct GameUtils {
  static void logVec2(const cocos2d::Vec2 &vec2);

  static void setPixelStyleTexture(cocos2d::Sprite *sp);

  static SpriteUV *createRect(cocos2d::Rect rect, cocos2d::Color3B col, bool setCameraMask = true);

  static SpriteUV *createRect(cocos2d::Size size, cocos2d::Vec2 pos,
                              cocos2d::Color3B col, bool setCameraMask = true);
};

#endif