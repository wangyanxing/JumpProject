//
//  DeathRotatorRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#include "DeathRotatorRenderer.h"
#include "SpriteUV.h"
#include "GameUtils.h"
#include "JsonFormat.h"

USING_NS_CC;

DeathRotatorRenderer::DeathRotatorRenderer(GameObject *parent) : SimpleRenderer(parent) {
}

DeathRotatorRenderer::~DeathRotatorRenderer() {
#if EDITOR_MODE
  mChild->removeFromParent();
#endif
}

GameRenderer *DeathRotatorRenderer::init(Parameter& param) {
  mChild = GameUtils::createRect(param.get<Size>(PARAM_SIZE), param.get<Vec2>(PARAM_POS));
  return SimpleRenderer::init(param);
}

void DeathRotatorRenderer::load(JsonValueT &json) {
  SimpleRenderer::load(json);

  if (json.HasMember(RENDERER_ROT_SPEED)) {
    mRotationSpeed = json[RENDERER_ROT_SPEED].GetInt();
  }
}

void DeathRotatorRenderer::clone(GameRenderer *renderer) {
  mChild = GameUtils::createRect(renderer->getOriginalSize(), renderer->getOriginalPosition());

  SimpleRenderer::clone(renderer);
  DeathRotatorRenderer *other = dynamic_cast<DeathRotatorRenderer*>(renderer);
  CC_ASSERT(other);
  mRotationSpeed = other->mRotationSpeed;
}

void DeathRotatorRenderer::save(JsWriter &writer) {
  SimpleRenderer::save(writer);

  if (mRotationSpeed != DEFAULT_DEATH_ROTATOR_SPEED) {
    writer.String(RENDERER_ROT_SPEED);
    writer.Int(mRotationSpeed);
  }
}

void DeathRotatorRenderer::addToParent(cocos2d::Node* parent, int zorder) {
  GameRenderer::addToParent(parent, zorder);
  parent->addChild(mChild, zorder);
}

void DeathRotatorRenderer::setSize(const cocos2d::Size &size) {
  auto contentSize = getContentSize();
  float length = std::max(size.width, size.height);
  getNode()->setScale(length / contentSize.width, length / contentSize.height);
}

void DeathRotatorRenderer::setPosition(const cocos2d::Vec2& pos) {
  GameRenderer::setPosition(pos);
  mChild->setPosition(pos);
}

void DeathRotatorRenderer::setRotation(float val) {
  GameRenderer::setRotation(val);
  mChild->setRotation(45 - val);
}

void DeathRotatorRenderer::setScale(float x, float y) {
  GameRenderer::setScale(x, y);
  mChild->setScale(x, y);
}

void DeathRotatorRenderer::setColor(const cocos2d::Color3B& color) {
  GameRenderer::setColor(color);
  mChild->setColor(color);
}

void DeathRotatorRenderer::setRotationSpeed(int speed) {
  mRotationSpeed = speed;
}

int DeathRotatorRenderer::getRotationSpeed() {
  return mRotationSpeed;
}

void DeathRotatorRenderer::update(float dt) {
  SimpleRenderer::update(dt);

  if (mRotationSpeed > 0) {
    auto rotation = getRotation();
    rotation += mRotationSpeed * dt;
    if (rotation > 360) {
      rotation -= 360;
    }
    setRotation(rotation);
  }
}
