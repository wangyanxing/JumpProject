//
//  GameScene.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/27/14.
//
//

#include "GameScene.h"
#include "MapSerial.h"
#include "VisibleRect.h"

#define TRANSPARENT_BUTTON 80

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
    
    auto listener1 = EventListenerTouchAllAtOnce::create();
    listener1->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* event){
        for(auto& t : touches) {
            onTouch(t->getLocation());
        }
    };
    listener1->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event*){
        for(auto& t : touches) {
            onTouch(t->getLocation());
        }
    };
    listener1->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event*){
        for(auto& t : touches) {
            onEndTouch(t->getLocation());
        }
    };
    listener1->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event*){
        for(auto& t : touches) {
            onEndTouch(t->getLocation());
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
    
    mGame = new GameLogic(this);
    
    //auto str = FileUtils::getInstance()->fullPathForFilename("maps/local/w1_MO_sawDancing.json");
    auto str = FileUtils::getInstance()->fullPathForFilename("maps/local/t_yw_shadowbug.json");
    MapSerial::loadMap(str.c_str());
    
    mGame->enableGame(true);
    
    createControlPad();
    
    return true;
}

void GameScene::update(float dt) {
    mGame->update(dt);
}

bool GameScene::onContactPreSolve(cocos2d::PhysicsContact& contact,
                                  cocos2d::PhysicsContactPreSolve& solve) {
    return mGame->onContactPreSolve(contact, solve);
}

void GameScene::onTouch(const cocos2d::Vec2& pos) {
    auto leftBound = mLeftButton->getBoundingBox();
    auto rightBound = mRightButton->getBoundingBox();
    auto jumpBound = mJumpButton->getBoundingBox();
    
    if(leftBound.containsPoint(pos)){
        mLeftButton->setOpacity(255);
        mGame->mMoveLeft = true;
        mGame->mMoveRight = false;
    }else if(rightBound.containsPoint(pos)){
        mRightButton->setOpacity(255);
        mGame->mMoveLeft = false;
        mGame->mMoveRight = true;
    }
    
    if(jumpBound.containsPoint(pos)){
        mJumpButton->setOpacity(255);
        mGame->jump();
    }
}

void GameScene::onEndTouch(const cocos2d::Vec2& pos) {
    
    if(pos.x < VisibleRect::center().x) {
        mLeftButton->setOpacity(TRANSPARENT_BUTTON);
        mRightButton->setOpacity(TRANSPARENT_BUTTON);
        mGame->mMoveLeft = false;
        mGame->mMoveRight = false;
    } else {
        mJumpButton->setOpacity(TRANSPARENT_BUTTON);
    }
}

void GameScene::createControlPad() {
    mLeftButton = Sprite::create("images/left_arrow.png");
    mRightButton = Sprite::create("images/right_arrow.png");
    mJumpButton = Sprite::create("images/jump_arrow_sel.png");
    
    mLeftButton->setPosition(Vec2(74,60));
    mRightButton->setPosition(Vec2(280,60));
    mJumpButton->setPosition(Vec2(820,60));
    
    mLeftButton->setScale(0.3);
    mRightButton->setScale(0.3);
    mJumpButton->setScale(0.3);
    
    mLeftButton->setOpacity(TRANSPARENT_BUTTON);
    mRightButton->setOpacity(TRANSPARENT_BUTTON);
    mJumpButton->setOpacity(TRANSPARENT_BUTTON);
    
    addChild(mLeftButton, 1000);
    addChild(mRightButton, 1000);
    addChild(mJumpButton, 1000);
}
