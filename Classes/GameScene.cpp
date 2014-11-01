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
#include "Blocks.h"
#include "Shadows.h"
#include "LevelSelector.h"
#include "LevelScene.h"

#define TRANSPARENT_BUTTON 80

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Vec2(0,-1000));
    //scene->retain();

    auto game = GameScene::create();
    scene->addChild(game);

    return scene;
}

GameScene* GameScene::Scene = nullptr;

GameScene::~GameScene() {

    getScheduler()->unscheduleAllForTarget(&mPostUpdater);
    
    mTimerLabel->removeFromParent();
    mBackMenu->removeFromParent();
    mRestartMenu->removeFromParent();
    
    delete mGame;
    mGame = nullptr;
}

void GameScene::onEnter() {
    Layer::onEnter();
    
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
}

bool GameScene::init() {
    Scene = this;
    
    Layer::init();
    
    mGame = new GameLogic(this);
    mGame->mWinGameEvent = [this]{onWinGame();};
    createControlPad();
    createMenuButtons();
    
    //loadChooseLevel("maps/remote/w1_chooselevel.json");
    /*
#if 1
    loadChooseLevel("maps/remote/w1_chooselevel.json");
#else
    auto str = FileUtils::getInstance()->fullPathForFilename("maps/local/t_yw_shadow.json");
    MapSerial::loadMap(str.c_str());
    enableGame(true);
#endif
     */
    
    return true;
}

void GameScene::loadChooseLevel(const std::string& name) {

    auto str = FileUtils::getInstance()->fullPathForFilename("maps/remote/w1_chooselevel.json");
    MapSerial::loadMap(str.c_str());
    
    for(auto l : mLevelLabels) {
        l->removeFromParent();
    }
    mLevelLabels.clear();
    mCurrentLevels.clear();
    
    mChoosingLevel = true;
    
    enableGame(true);
    mGame->mHero->getSprite()->setVisible(false);
    mLeftButton->setVisible(false);
    mRightButton->setVisible(false);
    mJumpButton->setVisible(false);
    
    for(auto b : mGame->mBlocks) {
        auto block = b.second;
        if(block->mUserData == "lightPath") {
            mLightPath = block;
            block->getSprite()->setVisible(false);
        }
        else if(!block->mUserData.empty()) {
            int id = atoi(block->mUserData.c_str());
            mCurrentLevels[id] = block;
        }
    }

    size_t size = mCurrentLevels.size();
    mLevelLabels.resize(size);
    for(size_t i = 0; i < size; ++i){
        TTFConfig config("fonts/Montserra.ttf", 30);
        char t[5];
        sprintf(t, "%d", (int)i+1);
        mLevelLabels[i] = Label::createWithTTF(config,t,TextHAlignment::CENTER);
        addChild(mLevelLabels[i],1000);
        mLevelLabels[i]->setPosition(mCurrentLevels[(int)i+1]->getPosition());
        mLevelLabels[i]->setColor(Color3B(100,100,100));
    }
}

void GameScene::updateChoosingLevel(float dt) {
    if(mLightPath) {
        mGame->mShadows->mLightPos = mLightPath->getPosition();
    }
}

void GameScene::update(float dt) {
    if(mChoosingLevel)
        updateChoosingLevel(dt);
    else {
        // update timer
        char time[10];
        sprintf(time, "%.1f",mGame->mGameTimer);
        mTimerLabel->setString(time);
    }
    
    mGame->update(dt);
}

void GameScene::enterGame(const std::string& name) {
    for(auto l : mLevelLabels) {
        l->removeFromParent();
    }
    mLevelLabels.clear();
    mCurrentLevels.clear();
    mChoosingLevel = false;

    auto str = FileUtils::getInstance()->fullPathForFilename(name);
    MapSerial::loadMap(str.c_str());
    
    enableGame(true);
}

bool GameScene::onContactPreSolve(cocos2d::PhysicsContact& contact,
                                  cocos2d::PhysicsContactPreSolve& solve) {
    return mGame->onContactPreSolve(contact, solve);
}

