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

  TTFConfig config("fonts/Montserra.ttf", 30);
  mFileNameLabel = Label::createWithTTF(config, "");
  mFileNameLabel->setScale(0.5f);
  mLayer->addChild(mFileNameLabel);

  mEditModeLebel = Label::createWithTTF(config, "");
  mEditModeLebel->setScale(0.5f);
  mLayer->addChild(mEditModeLebel);

  setFileName("Untitled");

  auto camera = Camera::create();
  camera->setCameraFlag(CameraFlag::USER1);
  mLayer->addChild(camera);
  mLayer->setCameraMask((unsigned short) CameraFlag::USER1);
}

void UILayer::setEditModeName(const std::string &name) {
  mEditModeLebel->setString(name);
  auto size = mEditModeLebel->getBoundingBox().size;
  auto w = mLayer->getBoundingBox().size.width;
  mEditModeLebel->setPosition(w - size.width / 2 - RIGHT_GAP,
                              VisibleRect::getFrameSize().height - size.height / 2 - RIGHT_GAP);
}

void UILayer::addMessage(const char *message) {
  TTFConfig config("fonts/Montserra.ttf", 32);
  auto label = Label::createWithTTF(config, message);
  label->setCameraMask((unsigned short) CameraFlag::USER1);

  mLayer->addChild(label);
  auto size = label->getBoundingBox().size;
  auto w = mLayer->getBoundingBox().size.width;
  label->setScale(0.5f);
  label->setPosition(w - size.width / 2 - RIGHT_GAP, VIS_RECT_HEIGHT + size.height / 2);
  label->runAction(Spawn::create(FadeOut::create(0.8),
                                 Sequence::create(MoveBy::create(1, Vec2(0, 50)),
                                                  CallFuncN::create([&](Node *n) {
                                                      n->removeFromParent();
                                                  }), nullptr), nullptr));
}

void UILayer::setFileName(const std::string &file) {
  std::string text = "File: ";
  mFileNameLabel->setString(text + file);
  auto size = mFileNameLabel->getBoundingBox().size;
  auto h = mLayer->getBoundingBox().size.height;
  mFileNameLabel->setPosition(LEFT_GAP + size.width / 2, h - size.height / 2 + EDT_UI_YBIAS);
}
