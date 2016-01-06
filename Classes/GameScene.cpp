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
#include "ControlPad.h"

#if !EDITOR_MODE

USING_NS_CC;

GameScene::GameScene() : GameLayerContainer() {
}

GameScene::~GameScene() {
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

  getGame()->updateCamera(getCamera(), true);
  getGame()->showBeginCurtain();
}

void GameScene::onEnterTransitionDidFinish() {
  Layer::onEnterTransitionDidFinish();
}

bool GameScene::init() {
  GameLayerContainer::init();

  createControlPad();
  createMenuButtons();
  return true;
}

void GameScene::update(float dt) {
  GameLayerContainer::update(dt);

  // Update timer
  char time[10];
  sprintf(time, "%.1f", getGame()->mGameTimer);
  mTimerLabel->setString(time);
}

void GameScene::enterGame(const std::string &name, bool absPath) {
  if (absPath) {
    MapSerial::loadMap(name.c_str());
  } else {
    auto str = FileUtils::getInstance()->fullPathForFilename(name);
    MapSerial::loadMap(str.c_str());
  }
  getGame()->enableGame(false);
  mLeftButton->setVisible(true);
  mRightButton->setVisible(true);
  mJumpButton->setVisible(true);
}

void GameScene::onTouch(const cocos2d::Vec2 &pos) {
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
    getGame()->mMoveLeft = true;
    getGame()->mMoveRight = false;
  } else if (boundRight.containsPoint(pos)) {
    mRightButton->setOpacity(255);
    getGame()->mMoveLeft = false;
    getGame()->mMoveRight = true;
  }

  if (mCanJump && boundJump.containsPoint(pos)) {
    mJumpButton->setOpacity(255);
    getGame()->mJumpFlag = true;
    mCanJump = false;
  }
}

void GameScene::showHideMenu(bool force) {
  if (force || mTimerLabel->getNumberOfRunningActions() == 0) {
    auto down = Vec2(VisibleRect::center().x, VisibleRect::top().y - 50);
    auto up = Vec2(VisibleRect::center().x, VisibleRect::top().y + 50);
    bool out = mTimerLabel->getPositionY() > VisibleRect::top().y;
    mTimerLabel->runAction(MoveTo::create(0.3, out ? down : up));
  }
  if (force || mBackMenu->getNumberOfRunningActions() == 0) {
    auto down = Vec2(50, VisibleRect::top().y - 50);
    auto up = Vec2(50, VisibleRect::top().y + 50);
    bool out = mBackMenu->getPositionY() > VisibleRect::top().y;
    mBackMenu->runAction(MoveTo::create(0.3, out ? down : up));
  }
  if (force || mRestartMenu->getNumberOfRunningActions() == 0) {
    auto down = Vec2(VisibleRect::right().x - 50, VisibleRect::top().y - 50);
    auto up = Vec2(VisibleRect::right().x - 50, VisibleRect::top().y + 50);
    bool out = mRestartMenu->getPositionY() > VisibleRect::top().y;
    mRestartMenu->runAction(MoveTo::create(0.3, out ? down : up));
  }
}

void GameScene::onEndTouch(const cocos2d::Vec2 &pos) {
  if (pos.y > VisibleRect::top().y * 0.3 && pos.y < VisibleRect::top().y - 100) {
    showHideMenu();
  } else {
    if (pos.x < VisibleRect::center().x) {
      mLeftButton->setOpacity(CONTROL_BUTTON_OPACITY);
      mRightButton->setOpacity(CONTROL_BUTTON_OPACITY);
      getGame()->mMoveLeft = false;
      getGame()->mMoveRight = false;
    } else {
      mCanJump = true;
      mJumpButton->setOpacity(CONTROL_BUTTON_OPACITY);
    }
  }
}

void GameScene::enableGame(bool v) {
  getGame()->enableGame(v);
  mLeftButton->setVisible(v);
  mRightButton->setVisible(v);
  mJumpButton->setVisible(v);
}

void GameScene::preWinGame() {
  if (mBackMenu->getPositionY() < VisibleRect::top().y) {
    showHideMenu(true);
  }
}

void GameScene::onWinGame() {
  enableGame(false);
  toMainMenu();
}

void GameScene::toMainMenu() {
  enableGame(false);
  Director::getInstance()->replaceScene(LevelScene::getInstance());
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

  addChild(mTimerLabel, 1000);
  mTimerLabel->setPosition(VisibleRect::center().x, VisibleRect::top().y + 50);

  mBackMenu = MenuItemImage::create(
      "images/menu_icon.png",
      "images/menu_icon.png",
      [&](Ref *) {
          mBackMenu->runAction(
              Sequence::create(ScaleTo::create(0.1, 0.5),
                               ScaleTo::create(0.1, 0.4), NULL));
          showHideMenu(true);
          this->runAction(
              Sequence::create(DelayTime::create(0.2),
                               CallFunc::create([this] {
                                   toMainMenu();
                               }), NULL));
      });

  mRestartMenu = MenuItemImage::create(
      "images/restart_icon.png",
      "images/restart_icon.png",
      [&](Ref *) {
          mRestartMenu->runAction(
              Sequence::create(ScaleTo::create(0.1, 0.6),
                               ScaleTo::create(0.1, 0.5), NULL));
          showHideMenu(true);
          this->runAction(
              Sequence::create(DelayTime::create(0.2),
                               CallFunc::create(
                                   [this] {
                                       getGame()->enableGame(false);
                                       getGame()->enableGame(true);
                                   }), NULL));
      });

  mBackMenu->setColor(Color3B(200, 200, 200));
  mRestartMenu->setColor(Color3B(200, 200, 200));
  mTimerLabel->setColor(Color3B(200, 200, 200));

  mBackMenu->setPosition(50, VisibleRect::top().y + 50);
  mRestartMenu->setPosition(VisibleRect::right().x - 50, VisibleRect::top().y + 50);

  mBackMenu->setScale(0.4);
  mRestartMenu->setScale(0.5);

  auto menu = Menu::create(mBackMenu, mRestartMenu, NULL);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, 1000);
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
