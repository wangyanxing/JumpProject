//
//  LevelScene.cpp
//  JumpEdt
//
//

#include "LevelScene.h"
#include "LevelLayer.h"
#include "LevelSelector.h"

USING_NS_CC;

#include "SimpleAudioEngine.h"
using namespace CocosDenshion; 

#define LAYER_NUMBER 3

LevelScene* LevelScene::instance;

LevelScene *LevelScene::getInstance() {
    if(instance != NULL) {
        return instance;
    }
    return  NULL;
}

bool LevelScene::init()  {
    if (!Scene::init()) {
		return false;
	}
    instance = this;
    
    Size size = Director::getInstance()->getWinSize();

    LevelLayer* scrollView = LevelLayer::create();
    this->addChild(scrollView);
    {
        auto sel = LevelSelLayer::create();
        sel->initWorld(TITLE);
        sel->setTag(0);
        scrollView->addNode(sel);
    }
    {
        auto sel = LevelSelLayer::create();
        sel->initWorld(WORLD_1);
        sel->setTag(1);
        scrollView->addNode(sel);
    }
    {
        auto sel = LevelSelLayer::create();
        sel->initWorld(WORLD_2);
        sel->setTag(1);
        scrollView->addNode(sel);
    }
      
    return true;
}  