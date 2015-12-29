//
//  BlockRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/29/15.
//
//

#include "BlockRenderer.h"
#include "Defines.h"
#include "SpriteUV.h"
#include "GameUtils.h"

USING_NS_CC;

#define CHECK_PARAM(p) CC_ASSERT(param.find(p) != param.end())
#define GET_PARAM(p, T) any_cast<T>(param[p])

RectRenderer::RectRenderer() {
}

RectRenderer::~RectRenderer() {
#if EDITOR_MODE
  mSprite->removeFromParent();
#endif
}

void RectRenderer::init(InitParams& param) {
  CHECK_PARAM(PARAM_RECT);
  CHECK_PARAM(PARAM_COLOR);

  Rect rect = GET_PARAM(PARAM_RECT, Rect);
  Color3B color = GET_PARAM(PARAM_COLOR, Color3B);
  mSprite = GameUtils::createRect(rect, color);
}

void RectRenderer::setPosition(const cocos2d::Vec2& pos) {
  mSprite->setPosition(pos);
}

cocos2d::Vec2 RectRenderer::getPosition() const {
  return mSprite->getPosition();
}

void RectRenderer::setRotation(float val) {
  mSprite->setRotation(val);
}

float RectRenderer::getRotation() const {
  return mSprite->getRotation();
}

void RectRenderer::setScale(float x, float y) {
  mSprite->setScale(x, y);
}

void RectRenderer::setVisible(bool val) {
  mSprite->setVisible(val);
}

bool RectRenderer::isVisible() const {
  return mSprite->isVisible();
}

cocos2d::Size RectRenderer::getContentSize() const {
  return mSprite->getContentSize();
}

void RectRenderer::removePhysicsBody() {
  if (mSprite->getPhysicsBody()) {
    mSprite->removeComponent(mSprite->getPhysicsBody());
  }
}

void RectRenderer::setPhysicsBody(cocos2d::PhysicsBody* body) {
  mSprite->setPhysicsBody(body);
}

cocos2d::PhysicsBody* RectRenderer::getPhysicsBody() {
  return mSprite->getPhysicsBody();
}

void RectRenderer::setColor(const cocos2d::Color3B& color) {
  mSprite->setColor(color);
}

cocos2d::Color3B RectRenderer::getColor() const {
  return mSprite->getColor();
}

void RectRenderer::setZOrder(int val) {
  mSprite->setLocalZOrder(val);
}

void RectRenderer::setTexture(cocos2d::Texture2D* tex) {
  mSprite->setTexture(tex);
}

void RectRenderer::setTexture(const std::string& texName) {
  mSprite->setTexture(texName);
}

float RectRenderer::getScaleX() const {
  return mSprite->getScaleX();
}

float RectRenderer::getScaleY() const {
  return mSprite->getScaleY();
}

void RectRenderer::setOpacity(GLubyte val) {
  mSprite->setOpacity(val);
}

GLubyte RectRenderer::getOpacity() const {
  return mSprite->getOpacity();
}

void RectRenderer::addToParent(cocos2d::Node* parent, int zorder) {
  parent->addChild(mSprite, zorder);
}

cocos2d::Node* RectRenderer::getNode() {
  return mSprite;
}

cocos2d::Rect RectRenderer::getBoundingBox() const {
  return mSprite->getBoundingBox();
}
