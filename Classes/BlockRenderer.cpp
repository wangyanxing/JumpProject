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

void BlockRenderer::setPosition(const cocos2d::Vec2& pos) {
  getNode()->setPosition(pos);
}

cocos2d::Vec2 BlockRenderer::getPosition() const {
  return getNode()->getPosition();
}

void BlockRenderer::setRotation(float val) {
  getNode()->setRotation(val);
}

float BlockRenderer::getRotation() const {
  return getNode()->getRotation();
}

void BlockRenderer::setScale(float x, float y) {
  getNode()->setScale(x, y);
}

void BlockRenderer::setVisible(bool val) {
  getNode()->setVisible(val);
}

bool BlockRenderer::isVisible() const {
  return getNode()->isVisible();
}

cocos2d::Size BlockRenderer::getContentSize() const {
  return getNode()->getContentSize();
}

void BlockRenderer::removePhysicsBody() {
  if (getNode()->getPhysicsBody()) {
    getNode()->removeComponent(getNode()->getPhysicsBody());
  }
}

void BlockRenderer::setPhysicsBody(cocos2d::PhysicsBody* body) {
  getNode()->setPhysicsBody(body);
}

cocos2d::PhysicsBody* BlockRenderer::getPhysicsBody() {
  return getNode()->getPhysicsBody();
}

void BlockRenderer::setColor(const cocos2d::Color3B& color) {
  getNode()->setColor(color);
}

cocos2d::Color3B BlockRenderer::getColor() const {
  return getNode()->getColor();
}

void BlockRenderer::setZOrder(int val) {
  getNode()->setLocalZOrder(val);
}

float BlockRenderer::getScaleX() const {
  return getNode()->getScaleX();
}

float BlockRenderer::getScaleY() const {
  return getNode()->getScaleY();
}

void BlockRenderer::setOpacity(GLubyte val) {
  getNode()->setOpacity(val);
}

GLubyte BlockRenderer::getOpacity() const {
  return getNode()->getOpacity();
}

void BlockRenderer::addToParent(cocos2d::Node* parent, int zorder) {
  parent->addChild(getNode(), zorder);
}

cocos2d::Rect BlockRenderer::getBoundingBox() const {
  return getNode()->getBoundingBox();
}

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

void RectRenderer::setTexture(cocos2d::Texture2D* tex) {
  mSprite->setTexture(tex);
}

void RectRenderer::setTexture(const std::string& texName) {
  mSprite->setTexture(texName);
}

cocos2d::Node* RectRenderer::getNode() {
  return mSprite;
}

cocos2d::Node* RectRenderer::getNode() const {
  return mSprite;
}
