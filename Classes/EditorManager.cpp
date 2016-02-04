//
//  EditorManager.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 2/3/16.
//
//

#include "EditorManager.h"
#include "GameLevel.h"
#include "GameObject.h"
#include "GameInputs.h"
#include "GameLayerContainer.h"
#include "GameConfig.h"
#include "PathLib.h"
#include "VisibleRect.h"

USING_NS_CC;

void EditorManager::init() {
  initHelpers();
  registerInputs();

  loadLastEdit();
}

void EditorManager::initHelpers() {
  mEditorRoot = Node::create();
  mEditorRoot->setCameraMask((unsigned short) CameraFlag::USER2);
  GameLevel::instance().getGameLayer()->addChild(mEditorRoot, ZORDER_EDT_HELPER_EDITOR);

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
  mEditorRoot->addChild(mGridNode, ZORDER_EDT_GRID);
  mGridNode->setVisible(false);
}

void EditorManager::openMapFile() {
  std::string fullpath = PathLib::getMapDir();
  auto filename = PathLib::openJsonFile("Open Map", fullpath + "/remote");
  if (filename.empty()) {
    return;
  }
  GameLevel::instance().load(filename);
}

void EditorManager::toggleHelpersVisible() {
  bool visible = mGridNode->isVisible();
  mGridNode->setVisible(!visible);
  GameLevel::instance().traverseObjects([&](GameObject* obj) {
    obj->getHelperNode()->setVisible(!visible);
  }, true);
}

void EditorManager::afterLoad() {
  mGridNode->setPosition(Vec2::ZERO);
  if (mGridNode->isVisible()) {
    toggleHelpersVisible();
  }
}

void EditorManager::loadLastEdit() {
  auto file = UserDefault::getInstance()->getStringForKey("lastedit");
  GameLevel::instance().load(file.empty() ? TEMPLATE_MAP : file);
}

void EditorManager::onMouseDown(const MouseEvent &event) {
  // Left button only.
  if (event.button != 0) {
    return;
  }

  if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_SHIFT)) {
  }
}

void EditorManager::onMouseUp(const MouseEvent &event) {
  // Left button only.
  if (event.button != 0) {
    return;
  }
}

void EditorManager::onMouseMove(const MouseEvent &event) {
}

void EditorManager::registerInputs() {
  auto &gameInputs = GameInputs::instance();

  // Enter or leave game mode.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_L, [this](GameInputs::KeyCode key) {
    GameLevel::instance().enableGame(!GameLevel::instance().isGameEnabled());
  });

  // Show helpers.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_G, [this](GameInputs::KeyCode key) {
    toggleHelpersVisible();
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
