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
#include "LevelScene.h"
#include "EffectSprite.h"
#include "LogicManager.h"

#define TRANSPARENT_BUTTON 80

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
  srand((unsigned)time(nullptr));

  auto scene = Scene::createWithPhysics();
  scene->getPhysicsWorld()->setGravity(Vec2(0,0));
  GameLogic::PhysicsWorld = scene->getPhysicsWorld();

  auto game = GameScene::create();
  scene->addChild(game);

  return scene;
}

GameScene* GameScene::Scene = nullptr;

GameScene::~GameScene() {
  getScheduler()->unscheduleAllForTarget(&mPostUpdater);
}

void GameScene::showDieFullScreenAnim() {
#if USE_SHADER_LAYER
  enableShaderLayer = true;
  paramBlending = 2;
#endif
}

void GameScene::onEnter() {
  Layer::onEnter();

  getScheduler()->scheduleUpdate(this, -2, false);
  getScheduler()->scheduleUpdate(&mPostUpdater, 100, false);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactPreSolve = CC_CALLBACK_2(GameScene::onContactPreSolve, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

  auto touchListener = EventListenerTouchAllAtOnce::create();
  touchListener->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* event){
    for(auto& t : touches) {
      onTouch(t->getLocation());
    }
  };
  touchListener->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event*){
    for(auto& t : touches) {
      onTouch(t->getLocation());
    }
  };
  touchListener->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event*){
    for(auto& t : touches) {
      onEndTouch(t->getLocation());
    }
  };
  touchListener->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event*){
    for(auto& t : touches) {
      onEndTouch(t->getLocation());
    }
  };
  _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

  runAction(Sequence::create(DelayTime::create(0.4), CallFunc::create([this]{
    mGame->enableGame(true, true);
  }),  NULL));
}

void GameScene::onEnterTransitionDidFinish() {
#if USE_SHADER_LAYER
  ShaderLayer::onEnterTransitionDidFinish();
#else
  Layer::onEnterTransitionDidFinish();
#endif
}

bool GameScene::init() {
  Scene = this;
#if USE_SHADER_LAYER
  ShaderLayer::init("shaders/vignette.glsl");
  rendTexSprite->getGLProgramState()->setUniformVec2("darkness", Vec2(1,1));
#else
  Layer::init();
#endif

  mCamera = Camera::create();

  if (GameLogic::Game) {
    delete GameLogic::Game;
  }
  mGame = new GameLogic(this);
  mGame->mWinGameEvent = [this]{onWinGame();};

  mCamera->setCameraFlag(CameraFlag::USER2);
  mCamera->setDepth(-10);  
  addChild(mCamera);
  setCameraMask((unsigned short)CameraFlag::USER2);

  createControlPad();
  createMenuButtons();
  return true;
}

void GameScene::update(float dt) {
  // Update timer
  char time[10];
  sprintf(time, "%.1f",mGame->mGameTimer);
  mTimerLabel->setString(time);
  mGame->update(1.0f / 60.0f);
  mGame->updateCamera(mCamera);
}

void GameScene::enterGame(const std::string& name, bool absPath) {
  if(absPath) {
    MapSerial::loadMap(name.c_str());
  } else {
    auto str = FileUtils::getInstance()->fullPathForFilename(name);
    MapSerial::loadMap(str.c_str());
  }
  mGame->enableGame(false);
  mLeftButton->setVisible(true);
  mRightButton->setVisible(true);
  mJumpButton->setVisible(true);
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

  if(mCanJump && jumpBound.containsPoint(pos)){
    mJumpButton->setOpacity(255);
    mGame->mJumpFlag = true;
    mCanJump = false;
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
  if(pos.y > VisibleRect::top().y * 0.3 && pos.y < VisibleRect::top().y - 100) {
    showHideMenu();
  } else {
    if(pos.x < VisibleRect::center().x) {
      mLeftButton->setOpacity(TRANSPARENT_BUTTON);
      mRightButton->setOpacity(TRANSPARENT_BUTTON);
      mGame->mMoveLeft = false;
      mGame->mMoveRight = false;
    } else {
      mCanJump = true;
      mJumpButton->setOpacity(TRANSPARENT_BUTTON);
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
  enableGame(false);
  auto trans = TransitionFade::create(0.5, LevelScene::getInstance());
  Director::getInstance()->replaceScene(trans);
}

void GameScene::createMenuButtons() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  mTimerLabel = Label::createWithSystemFont("0.0",
                                            "Heiti TC",
                                            50,
                                            Size::ZERO,
                                            TextHAlignment::CENTER,
                                            TextVAlignment::CENTER);
#else
  TTFConfig config("fonts/Montserra.ttf",50);
  mTimerLabel = Label::createWithTTF(config,"0.0");
#endif

  addChild(mTimerLabel,1000);
  mTimerLabel->setPosition(VisibleRect::center().x, VisibleRect::top().y + 50);

  mBackMenu = MenuItemImage::create("images/menu_icon.png",
                                    "images/menu_icon.png",
                                    [&](Ref*) {
    mBackMenu->runAction(Sequence::create(ScaleTo::create(0.1, 0.5),
                                          ScaleTo::create(0.1, 0.4), NULL));
    showHideMenu(true);
    this->runAction(Sequence::create(DelayTime::create(0.2), CallFunc::create([this]{
      toMainMenu();
    }), NULL));
  });

  mRestartMenu = MenuItemImage::create("images/restart_icon.png",
                                       "images/restart_icon.png",
                                       [&](Ref*) {
    mRestartMenu->runAction(Sequence::create(ScaleTo::create(0.1, 0.6),
                                             ScaleTo::create(0.1, 0.5), NULL));
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

  MapSerial::loadControlConfig();
  auto config = ControlPad::controlPadConfig->getControlConfig();
  auto desc = config->mDescription;
  mLeftButton->setPosition(config->mLeftButtonPos);
  mRightButton->setPosition(config->mRightButtonPos);
  mJumpButton->setPosition(config->mJumpButtonPos);

  mLeftButton->setColor(Color3B(200,200,200));
  mRightButton->setColor(Color3B(200,200,200));
  mJumpButton->setColor(Color3B(200,200,200));

  float scale = config->mScale;
  mLeftButton->setScale(scale);
  mRightButton->setScale(scale);
  mJumpButton->setScale(scale);

  mLeftButton->setOpacity(TRANSPARENT_BUTTON);
  mRightButton->setOpacity(TRANSPARENT_BUTTON);
  mJumpButton->setOpacity(TRANSPARENT_BUTTON);

  addChild(mLeftButton, 1000);
  addChild(mRightButton, 1000);
  addChild(mJumpButton, 1000);
}
