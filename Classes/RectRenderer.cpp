//
//  RectRenderer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/7/16.
//
//

#include "RectRenderer.h"
#include "Blocks.h"
#include "LogicManager.h"
#include "GameUtils.h"

USING_NS_CC;

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
