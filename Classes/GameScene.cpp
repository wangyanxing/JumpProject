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
#include "LogicManager.h"
#include "ControlPad.h"
#include "GameLevel.h"
#include "GameObject.h"

#if !EDITOR_MODE

USING_NS_CC;

GameScene::GameScene() : GameLayerContainer() {
}

GameScene::~GameScene() {
  GameLevel::instance().release();
}

void GameScene::onEnter() {
  GameLayerContainer::onEnter();

  auto touchListener = EventListenerTouchAllAtOnce::create();
  touchListener->onTouchesBegan = [&](const std::vector<Touch *> &touches, Event *event) {
    for (auto &t : touches) {
      onTouch(t->getLocation());
    }
  };
  touchListener->onTouchesMoved = [&](const std::vector<Touch *> &touches, Event *) {
    for (auto &t : touches) {
      onTouch(t->getLocation());
    }
  };
  touchListener->onTouchesEnded = [&](const std::vector<Touch *> &touches, Event *) {
    for (auto &t : touches) {
      onEndTouch(t->getLocation());
    }
  };
  touchListener->onTouchesCancelled = [&](const std::vector<Touch *> &touches, Event *) {
    for (auto &t : touches) {
      onEndTouch(t->getLocation());
    }
  };
  _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

#if USE_REFACTOR
  GameLevel::instance().updateCamera(getCamera(), true);
#else
  getGame()->updateCamera(getCamera(), true);
  getGame()->showBeginCurtain();
#endif
}

void GameScene::onEnterTransitionDidFinish() {
  Layer::onEnterTransitionDidFinish();
}

bool GameScene::init() {
  GameLevel::instance().init(this);
  GameLayerContainer::init();

  createControlPad();
  createMenuButtons();
  return true;
}

void GameScene::update(float dt) {
  GameLayerContainer::update(dt);
}

void GameScene::enterGame(const std::string &name, bool absPath) {
#if USE_REFACTOR
  GameLevel::instance().load(absPath ? name : FileUtils::getInstance()->fullPathForFilename(name));
#else
  MapSerial::loadMap(absPath ? name : FileUtils::getInstance()->fullPathForFilename(name));
  getGame()->enableGame(false);
#endif

  mLeftButton->setVisible(true);
  mRightButton->setVisible(true);
  mJumpButton->setVisible(true);
}

void GameScene::onTouch(const cocos2d::Vec2 &pos) {
  if(mPauseUILayer->isVisible()) {
    return;
  }
  auto midPoint = mLeftButton->getPosition().getMidpoint(mRightButton->getPosition());
  auto jumpPoint = mJumpButton->getPosition();
  Rect boundLeft(midPoint.x - CONTROL_BUTTON_WIDTH,
                 midPoint.y - CONTROL_BUTTON_HEIGHT / 2,
                 CONTROL_BUTTON_WIDTH,
                 CONTROL_BUTTON_HEIGHT);
  Rect boundRight(midPoint.x,
                 midPoint.y - CONTROL_BUTTON_HEIGHT / 2,
                 CONTROL_BUTTON_WIDTH,
                 CONTROL_BUTTON_HEIGHT);
  Rect boundJump(jumpPoint.x - CONTROL_BUTTON_WIDTH / 2,
                 jumpPoint.y - CONTROL_BUTTON_HEIGHT / 2,
                 CONTROL_BUTTON_WIDTH,
                 CONTROL_BUTTON_HEIGHT);

  if (boundLeft.containsPoint(pos)) {
    mLeftButton->setOpacity(255);
#if USE_REFACTOR
    GameLevel::instance().getHero()->runCommand(COMMAND_INPUT, {{PARAM_INPUT, Any(INPUT_LEFT)}});
#else
    getGame()->mMoveLeft = true;
    getGame()->mMoveRight = false;
#endif
  } else if (boundRight.containsPoint(pos)) {
    mRightButton->setOpacity(255);
#if USE_REFACTOR
    GameLevel::instance().getHero()->runCommand(COMMAND_INPUT, {{PARAM_INPUT, Any(INPUT_RIGHT)}});
#else
    getGame()->mMoveLeft = false;
    getGame()->mMoveRight = true;
#endif
  }

  if (mCanJump && boundJump.containsPoint(pos)) {
    mJumpButton->setOpacity(255);
#if USE_REFACTOR
    GameLevel::instance().getHero()->runCommand(COMMAND_INPUT, {{PARAM_INPUT, Any(INPUT_JUMP)}});
#else
    getGame()->mJumpFlag = true;
#endif
    mCanJump = false;
  }
}

void GameScene::onEndTouch(const cocos2d::Vec2 &pos) {
  if (pos.y < VisibleRect::top().y * 0.3 || pos.y > VisibleRect::top().y - 100) {
    if (pos.x < VisibleRect::center().x) {
      mLeftButton->setOpacity(CONTROL_BUTTON_OPACITY);
      mRightButton->setOpacity(CONTROL_BUTTON_OPACITY);
#if !USE_REFACTOR
      getGame()->mMoveLeft = false;
      getGame()->mMoveRight = false;
#endif
    } else {
      mCanJump = true;
      mJumpButton->setOpacity(CONTROL_BUTTON_OPACITY);
    }
  }
}

void GameScene::enableGame(bool v) {
#if USE_REFACTOR
  GameLevel::instance().enableGame(v);
#else
  getGame()->enableGame(v);
#endif
  mLeftButton->setVisible(v);
  mRightButton->setVisible(v);
  mJumpButton->setVisible(v);
}

void GameScene::onWinGame() {
  enableGame(false);
  toMainMenu();
}

