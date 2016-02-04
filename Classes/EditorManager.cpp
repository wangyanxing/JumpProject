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
#include "GameRenderer.h"
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
  mSelections.clear();
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
    // Create object.
  } else {
    // Select objects.
    bool multiSelect = GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_CTRL);
    if (!multiSelect) {
      clearSelections();
    }
    GameLevel::instance().traverseObjects([&](GameObject *obj) {
      if (obj->isRemovable() && obj->containsPoint(event.posInMap)) {
        mSelections.push_back(obj);
        obj->runCommand(COMMAND_EDITOR, {{PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_SELECT)}});
      }
    }, false);
  }
}

void EditorManager::onMouseUp(const MouseEvent &event) {
  // Left button only.
  if (event.button != 0) {
    return;
  }
}

void EditorManager::onMouseMove(const MouseEvent &event) {
  if (!GameInputs::instance().isPressingMouse()) {
    return;
  }

  if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_ALT)) {
    // Move camera.
    auto camera = GameLevel::instance().getGameLayer()->getCamera();
    camera->setPosition(camera->getPosition() - event.movement);
  } else {
    // Move objects.
    for (auto obj : mSelections) {
      obj->runCommand(COMMAND_EDITOR, {
        {PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_MOVE)},
        {PARAM_MOUSE_MOVEMENT, Any(event.movement)}
      });
    }
  }
}

void EditorManager::clearSelections() {
  for (auto sel : mSelections) {
    sel->runCommand(COMMAND_EDITOR, {{PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_UNSELECT)}});
  }
  mSelections.clear();
}

void EditorManager::moveObjects(EventKeyboard::KeyCode key) {
  Vec2 dir;
  if (key == GameInputs::KeyCode::KEY_UP_ARROW) {
    dir = {0, 1};
  } else if (key == GameInputs::KeyCode::KEY_DOWN_ARROW) {
    dir = {0, -1};
  } else if (key == GameInputs::KeyCode::KEY_LEFT_ARROW) {
    dir = {-1, 0};
  } else if (key == GameInputs::KeyCode::KEY_RIGHT_ARROW) {
    dir = {1, 0};
  }
  float distance = GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_ALT) ? 20 : 1;
  dir *= distance;

  for (auto obj : mSelections) {
    obj->runCommand(COMMAND_EDITOR, {
      {PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_MOVE)},
      {PARAM_MOUSE_MOVEMENT, Any(dir)}
    });
  }
}

void EditorManager::resizeObjects(cocos2d::EventKeyboard::KeyCode key) {
  Vec2 delta;
  if (key == GameInputs::KeyCode::KEY_LEFT_BRACKET) {
    delta = {-1, 0};
  } else if (key == GameInputs::KeyCode::KEY_RIGHT_BRACKET) {
    delta = {1, 0};
  } else if (key == GameInputs::KeyCode::KEY_MINUS) {
    delta = {0, -1};
  } else if (key == GameInputs::KeyCode::KEY_EQUAL) {
    delta = {0, 1};
  }

  float size = GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_ALT) ? 20 : 5;
  delta *= size;

  for (auto obj : mSelections) {
    obj->runCommand(COMMAND_EDITOR, {
      {PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_RESIZE)},
      {PARAM_SIZE_DELTA, Any(delta)}
    });
  }
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

  // Move.
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_UP_ARROW,
                              CC_CALLBACK_1(EditorManager::moveObjects, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_DOWN_ARROW,
                              CC_CALLBACK_1(EditorManager::moveObjects, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_LEFT_ARROW,
                              CC_CALLBACK_1(EditorManager::moveObjects, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_RIGHT_ARROW,
                              CC_CALLBACK_1(EditorManager::moveObjects, this));

  // Resize.
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_LEFT_BRACKET,
                              CC_CALLBACK_1(EditorManager::resizeObjects, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_RIGHT_BRACKET,
                              CC_CALLBACK_1(EditorManager::resizeObjects, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_MINUS,
                              CC_CALLBACK_1(EditorManager::resizeObjects, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_EQUAL,
                              CC_CALLBACK_1(EditorManager::resizeObjects, this));

  // Rotate.
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_P, [this](GameInputs::KeyCode key) {
    for (auto obj : mSelections) {
      obj->runCommand(COMMAND_EDITOR, {{PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_ROTATE)}});
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
