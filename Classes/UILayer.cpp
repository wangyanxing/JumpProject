//
//  UILayer.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/17/14.
//
//

#include "UILayer.h"
#include "VisibleRect.h"
#include "UIColorEditor.h"
#include "Defines.h"
#include "SpriteUV.h"
#include "GameUtils.h"

#include <regex>

USING_NS_CC;

#define LEFT_GAP 10
#define RIGHT_GAP 10

UILayer *UILayer::Layer = nullptr;

void UILayer::init(cocos2d::Node *parent) {
  Layer = this;

  auto vis = Director::getInstance()->getOpenGLView()->getVisibleRect();
  auto height = vis.size.height - VisibleRect::top().y;

  mLayer = Layer::create();
  mLayer->setContentSize(Size(VisibleRect::right().x, height));
  parent->addChild(mLayer);

  auto uiLayer = new UIColorEditor();
  uiLayer->init(mLayer);

  TTFConfig config("fonts/Montserra.ttf", 15);
  mFileNameLabel = Label::createWithTTF(config, "");
  mLayer->addChild(mFileNameLabel);

  setFileName("untitled");

  auto camera = Camera::create();
  camera->setCameraFlag(CameraFlag::USER1);
  mLayer->addChild(camera);
  mLayer->setCameraMask((unsigned short) CameraFlag::USER1);
}

void UILayer::addMessage(const char *message) {
  TTFConfig config("fonts/Montserra.ttf", 12);
  auto label = Label::createWithTTF(config, message);
  mLayer->addChild(label);
  auto size = label->getBoundingBox().size;
  auto w = mLayer->getBoundingBox().size.width;
  label->setPosition(w - size.width / 2 - RIGHT_GAP, size.height / 2);
  label->runAction(Spawn::create(FadeOut::create(0.8),
                                 Sequence::create(MoveBy::create(1, Vec2(0, 50)),
                                                  CallFuncN::create([&](Node *n) {
                                                      n->removeFromParent();
                                                  }), NULL), NULL));
}

void UILayer::setFileName(const char *file) {
  std::string rawFile = file;
  std::regex rx("(\\/|\\\\)(local|remote)(\\/|\\\\)(\\w)*.json$", std::regex_constants::icase);
  std::regex rxback("(\\/|\\\\)(\\w)*.json$", std::regex_constants::icase);
  std::smatch base_match;

  auto ret = std::regex_search(rawFile, base_match, rx);
  if (ret) {
    rawFile = base_match[0].str();
  } else {
    ret = std::regex_search(rawFile, base_match, rxback);
    if (ret) {
      rawFile = base_match[0].str();
    }
  }

  if (rawFile.empty()) {
    rawFile = file;
  }
  if (rawFile[0] == '\\' || rawFile[0] == '/') {
    rawFile.erase(rawFile.begin(), rawFile.begin() + 1);
  }

  std::string text = "File: ";
  mFileNameLabel->setString(text + rawFile);
  auto size = mFileNameLabel->getBoundingBox().size;
  auto h = mLayer->getBoundingBox().size.height;
  mFileNameLabel->setPosition(LEFT_GAP + size.width / 2, h - size.height / 2 + EDT_UI_YBIAS);
}
