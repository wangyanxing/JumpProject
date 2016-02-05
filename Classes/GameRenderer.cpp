//
//  GameRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#include "GameRenderer.h"
#include "GameObject.h"
#include "GameLevel.h"
#include "ColorPalette.h"
#include "PhysicsComponent.h"
#include "PhysicsShape.h"
#include "JsonFormat.h"

USING_NS_CC;

void GameRenderer::reset() {
  getNode()->stopAllActions();
  setPosition(mOriginalPosition);
  setRotation(mOriginalRotation);
  setSize(mOriginalSize);
}

void GameRenderer::load(JsonValueT &json) {
  if (json.HasMember(RENDERER_SHADOW)) {
    mShadowEnabled = json[RENDERER_SHADOW].GetBool();
  }
  if (json.HasMember(RENDERER_SHADOW_LAYER)) {
    mShadowLayer = json[RENDERER_SHADOW_LAYER].GetInt();
  }
}

void GameRenderer::clone(GameRenderer *renderer) {
  mShadowEnabled = renderer->mShadowEnabled;
  mShadowLayer = renderer->mShadowLayer;
  mOriginalPosition = renderer->mOriginalPosition;
  mOriginalSize = renderer->mOriginalSize;
}

void GameRenderer::save(JsWriter &writer) {
  writer.String(LEVEL_BLOCK_SIZE);
  writer.Size(mOriginalSize);

  writer.String(LEVEL_BLOCK_POS);
  writer.Vec2(mOriginalPosition);

  if (!mShadowEnabled) {
    writer.String(RENDERER_SHADOW);
    writer.Bool(mShadowEnabled);
  }

  if (mShadowLayer != 0) {
    writer.String(RENDERER_SHADOW_LAYER);
    writer.Int(mShadowLayer);
  }

  if (mColorIndex != DEFAULT_COLOR_ID) {
    writer.String(LEVEL_BLOCK_PALETTE_ID);
    writer.Int(mColorIndex);
  }
}

void GameRenderer::setOriginalPosition(const cocos2d::Vec2 &pos) {
  mOriginalPosition = pos;
}

void GameRenderer::setPosition(const cocos2d::Vec2& pos) {
  getNode()->setPosition(pos);

  auto *physics = mParent->getComponent<PhysicsComponent>();
  if (physics && physics->getShape()) {
    physics->getShape()->onPositionSet(pos);
  }
}

void GameRenderer::move(const cocos2d::Vec2 &delta) {
  setPosition(getPosition() + delta);
  mOriginalPosition = getPosition();
}

Vec2 GameRenderer::getPosition() const {
  return getNode()->getPosition();
}

Vec2 GameRenderer::getOriginalPosition() const {
  return mOriginalPosition;
}

Size GameRenderer::getOriginalSize() const {
  return mOriginalSize;
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

Size GameRenderer::getContentSize() const {
  return getNode()->getContentSize();
}

Size GameRenderer::getSize() const {
  return getBoundingBox().size;
}

void GameRenderer::setSize(const cocos2d::Size &size) {
  auto contentSize = getContentSize();
  getNode()->setScale(size.width / contentSize.width, size.height / contentSize.height);

  auto *physics = mParent->getComponent<PhysicsComponent>();
  if (physics && physics->getShape()) {
    physics->getShape()->onSizeSet(size);
  }
}

void GameRenderer::reSize(const cocos2d::Vec2 &delta) {
  auto size = getSize();
  size.width += delta.x;
  size.height += delta.y;
  setSize(size);
  mOriginalSize = size;
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

Rect GameRenderer::getBoundingBox() const {
  return getNode()->getBoundingBox();
}

void GameRenderer::setColorIndex(int id) {
  mColorIndex = id;
  setColor(GameLevel::instance().getPalette()->getColor(mParent->getKind(), id));
}

int GameRenderer::getColorIndex() const {
  return mColorIndex;
}

void GameRenderer::getPointsForShadow(const cocos2d::Vec2 &source,
                                      std::vector <cocos2d::Vec2> &out) {
  auto size = getBoundingBox().size;
  auto p = getPosition();
  out.resize(4);
  out[0] = p + Vec2(-size.width / 2, size.height / 2);
  out[2] = p + Vec2(-size.width / 2, -size.height / 2);
  out[1] = p + Vec2(size.width / 2, size.height / 2);
  out[3] = p + Vec2(size.width / 2, -size.height / 2);
}
