#include "GameUtils.h"
#include "VisibleRect.h"
#include "chipmunk.h"
#include "EditorScene.h"
#include "SpriteUV.h"
#include "Defines.h"

using namespace cocos2d;
using namespace cocos2d::extension;

bool GameUtils::vec2Equal(const cocos2d::Vec2 &v1, const cocos2d::Vec2 &v2) {
  return fabs(v1.x - v1.x) < std::numeric_limits<float>::epsilon() &&
    fabs(v1.y - v1.y) < std::numeric_limits<float>::epsilon();
}

void GameUtils::logVec2(const cocos2d::Vec2 &vec2) {
  CCLOG("x = %.2f, y = %.2f", vec2.x, vec2.y);
}

void GameUtils::setPixelStyleTexture(Sprite *sp) {
  static Texture2D::TexParams param;
  param.minFilter = GL_NEAREST;
  param.magFilter = GL_NEAREST;
  param.wrapS = GL_REPEAT;
  param.wrapT = GL_REPEAT;
  sp->getTexture()->setTexParameters(param);
}

SpriteUV *GameUtils::createRect(cocos2d::Rect rect, cocos2d::Color3B col, bool setCameraMask) {
  SpriteUV *sprite = SpriteUV::create(BLOCK_IMAGE);
  setPixelStyleTexture(sprite);

  sprite->setScale(rect.size.width / ORG_RECT_SIZE, rect.size.height / ORG_RECT_SIZE);
  sprite->setPosition(rect.origin.x + rect.size.width / 2, rect.origin.y + rect.size.height / 2);
  sprite->setColor(col);
  sprite->setContentSize(cocos2d::Size(ORG_RECT_SIZE, ORG_RECT_SIZE));
  if (setCameraMask) {
    sprite->setCameraMask((unsigned short) CameraFlag::USER2);
  }
  return sprite;
}

SpriteUV *GameUtils::createRect(cocos2d::Size size, cocos2d::Vec2 pos,
                                cocos2d::Color3B col, bool setCameraMask) {
  SpriteUV *sprite = SpriteUV::create(BLOCK_IMAGE);
  setPixelStyleTexture(sprite);

  sprite->setScale(size.width / ORG_RECT_SIZE, size.height / ORG_RECT_SIZE);
  sprite->setPosition(pos);
  sprite->setColor(col);
  sprite->setContentSize(cocos2d::Size(ORG_RECT_SIZE, ORG_RECT_SIZE));
  if (setCameraMask) {
    sprite->setCameraMask((unsigned short) CameraFlag::USER2);
  }
  return sprite;
}
