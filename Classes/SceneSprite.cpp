//
//  SceneSprite.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/31/15.
//
//

#include "SceneSprite.h"
#include "GameUtils.h"
#include "Defines.h"

USING_NS_CC;

void SceneSprite::create() {
  clean();
  mSprite = GameUtils::createRect(Size, Position, Color);
  mSprite->setOpacity(Opacity);
  mSprite->setTexture(Director::getInstance()->getTextureCache()->addImage(Image));
  mSprite->setCameraMask((unsigned short) CameraFlag::USER2);
}

void SceneSprite::clean() {
  if (mSprite) {
#if EDITOR_MODE
    mSprite->removeFromParent();
#endif
    mSprite = nullptr;
  }
}