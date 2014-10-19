#ifndef __GAME_UTILS_H__
#define __GAME_UTILS_H__

#include "cocos2d.h"
#include "cocos-ext.h"

struct GameUtils {

    static void setPixelStyleTexture(cocos2d::Sprite* sp);
    
    static cocos2d::Sprite* createRect(cocos2d::Rect rect, cocos2d::Color3B col);
    
    static cocos2d::Sprite* createRect(cocos2d::Size size, cocos2d::Vec2 pos, cocos2d::Color3B col);

};

#endif