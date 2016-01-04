//
//  BlockRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/29/15.
//
//

#include "BlockRenderer.h"
#include "Blocks.h"
#include "Defines.h"
#include "SpriteUV.h"
#include "GameUtils.h"
#include "LogicManager.h"

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

RectRenderer::RectRenderer(BlockBase *parent) : BlockRenderer(parent) {
}

RectRenderer::~RectRenderer() {
#if EDITOR_MODE
  mSprite->removeFromParent();
#endif
  GameLogic::Game->mBlockTable.erase(mSprite);
}

void RectRenderer::init(InitParams& param) {
  CHECK_PARAM(PARAM_RECT);
  CHECK_PARAM(PARAM_COLOR);

  Rect rect = GET_PARAM(PARAM_RECT, Rect);
  Color3B color = GET_PARAM(PARAM_COLOR, Color3B);
  mSprite = GameUtils::createRect(rect, color);
  GameLogic::Game->mBlockTable[mSprite] = mParentBlock;
}

void RectRenderer::setTexture(const std::string& texName) {
  TextureName = texName;
  Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(texName);
  mSprite->setTexture(texture);
}

cocos2d::Node* RectRenderer::getNode() {
  return mSprite;
}

cocos2d::Node* RectRenderer::getNode() const {
  return mSprite;
}

void RectRenderer::normalizeUV() {
  if (TextureName != "images/saw.png" && TextureName != "images/saw_r.png") {
    mSprite->resetUV();
    return;
  }

  auto w = mParentBlock->getWidth();
  auto h = mParentBlock->getThickness();
  mSprite->resetUV();
  if (w >= h) {
    if (TextureName != "images/saw.png") {
      TextureName = "images/saw.png";
      Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(TextureName);
      mSprite->setTexture(texture);
    }

    Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE};
    mSprite->getTexture()->setTexParameters(params);

    mSprite->setUVWidth(w / h);
    if (mParentBlock->mUVFlipped) {
      mSprite->flipUVY();
    }
  } else {
    if (TextureName != "images/saw_r.png") {
      TextureName = "images/saw_r.png";
      Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(TextureName);
      mSprite->setTexture(texture);
    }

    Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_REPEAT};
    mSprite->getTexture()->setTexParameters(params);

    mSprite->setUVHeight(h / w);
    if (mParentBlock->mUVFlipped) {
      mSprite->flipUVX();
    }
  }
}

DirthCircleRenderer::DirthCircleRenderer(BlockBase *parent) : RectRenderer(parent) {
}

DirthCircleRenderer::~DirthCircleRenderer() {
#if EDITOR_MODE
  mChild->removeFromParent();
#endif
}

void DirthCircleRenderer::init(InitParams& param) {
  RectRenderer::init(param);
  Rect rect = GET_PARAM(PARAM_RECT, Rect);
  mChild = GameUtils::createRect(rect, mSprite->getColor());
}

void DirthCircleRenderer::addToParent(cocos2d::Node* parent, int zorder) {
  BlockRenderer::addToParent(parent, zorder);
  parent->addChild(mChild, zorder);
}

void DirthCircleRenderer::setPosition(const cocos2d::Vec2& pos) {
  BlockRenderer::setPosition(pos);
  mChild->setPosition(pos);
}

void DirthCircleRenderer::setRotation(float val) {
  BlockRenderer::setRotation(val);
  mChild->setRotation(45 - val);
}

void DirthCircleRenderer::setScale(float x, float y) {
  BlockRenderer::setScale(x, y);
  mChild->setScale(x, y);
}

void DirthCircleRenderer::setColor(const cocos2d::Color3B& color) {
  BlockRenderer::setColor(color);
  mChild->setColor(color);
}
