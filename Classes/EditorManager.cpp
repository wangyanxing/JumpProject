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
#include "ObjectManager.h"
#include "PathLib.h"
#include "VisibleRect.h"

USING_NS_CC;

void EditorManager::init() {
  initHelpers();
  registerInputs();
  loadLastEdit();
}

void EditorManager::onGameEnabled(bool val) {
  mHeroSpawnNode->setVisible(!val);
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

  mHeroSpawnNode = DrawNode::create();
  float spawnSize = 5;
  mHeroSpawnNode->drawSegment({-spawnSize, -spawnSize}, {spawnSize, spawnSize}, 1, Color4F::YELLOW);
  mHeroSpawnNode->drawSegment({-spawnSize, spawnSize}, {spawnSize, -spawnSize}, 1, Color4F::YELLOW);
  mHeroSpawnNode->setCameraMask((unsigned short) CameraFlag::USER2);
  mHeroSpawnNode->setVisible(false);
  mEditorRoot->addChild(mHeroSpawnNode, ZORDER_EDT_GRID);
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
  mHeroSpawnNode->setPosition(GameLevel::instance().getHero()->getRenderer()->getPosition());
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
  if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_M)) {
    // Set spawn position.
    GameLevel::instance().setHeroSpawnPosition(event.posInMap);
    mHeroSpawnNode->setPosition(event.posInMap);
  } else if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_SHIFT)) {
    // Create object.
  } else {
    // Select objects.
    bool multiSelect = GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_CTRL);
    if (!multiSelect) {
      clearSelections();
    }
    GameLevel::instance().traverseObjects([&](GameObject *obj) {
      if (obj->isRemovable() && obj->containsPoint(event.posInMap)) {
        bool first = mSelections.empty();
        mSelections.push_back(obj);
        obj->runCommand(COMMAND_EDITOR, {
          {PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_SELECT)},
          {PARAM_FIRST_SELECTION, Any(first)}
        });
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
  if (mSelections.empty()) {
    return;
  }

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

  if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_SHIFT)) {
    // Alignment.
    auto headPos = mSelections.front()->getRenderer()->getPosition();
    Size halfHeadSize = mSelections.front()->getRenderer()->getSize() / 2;
    Vec2 offsetHead = {
      headPos.x + dir.x * halfHeadSize.width,
      headPos.y + dir.y * halfHeadSize.height
    };

    for (size_t i = 1; i < mSelections.size(); ++i) {
      auto pos = mSelections[i]->getRenderer()->getPosition();
      Size halfSize = mSelections[i]->getRenderer()->getSize() / 2;
      Vec2 offset = {
        pos.x + dir.x * halfSize.width,
        pos.y + dir.y * halfSize.height
      };
      Vec2 cof = {fabsf(dir.x), fabsf(dir.y)};
      Vec2 delta = (offsetHead - offset);
      delta.x *= cof.x;
      delta.y *= cof.y;

      mSelections[i]->runCommand(COMMAND_EDITOR, {
        {PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_MOVE)},
        {PARAM_MOUSE_MOVEMENT, Any(delta)}
      });
    }
  } else {
    // Move.
    float distance = GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_ALT) ? 20 : 1;
    dir *= distance;

    for (auto obj : mSelections) {
      obj->runCommand(COMMAND_EDITOR, {
        {PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_MOVE)},
        {PARAM_MOUSE_MOVEMENT, Any(dir)}
      });
    }
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

  // Delete.
  auto deleteCallback = [this](GameInputs::KeyCode key) {
    auto objManager = GameLevel::instance().getObjectManager();
    for (auto obj : mSelections) {
      objManager->deleteObject(obj->getID());
    }
    mSelections.clear();
  };
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_DELETE, deleteCallback);
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_BACKSPACE, deleteCallback);

  // Clone.
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_V, [this](GameInputs::KeyCode key) {
    std::vector<GameObject*> newSelection;
    for (auto obj : mSelections) {
      auto newObj = GameLevel::instance().getObjectManager()->cloneObject(obj);
      newObj->setEnabled(GameLevel::instance().isGameEnabled());

      bool first = newSelection.empty();
      newSelection.push_back(newObj);
      newObj->runCommand(COMMAND_EDITOR, {
        {PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_SELECT)},
        {PARAM_FIRST_SELECTION, Any(first)}
      });
    }
    clearSelections();
    mSelections = newSelection;
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
