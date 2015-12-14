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

#include <regex>
#include <iostream>

USING_NS_CC;

#define LEFT_GAP 10
#define RIGHT_GAP 10

UILayer* UILayer::Layer = nullptr;

// on "init" you need to initialize your instance
void UILayer::init(cocos2d::Node* parent) {

  Layer = this;

  auto vis = Director::getInstance()->getOpenGLView()->getVisibleRect();
  auto height = vis.size.height - VisibleRect::top().y;

  mLayer = LayerColor::create(Color4B(0x1E,0xB5,0xC7,0xFF), VisibleRect::right().x, height);
  parent->addChild(mLayer, 1500);

  mLayer->setPositionY(VisibleRect::top().y);

  TTFConfig config("fonts/Montserra.ttf",15);
  mFileNameLabel = Label::createWithTTF(config,"");
  mFileNameLabel->setPosition(0,30);
  mLayer->addChild(mFileNameLabel);

  auto uiLayer = new UIColorEditor();
  uiLayer->init(mLayer);

  setFileName("untitled");
}

void UILayer::addMessage(const char* message) {
  TTFConfig config("fonts/Montserra.ttf",12);
  auto label = Label::createWithTTF(config,message);
  mLayer->addChild(label);
  auto size = label->getBoundingBox().size;
  auto w = mLayer->getBoundingBox().size.width;
  label->setPosition(w - size.width/2 - RIGHT_GAP, size.height/2);

  label->runAction(Spawn::create(FadeOut::create(0.8),
                                 Sequence::create(MoveBy::create(1, Vec2(0,50)),
                                                  CallFuncN::create([&](Node* n){
                                   n->removeFromParent();
                                 })
                                                  , NULL), NULL));
}

void UILayer::setFileName(const char* file) {
  std::string rawFile = file;

  std::regex rx("(\\/|\\\\)(local|remote)(\\/|\\\\)(\\w)*.json$", std::regex_constants::icase);
  std::regex rxback("(\\/|\\\\)(\\w)*.json$", std::regex_constants::icase);
  std::smatch base_match;

  auto ret = std::regex_search(rawFile, base_match, rx );
  if(ret) {
    rawFile = base_match[0].str();
  } else {
    ret = std::regex_search(rawFile, base_match, rxback );
    if(ret) {
      rawFile = base_match[0].str();
    }
  }

  if(rawFile.empty()) rawFile = file;
  if(rawFile[0] == '\\' || rawFile[0] == '/')
    rawFile.erase(rawFile.begin(), rawFile.begin()+1);

  std::string text = "File: ";
  mFileNameLabel->setString(text + rawFile);
  auto size = mFileNameLabel->getBoundingBox().size;
  auto h = mLayer->getBoundingBox().size.height;
  mFileNameLabel->setPosition(LEFT_GAP + size.width/2, h - size.height/2);
}

