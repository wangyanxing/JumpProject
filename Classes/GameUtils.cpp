
#include "GameUtils.h"
#include "VisibleRect.h"
#include "chipmunk.h"
#include "GameScene.h"

#define ORG_RECT_SIZE 10

using namespace cocos2d;
using namespace cocos2d::extension;

void GameUtils::setPixelStyleTexture(Sprite* sp) {
    static Texture2D::TexParams param;
    param.minFilter = GL_NEAREST;
    param.magFilter = GL_NEAREST;
    param.wrapS = GL_CLAMP_TO_EDGE;
    param.wrapT = GL_CLAMP_TO_EDGE;
    sp->getTexture()->setTexParameters(param);
}

cocos2d::Sprite* GameUtils::createRect(cocos2d::Rect rect, cocos2d::Color3B col) {
    cocos2d::Sprite* sprite = Sprite::create("images/rect.png");
    setPixelStyleTexture(sprite);
    
    sprite->setScale(rect.size.width / ORG_RECT_SIZE, rect.size.height / ORG_RECT_SIZE);
    sprite->setPosition(rect.origin.x + rect.size.width / 2, rect.origin.y + rect.size.height / 2);
    sprite->setColor(col);
    return sprite;
}

cocos2d::Sprite* GameUtils::createRect(cocos2d::Size size, cocos2d::Vec2 pos, cocos2d::Color3B col) {
    cocos2d::Sprite* sprite = Sprite::create("images/rect.png");
    setPixelStyleTexture(sprite);
    
    sprite->setScale(size.width / ORG_RECT_SIZE, size.height / ORG_RECT_SIZE);
    sprite->setPosition(pos);
    sprite->setColor(col);
    
    return sprite;
}
