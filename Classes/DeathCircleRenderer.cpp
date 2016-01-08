//
//  DeathCircleRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/7/16.
//
//

#include "DeathCircleRenderer.h"
#include "GameUtils.h"
#include "SpriteUV.h"

USING_NS_CC;

DeathCircleRenderer::DeathCircleRenderer(BlockBase *parent) : RectRenderer(parent) {
}

DeathCircleRenderer::~DeathCircleRenderer() {
#if EDITOR_MODE
  mChild->removeFromParent();
#endif
}

void DeathCircleRenderer::init(InitParams& param) {
  RectRenderer::init(param);
  Rect rect = GET_PARAM(PARAM_RECT, Rect);
  mChild = GameUtils::createRect(rect, mSprite->getColor());
}

void DeathCircleRenderer::addToParent(cocos2d::Node* parent, int zorder) {
  BlockRenderer::addToParent(parent, zorder);
  parent->addChild(mChild, zorder);
}

void DeathCircleRenderer::setPosition(const cocos2d::Vec2& pos) {
  BlockRenderer::setPosition(pos);
  mChild->setPosition(pos);
}

void DeathCircleRenderer::setRotation(float val) {
  BlockRenderer::setRotation(val);
  mChild->setRotation(45 - val);
}

void DeathCircleRenderer::setScale(float x, float y) {
  BlockRenderer::setScale(x, y);
  mChild->setScale(x, y);
}

void DeathCircleRenderer::setColor(const cocos2d::Color3B& color) {
  BlockRenderer::setColor(color);
  mChild->setColor(color);
}