void GameScene::onTouch(const cocos2d::Vec2& pos) {
    if(mChoosingLevel)
        return;
    
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

void GameScene::showHideMenu(bool force) {
    if(force || mTimerLabel->getNumberOfRunningActions() == 0) {
        auto down = Vec2(VisibleRect::center().x, VisibleRect::top().y - 50);
        auto up = Vec2(VisibleRect::center().x, VisibleRect::top().y + 50);
        bool out = mTimerLabel->getPositionY() > VisibleRect::top().y;
        
        mTimerLabel->runAction(MoveTo::create(0.3, out ? down : up));
    }
    if(force || mBackMenu->getNumberOfRunningActions() == 0) {
        auto down = Vec2(50, VisibleRect::top().y - 50);
        auto up = Vec2(50, VisibleRect::top().y + 50);
        bool out = mBackMenu->getPositionY() > VisibleRect::top().y;
        mBackMenu->runAction(MoveTo::create(0.3, out ? down : up));
    }
    if(force || mRestartMenu->getNumberOfRunningActions() == 0) {
        auto down = Vec2(VisibleRect::right().x - 50, VisibleRect::top().y - 50);
        auto up = Vec2(VisibleRect::right().x - 50, VisibleRect::top().y + 50);
        bool out = mRestartMenu->getPositionY() > VisibleRect::top().y;
        mRestartMenu->runAction(MoveTo::create(0.3, out ? down : up));
    }
}

void GameScene::onEndTouch(const cocos2d::Vec2& pos) {
    
    if(mChoosingLevel){
        for (auto l : mCurrentLevels) {
            auto b = l.second->getSprite()->getBoundingBox();
            if(b.containsPoint(pos)) {
                int id = l.first;
                char levelName[256];
                sprintf(levelName, "maps/remote/w1_%03d.json",id);
                enterGame(levelName);
                return;
            }
        }
    } else {
        
        if(pos.y > VisibleRect::top().y * 0.3 && pos.y < VisibleRect::top().y - 100) {
            showHideMenu();
        } else {
            if(pos.x < VisibleRect::center().x) {
                mLeftButton->setOpacity(TRANSPARENT_BUTTON);
                mRightButton->setOpacity(TRANSPARENT_BUTTON);
                mGame->mMoveLeft = false;
                mGame->mMoveRight = false;
            } else {
                mJumpButton->setOpacity(TRANSPARENT_BUTTON);
            }
        }
    }
}

void GameScene::enableGame(bool v) {
    mGame->enableGame(v);
    mLeftButton->setVisible(v);
    mRightButton->setVisible(v);
    mJumpButton->setVisible(v);
}

void GameScene::onWinGame() {
    enableGame(false);
    if(mTimerLabel->getPositionY() < VisibleRect::top().y) {
        showHideMenu(true);
    }
    this->runAction(Sequence::create(DelayTime::create(0.3), CallFunc::create([this]{
        toMainMenu();
    }), NULL));
}

void GameScene::toMainMenu() {
    //loadChooseLevel("maps/remote/w1_chooselevel.json");
    enableGame(false);
    auto trans = TransitionFade::create(0.5, LevelScene::getInstance());
    Director::getInstance()->replaceScene(trans);
}

void GameScene::createMenuButtons() {

    mTimerLabel = Label::createWithSystemFont("0.0", "Heiti TC", 50,
                                              Size::ZERO, TextHAlignment::CENTER,TextVAlignment::CENTER);
    addChild(mTimerLabel,1000);
    mTimerLabel->setPosition(VisibleRect::center().x, VisibleRect::top().y + 50);
    
    mBackMenu = MenuItemImage::create("images/menu_icon.png", "images/menu_icon.png", [&](Ref*) {
        mBackMenu->runAction(Sequence::create(ScaleTo::create(0.1, 0.5),ScaleTo::create(0.1, 0.4), NULL));
        
        showHideMenu(true);
        this->runAction(Sequence::create(DelayTime::create(0.2), CallFunc::create([this]{
            toMainMenu();
        }), NULL));
    });

    mRestartMenu = MenuItemImage::create("images/restart_icon.png", "images/restart_icon.png", [&](Ref*) {
        mRestartMenu->runAction(Sequence::create(ScaleTo::create(0.1, 0.6),ScaleTo::create(0.1, 0.5), NULL));
        
        showHideMenu(true);
        this->runAction(Sequence::create(DelayTime::create(0.2), CallFunc::create([this]{
            mGame->enableGame(false);
            mGame->enableGame(true);
        }), NULL));
    });
    
    mBackMenu->setColor(Color3B(200,200,200));
    mRestartMenu->setColor(Color3B(200,200,200));
    mTimerLabel->setColor(Color3B(200,200,200));
    
    mBackMenu->setPosition(50, VisibleRect::top().y + 50);
    mRestartMenu->setPosition(VisibleRect::right().x - 50, VisibleRect::top().y + 50);
    
    mBackMenu->setScale(0.4);
    mRestartMenu->setScale(0.5);

    auto menu = Menu::create(mBackMenu,mRestartMenu, NULL);
    menu->setPosition(Vec2::ZERO);
    addChild(menu,1000);
}

void GameScene::createControlPad() {
    mLeftButton = Sprite::create("images/left_arrow.png");
    mRightButton = Sprite::create("images/right_arrow.png");
    mJumpButton = Sprite::create("images/jump_arrow_sel.png");
    
    mLeftButton->setPosition(Vec2(74,60));
    mRightButton->setPosition(Vec2(280,60));
    mJumpButton->setPosition(Vec2(820,60));
    
    mLeftButton->setColor(Color3B(200,200,200));
    mRightButton->setColor(Color3B(200,200,200));
    mJumpButton->setColor(Color3B(200,200,200));
    
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
