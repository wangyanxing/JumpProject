//
//  GameRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#include "GameRenderer.h"
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsShape.h"

USING_NS_CC;

void GameRenderer::setPosition(const cocos2d::Vec2& pos) {
  getNode()->setPosition(pos);

  auto *physics = mParent->getComponent<PhysicsComponent>();
  if (physics && physics->getShape()) {
    physics->getShape()->onPositionSet(pos);
  }
}

cocos2d::Vec2 GameRenderer::getPosition() const {
  return getNode()->getPosition();
}

void GameRenderer::setRotation(float val) {
  getNode()->setRotation(val);

  auto *physics = mParent->getComponent<PhysicsComponent>();
  if (physics && physics->getShape()) {
    physics->getShape()->onRotationSet(val);
  }
}

float GameRenderer::getRotation() const {
  return getNode()->getRotation();
}

void GameRenderer::setScale(float x, float y) {
  getNode()->setScale(x, y);

  auto *physics = mParent->getComponent<PhysicsComponent>();
  if (physics && physics->getShape()) {
    physics->getShape()->onSizeSet(getNode()->getBoundingBox().size);
  }
}

void GameRenderer::setVisible(bool val) {
  getNode()->setVisible(val);
}

bool GameRenderer::isVisible() const {
  return getNode()->isVisible();
}

cocos2d::Size GameRenderer::getContentSize() const {
  return getNode()->getContentSize();
}

cocos2d::Size GameRenderer::getSize() const {
  return getBoundingBox().size;
}

void GameRenderer::setColor(const cocos2d::Color3B& color) {
  getNode()->setColor(color);
}

cocos2d::Color3B GameRenderer::getColor() const {
  return getNode()->getColor();
}

void GameRenderer::setZOrder(int val) {
  getNode()->setLocalZOrder(val);
}

float GameRenderer::getScaleX() const {
  return getNode()->getScaleX();
}

float GameRenderer::getScaleY() const {
  return getNode()->getScaleY();
}

void GameRenderer::setOpacity(GLubyte val) {
  getNode()->setOpacity(val);
}

GLubyte GameRenderer::getOpacity() const {
  return getNode()->getOpacity();
}

void GameRenderer::addToParent(cocos2d::Node* parent, int zorder) {
  parent->addChild(getNode(), zorder);
}

cocos2d::Rect GameRenderer::getBoundingBox() const {
  return getNode()->getBoundingBox();
}
