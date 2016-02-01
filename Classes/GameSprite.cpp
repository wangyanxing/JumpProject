//
//  GameSprite.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 2/1/16.
//
//

#include "GameSprite.h"
#include "GameUtils.h"
#include "JsonFormat.h"

USING_NS_CC;

void GameSprite::create() {
  clean();
  mSprite = GameUtils::createRect(Size, Position, Color);
  mSprite->setOpacity(Opacity);
  mSprite->setTexture(Director::getInstance()->getTextureCache()->addImage(Image));
  mSprite->setCameraMask((unsigned short) CameraFlag::USER2);
}

void GameSprite::clean() {
  if (mSprite) {
    mSprite = nullptr;
  }
}

void GameSprite::load(JsonValueT &var) {
  Image = var[SPRITE_IMAGE].GetString();
  Size = var[SPRITE_SIZE].GetSize();
  Position = var[SPRITE_POSITION].GetVec2();
  ZOrder = var[SPRITE_ZORDER].GetInt();
  Opacity = GLubyte(var[SPRITE_OPACITY].GetInt());
  Color = var[SPRITE_COLOR].GetColor();

  create();
}
