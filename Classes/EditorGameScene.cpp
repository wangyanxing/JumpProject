//
//  EditorGameScene.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#include "EditorGameScene.h"
#include "GameLevel.h"
#include "GameInputs.h"
#include "GameObject.h"
#include "Parameter.h"

#if USE_REFACTOR

USING_NS_CC;

EditorGameScene::EditorGameScene() {
}

EditorGameScene::~EditorGameScene() {
}

bool EditorGameScene::init() {
  GameLayerContainer::init();
  GameLevel::instance().setGameLayer(this);

  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(EditorGameScene::keyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(EditorGameScene::keyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  // Test.
  GameLevel::instance().load("maps/local/test_refactor1.json");
  return true;
}

void EditorGameScene::onEnter() {
  GameLayerContainer::onEnter();
}

void EditorGameScene::update(float dt) {
  processInput();

  GameLayerContainer::update(dt);
}

void EditorGameScene::postUpdate(float dt) {
  GameLayerContainer::postUpdate(dt);
}

void EditorGameScene::processInput() {
  auto &gameInputs = GameInputs::instance();
  auto hero = GameLevel::instance().getHero();
  Parameter param;

  if (gameInputs.isPressing(EventKeyboard::KeyCode::KEY_A)) {
    param.set(PARAM_INPUT, INPUT_LEFT);
    hero->runCommand(COMMAND_INPUT, param);
  } else if (gameInputs.isPressing(EventKeyboard::KeyCode::KEY_D)) {
    param.set(PARAM_INPUT, INPUT_RIGHT);
    hero->runCommand(COMMAND_INPUT, param);
  } else if (gameInputs.isPressing(EventKeyboard::KeyCode::KEY_SPACE)) {
    param.set(PARAM_INPUT, INPUT_JUMP);
    hero->runCommand(COMMAND_INPUT, param);
  }
}

void EditorGameScene::clean() {
  GameLayerContainer::clean();
  GameLevel::instance().unload();
}

void EditorGameScene::onWinGame() {
  
}

void EditorGameScene::keyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
  GameInputs::instance().keyPressed(keyCode, event);
}

void EditorGameScene::keyReleased(EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
  GameInputs::instance().keyReleased(keyCode, event);
}

#endif
