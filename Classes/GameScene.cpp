//
//  GameScene.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/27/14.
//
//

#include "GameScene.h"
#include "MapSerial.h"

USING_NS_CC;

GameScene* GameScene::Scene = nullptr;

GameScene::~GameScene() {
    delete mGame;
    mGame = nullptr;
}

bool GameScene::init() {
    Scene = this;
    
    Layer::init();
    
    getScheduler()->scheduleUpdate(this, -2, false);
    getScheduler()->scheduleUpdate(&mPostUpdater, 100, false);
    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactPreSolve = CC_CALLBACK_2(GameScene::onContactPreSolve, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    mGame = new GameLogic(this);
    
    auto str = FileUtils::getInstance()->fullPathForFilename("maps/local/t_yw_rotator.json");
    MapSerial::loadMap(str.c_str());
    
    mGame->enableGame(true);
    
    return true;
}

void GameScene::update(float dt) {
    mGame->update(dt);
}

bool GameScene::onContactPreSolve(cocos2d::PhysicsContact& contact,
                                  cocos2d::PhysicsContactPreSolve& solve) {
    return mGame->onContactPreSolve(contact, solve);
}
