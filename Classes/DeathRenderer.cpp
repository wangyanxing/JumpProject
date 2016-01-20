//
//  ImageRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#include "DeathRenderer.h"
#include "SpriteUV.h"

USING_NS_CC;

#define SAW_IMAGE "images/saw.png"
#define SAW_IMAGE_ROT "images/saw_r.png"

DeathRenderer::DeathRenderer(GameObject *parent) : SimpleRenderer(parent) {
}

DeathRenderer::~DeathRenderer() {
}

GameRenderer *DeathRenderer::init(Parameter &param) {
  SimpleRenderer::init(param);
  normalizeUV();
  return this;
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

  if (mTextureName != SAW_IMAGE && mTextureName != SAW_IMAGE_ROT) {
    return;
  }

  auto w = getSize().width;
  auto h = getSize().height;

  if (w >= h) {
    if (mTextureName != SAW_IMAGE) {
      setTexture(SAW_IMAGE);
    }
    mSprite->getTexture()->setTexParameters({GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE});
    mSprite->setUVWidth(w / h);
  } else {
    if (mTextureName != SAW_IMAGE_ROT) {
      setTexture(SAW_IMAGE_ROT);
    }
    mSprite->getTexture()->setTexParameters({GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_REPEAT});
    mSprite->setUVHeight(h / w);
  }
  if (mUVFlipped) {
    mSprite->flipUVY();
  }
}
