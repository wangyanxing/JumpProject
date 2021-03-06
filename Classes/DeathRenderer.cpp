//
//  ImageRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#include "DeathRenderer.h"
#include "SpriteUV.h"
#include "GameConfig.h"
#include "JsonFormat.h"

USING_NS_CC;

DeathRenderer::DeathRenderer(GameObject *parent) : SimpleRenderer(parent) {
}

DeathRenderer::~DeathRenderer() {
}

GameRenderer *DeathRenderer::init(Parameter &param) {
  SimpleRenderer::init(param);
  normalizeUV();
  return this;
}

void DeathRenderer::load(JsonValueT &json) {
  SimpleRenderer::load(json);

  if (json.HasMember(RENDERER_FLIP_UV)) {
    setFlipUV(json[RENDERER_FLIP_UV].GetBool());
  }
  if (json.HasMember(RENDERER_TEXTURE)) {
    setTexture(json[RENDERER_TEXTURE].GetString());
  }
}

void DeathRenderer::clone(GameRenderer *renderer) {
  SimpleRenderer::clone(renderer);
  DeathRenderer *other = dynamic_cast<DeathRenderer*>(renderer);
  CC_ASSERT(other);
  setFlipUV(other->mUVFlipped);
  normalizeUV();
}

void DeathRenderer::save(JsWriter &writer) {
  SimpleRenderer::save(writer);

  if (mUVFlipped) {
    writer.String(RENDERER_FLIP_UV);
    writer.Bool(mUVFlipped);
  }
  if (!mTextureName.empty()) {
    writer.String(RENDERER_TEXTURE);
    writer.String(mTextureName);
  }
}

void DeathRenderer::setSize(const cocos2d::Size &size) {
  GameRenderer::setSize(size);
  normalizeUV();
}

void DeathRenderer::setFlipUV(bool val) {
  mUVFlipped = val;
}

bool DeathRenderer::isFlipUV() {
  return mUVFlipped;
}

void DeathRenderer::normalizeUV() {
  mSprite->resetUV();
  
  auto sawImage = GameConfig::instance().SawImage;
  auto sawImageRot = GameConfig::instance().SawImageRotation;

  if (mTextureName != sawImage && mTextureName != sawImageRot) {
    return;
  }

  auto w = getSize().width, h = getSize().height;
  if (w >= h) {
    if (mTextureName != sawImage) {
      setTexture(sawImage);
    }
    mSprite->getTexture()->setTexParameters({GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE});
    mSprite->setUVWidth(w / h);
  } else {
    if (mTextureName != sawImageRot) {
      setTexture(sawImageRot);
    }
    mSprite->getTexture()->setTexParameters({GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_REPEAT});
    mSprite->setUVHeight(h / w);
  }
  if (mUVFlipped) {
    mSprite->flipUVY();
  }
}
