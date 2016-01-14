//
//  SimpleRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "SimpleRenderer.h"
#include "SpriteUV.h"
#include "Defines.h"
#include "GameUtils.h"

USING_NS_CC;

SimpleRenderer::SimpleRenderer(GameObject *parent) : GameRenderer(parent) {
}

SimpleRenderer::~SimpleRenderer() {
#if EDITOR_MODE
  mSprite->removeFromParent();
#endif
}

void SimpleRenderer::init(Parameter &param) {
  CHECK_PARAM(PARAM_POS);
  CHECK_PARAM(PARAM_SIZE);
  CHECK_PARAM(PARAM_COLOR);

  Size size = GET_PARAM(PARAM_SIZE, Size);
  Vec2 pos = GET_PARAM(PARAM_POS, Vec2);
  Color3B color = GET_PARAM(PARAM_COLOR, Color3B);

  mSprite = GameUtils::createRect(size, pos, color);
}

cocos2d::Node* SimpleRenderer::getNode() {
  return mSprite;
}

cocos2d::Node* SimpleRenderer::getNode() const {
  return mSprite;
}
