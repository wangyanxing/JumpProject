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
#include "GameRenderer.h"
#include "PhysicsManager.h"
#include "Parameter.h"
#include "VisibleRect.h"
#include "GameConfig.h"
#include "PathLib.h"
#include "EditorManager.h"
#include "UILayer.h"

#if USE_REFACTOR

USING_NS_CC;

EditorGameScene::EditorGameScene() {
}

EditorGameScene::~EditorGameScene() {
}

bool EditorGameScene::init() {
  GameLevel::instance().init(this);
  GameLayerContainer::init();

  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(EditorGameScene::keyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(EditorGameScene::keyReleased, this);
  getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto mouseListener = EventListenerMouse::create();
  mouseListener->onMouseDown = [](EventMouse* event) {
    GameInputs::instance().setPressingMouse(true);
    EditorManager::instance().onMouseDown(GameInputs::instance().convertMouseEvent(event));
  };
  mouseListener->onMouseUp = [](EventMouse* event) {
    GameInputs::instance().setPressingMouse(false);
    EditorManager::instance().onMouseUp(GameInputs::instance().convertMouseEvent(event));
  };
  mouseListener->onMouseMove = [](EventMouse* event) {
    EditorManager::instance().onMouseMove(GameInputs::instance().convertMouseEvent(event));
    GameInputs::instance().setLastMousePosition({event->getLocationInView()});
  };
  getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);

  GameInputs::instance().addKeyboardEvent(EventKeyboard::KeyCode::KEY_SPACE,
                                          [&](GameInputs::KeyCode key) {
      GameLevel::instance().getHero()->runCommand(COMMAND_INPUT, {{PARAM_INPUT, Any(INPUT_JUMP)}});
  });

  EditorManager::instance().init();
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

  if (GameLevel::instance().isGameEnabled()) {
    auto hero = GameLevel::instance().getHero();
    if (gameInputs.isPressing(EventKeyboard::KeyCode::KEY_A)) {
      hero->runCommand(COMMAND_INPUT, {{PARAM_INPUT, Any(INPUT_LEFT)}});
    } else if (gameInputs.isPressing(EventKeyboard::KeyCode::KEY_D)) {
      hero->runCommand(COMMAND_INPUT, {{PARAM_INPUT, Any(INPUT_RIGHT)}});
    }
  }
}

void EditorGameScene::onCameraUpdate(const cocos2d::Vec2 &relative) {
  EditorManager::instance().getGridNode()->setPosition(relative);
}

void EditorGameScene::clean() {
  GameLayerContainer::clean();
  GameLevel::instance().unload();
}

void EditorGameScene::onGameEnabled(bool val) {
  EditorManager::instance().onGameEnabled(val);
}

void EditorGameScene::onWinGame() {
}

void EditorGameScene::keyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
  GameInputs::instance().keyPressed(keyCode, event);
}

void EditorGameScene::keyReleased(EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
  GameInputs::instance().keyReleased(keyCode, event);
}

void EditorGameScene::afterLoad() {
  EditorManager::instance().afterLoad();

  auto filename = GameLevel::instance().getCurrentLevelFile();
  UILayer::Layer->setFileName(filename.empty() ? "Untitled" : filename);
  UILayer::Layer->addMessage("File loaded");
}

#endif