void GameScene::toMainMenu() {
  enableGame(false);
  Director::getInstance()->replaceScene(LevelScene::getInstance());
}

void GameScene::showPauseUI(bool val) {
  Node *buttons = mPauseUILayer->getChildByName("Buttons");
  CC_ASSERT(buttons);

  if (val) {
    mPauseUILayer->setVisible(true);
    mPauseUILayer->runAction(Sequence::create(FadeTo::create(0.3f, 200), nullptr));
    buttons->runAction(Sequence::create(FadeTo::create(0.3f, 255), nullptr));
  } else {
    buttons->runAction(FadeTo::create(0.3f, 0));
    mPauseUILayer->runAction(Sequence::create(FadeTo::create(0.3f, 0),
                                              CallFunc::create([this](){
        mPauseUILayer->setVisible(false);
    }), nullptr));
  }
}

void GameScene::createMenuButtons() {
  auto pauseButton = MenuItemImage::create(
      "images/button_pause.png",
      "images/button_pause.png",
      [&](Ref *) {
#if USE_REFACTOR
        GameLevel::instance().pause();
#else
        getGame()->pauseGame(true);
#endif
        showPauseUI(true);
      });

  pauseButton->setColor(Color3B(200, 200, 200));
  pauseButton->setPosition(VisibleRect::right().x - 50, VisibleRect::top().y - 50);
  pauseButton->setScale(0.7f);

  auto menu = Menu::create(pauseButton, nullptr);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, ZORDER_GAME_CONTROLPAD);

  static float buttonScale = 0.7f;
  mPauseUILayer = LayerColor::create(Color4B(30, 30, 30, 0));
  mPauseUILayer->setVisible(false);
  addChild(mPauseUILayer, ZORDER_GAME_PAUSELAYER);
  auto resumeButton = MenuItemImage::create(
    "images/button_resume.png",
    "images/button_resume.png",
    [&](Ref *ref) {
      dynamic_cast<cocos2d::Node*>(ref)->runAction(Sequence::create(
                                         ScaleTo::create(0.1f, buttonScale + 0.05f),
                                         ScaleTo::create(0.1f, buttonScale),
                                         CallFunc::create([&](){
          showPauseUI(false);
#if USE_REFACTOR
          GameLevel::instance().resume();
#else
          getGame()->pauseGame(false);
#endif
      }), nullptr));
    });
  auto replayButton = MenuItemImage::create(
    "images/button_replay.png",
    "images/button_replay.png",
    [&](Ref *ref) {
      dynamic_cast<cocos2d::Node*>(ref)->runAction(Sequence::create(
                                          ScaleTo::create(0.1f, buttonScale + 0.05f),
                                          ScaleTo::create(0.1f, buttonScale),
                                          CallFunc::create([&](){
          showPauseUI(false);
#if USE_REFACTOR
        GameLevel::instance().enableGame(false);
        GameLevel::instance().enableGame(true);
#else
        getGame()->restartGame();
#endif
      }), nullptr));
    });
  auto returnButton = MenuItemImage::create(
    "images/button_main_menu.png",
    "images/button_main_menu.png",
    [&](Ref *ref) {
      dynamic_cast<cocos2d::Node*>(ref)->runAction(Sequence::create(
                                          ScaleTo::create(0.1f, buttonScale + 0.05f),
                                          ScaleTo::create(0.1f, buttonScale),
                                          CallFunc::create([&](){
          toMainMenu();
      }), nullptr));
    });

  resumeButton->setScale(buttonScale);
  replayButton->setScale(buttonScale);
  returnButton->setScale(buttonScale);

  resumeButton->setPosition(VisibleRect::right().x / 4 - 50, VisibleRect::top().y / 2);
  replayButton->setPosition(VisibleRect::right().x / 2, VisibleRect::top().y / 2);
  returnButton->setPosition(VisibleRect::right().x / 4 * 3 + 50, VisibleRect::top().y / 2);

  auto pauseMenu = Menu::create(resumeButton, replayButton, returnButton, nullptr);
  pauseMenu->setPosition(Vec2::ZERO);
  pauseMenu->setOpacity(0);
  mPauseUILayer->addChild(pauseMenu, 0, "Buttons");
}

void GameScene::createControlPad() {
  mLeftButton = Sprite::create("images/button_left.png");
  mRightButton = Sprite::create("images/button_right.png");
  mJumpButton = Sprite::create("images/button_jump.png");

  MapSerial::loadControlConfig();
  auto config = ControlPad::controlPadConfig->getControlConfig();
  auto desc = config->mDescription;
  mLeftButton->setPosition(config->mLeftButtonPos);
  mRightButton->setPosition(config->mRightButtonPos);
  mJumpButton->setPosition(config->mJumpButtonPos);

  mLeftButton->setColor(CONTROL_BUTTON_COLOR);
  mRightButton->setColor(CONTROL_BUTTON_COLOR);
  mJumpButton->setColor(CONTROL_BUTTON_COLOR);

  float scale = config->mScale;
  mLeftButton->setScale(scale);
  mRightButton->setScale(scale);
  mJumpButton->setScale(scale);

  mLeftButton->setOpacity(CONTROL_BUTTON_OPACITY);
  mRightButton->setOpacity(CONTROL_BUTTON_OPACITY);
  mJumpButton->setOpacity(CONTROL_BUTTON_OPACITY);

  addChild(mLeftButton, ZORDER_GAME_CONTROLPAD);
  addChild(mRightButton, ZORDER_GAME_CONTROLPAD);
  addChild(mJumpButton, ZORDER_GAME_CONTROLPAD);
}

#endif
