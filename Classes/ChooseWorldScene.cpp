//
//  ChooseWorldScene.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/5/16.
//
//

#include "ChooseWorldScene.h"
#include "cocos-ext.h"
#include "VisibleRect.h"
#include "GameLevel.h"

#if !EDITOR_MODE

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;

ChooseWorldScene *ChooseWorldScene::instance = nullptr;

ChooseWorldScene *ChooseWorldScene::getInstance() {
  return instance;
}

bool ChooseWorldScene::init() {
  instance = this;
  GameLayerContainer::init();

  // Load UI game.
  auto uiFile = FileUtils::getInstance()->getWritablePath() + "choose_world.json";
  GameLevel::instance().load(uiFile.c_str());
  return true;
}

void ChooseWorldScene::update(float dt) {
  GameLayerContainer::update(dt);
}

void ChooseWorldScene::touchEvent(Ref *pSender, Widget::TouchEventType type) {
}

#endif
