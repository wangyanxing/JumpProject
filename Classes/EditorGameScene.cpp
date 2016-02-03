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
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  GameInputs::instance().addKeyboardEvent(EventKeyboard::KeyCode::KEY_SPACE,
                                          [&](GameInputs::KeyCode key) {
      GameLevel::instance().getHero()->runCommand(COMMAND_INPUT, {{PARAM_INPUT, Any(INPUT_JUMP)}});
  });
  
  // Register editor commands.
  registerCommands();

  // Helpers.
  initHelpers();

  // Load last edited map.
  loadLastEdit();
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
  mGridNode->setPosition(relative);
}

void EditorGameScene::registerCommands() {
  auto &gameInputs = GameInputs::instance();
  
  // Enter or leave game mode.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_L, [this](GameInputs::KeyCode key) {
      GameLevel::instance().enableGame(!GameLevel::instance().isGameEnabled());
  });
  
  // Show helpers.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_G, [this](GameInputs::KeyCode key) {
    toggleHelpersVisible();
  });

  // Reset scene.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_0, [this](GameInputs::KeyCode key) {
    GameLevel::instance().unload();
  });

  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_O, [this](GameInputs::KeyCode key) {
    if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_CTRL)) {
      openMapFile();
    }
  });

  // Test.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_7, [this](GameInputs::KeyCode key) {
    GameLevel::instance().load("maps/local/test_refactor1.json");
  });
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_8, [this](GameInputs::KeyCode key) {
    GameLevel::instance().load("maps/local/test_refactor2.json");
  });
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_9, [this](GameInputs::KeyCode key) {
    GameLevel::instance().load("maps/local/test_refactor3.json");
  });
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

void EditorGameScene::initHelpers() {
  mGridNode = DrawNode::create();
  Color4F gridColor(0.8f, 0.8f, 0.8f, 0.3f);

  for (float y = 0; y < VisibleRect::top().y;) {
    mGridNode->drawLine(Vec2(0, y), Vec2(VisibleRect::right().x, y), gridColor);
    y += GameConfig::instance().HeroSize;
  }
  for (float x = 0; x < VisibleRect::right().x + 50;) {
    mGridNode->drawLine(Vec2(x, 0), Vec2(x, VisibleRect::top().y), gridColor);
    x += GameConfig::instance().HeroSize;
  }

  mGridNode->setCameraMask((unsigned short) CameraFlag::USER2);
  addChild(mGridNode, ZORDER_EDT_GRID);
  mGridNode->setVisible(false);
}

void EditorGameScene::toggleHelpersVisible() {
  bool visible = mGridNode->isVisible();
  mGridNode->setVisible(!visible);
  GameLevel::instance().traverseObjects([&](GameObject* obj) {
    obj->getHelperNode()->setVisible(!visible);
  }, true);
}

void EditorGameScene::afterLoad() {
  mGridNode->setPosition(Vec2::ZERO);
  if (mGridNode->isVisible()) {
    toggleHelpersVisible();
  }
}

void EditorGameScene::openMapFile() {
  std::string fullpath = PathLib::getMapDir();
  auto filename = PathLib::openJsonFile("Open Map", fullpath + "/remote");
  if (filename.empty()) {
    return;
  }
  GameLevel::instance().load(filename);
}

void EditorGameScene::loadLastEdit() {
  auto file = UserDefault::getInstance()->getStringForKey("lastedit");
  GameLevel::instance().load(file.empty() ? TEMPLATE_MAP : file);
}

#endif
