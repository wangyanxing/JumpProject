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
#include "UILayer.h"
#include "HttpHelper.h"
#include "ShadowManager.h"
#include "JsonWriter.h"
#include "JsonParser.h"
#include "PathComponent.h"

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

void EditorManager::newMapFile() {
  auto current = GameLevel::instance().getCurrentLevelFile();
  if (!current.empty()) {
    GameLevel::instance().save(current);
    UserDefault::getInstance()->setStringForKey("lastedit", current);
    UserDefault::getInstance()->flush();
  }
  GameLevel::instance().load("");
}

void EditorManager::openMapFile() {
  std::string fullpath = PathLib::getMapDir();
  auto filename = PathLib::openJsonFile("Open Map", fullpath + "/remote");
  if (filename.empty()) {
    return;
  }
  GameLevel::instance().load(filename);
}

void EditorManager::saveMapFile() {
  auto current = GameLevel::instance().getCurrentLevelFile();
  if (current.empty()) {
    // Save as.
    std::string fullpath = PathLib::getMapDir();
    std::vector<std::string> out;
    auto filter = "JSON file(json)|*.json|All files (*.*)|*.*";
    PathLib::saveFileDialog(nullptr, "Save map", fullpath + "/local", "", filter, 0, out);
    if (out.empty()) {
      return;
    }
    auto filename = out[0];
#if EDITOR_RATIO == EDITOR_IPAD_MODE
    if (!PathLib::endsWith(filename, "_pad.json")) {
      PathLib::replaceString(filename, ".json", "_pad.json");
    }
#elif EDITOR_RATIO == EDITOR_IP4_MODE
    if (!PathLib::endsWith(filename, "_ip4.json")) {
      PathLib::replaceString(filename, ".json", "_ip4.json");
    }
#endif
    current = filename;
  }
  GameLevel::instance().save(current);
  UserDefault::getInstance()->setStringForKey("lastedit", current);
  UserDefault::getInstance()->flush();

  UILayer::Layer->setFileName(current);
  UILayer::Layer->addMessage("File saved");

  // Upload to server.
  std::string filename = current;
  const size_t last_slash_idx = filename.find_last_of("\\/");
  if (std::string::npos != last_slash_idx) {
    filename.erase(0, last_slash_idx + 1);
  }
  HttpHelper::uploadMap(filename,
                        JsonWriter::getAuthorString(),
                        JsonWriter::getTimeString(),
                        JsonParser::getBuffer(current));
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

void EditorManager::update(float dt) {
  if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_CTRL)) {
    if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_COMMA)) {
      GameLevel::instance().getShadowManager(0)->addLightDirDegree(dt * 100);
    } else if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_PERIOD)) {
      GameLevel::instance().getShadowManager(0)->addLightDirDegree(dt * -100);
    }
  }
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
    clearSelections();
    createDefaultObject(event.posInMap);
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
  pathEditorMode(false);
}

void EditorManager::moveThings(EventKeyboard::KeyCode key) {
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

  if (mPathEditMode) {
    movePathNode(dir);
  } else {
    if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_SHIFT)) {
      alignObjects(dir);
    } else {
      moveObjects(dir);
    }
  }
}

void EditorManager::alignObjects(const cocos2d::Vec2 &dir) {
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
}

void EditorManager::moveObjects(const cocos2d::Vec2 &dir) {
  float distance = GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_ALT) ? 20 : 1;
  Vec2 movement = dir * distance;

  for (auto obj : mSelections) {
    obj->runCommand(COMMAND_EDITOR, {
      {PARAM_EDITOR_COMMAND, Any(EDITOR_CMD_MOVE)},
      {PARAM_MOUSE_MOVEMENT, Any(movement)}
    });
  }
}

void EditorManager::movePathNode(const cocos2d::Vec2 &dir) {
  auto selection = mSelections.front();
  if (!selection->hasComponent(COMPONENT_PATH)) {
    selection->addComponent<PathComponent>();
  }
  auto path = selection->getComponent<PathComponent>();
  if (path->empty()) {
    return;
  }

  float distance = GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_ALT) ? 20 : 1;
  Vec2 movement = dir * distance;
  path->setBackPos(path->getBackPos() + movement);
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

GameObject *EditorManager::createDefaultObject(const cocos2d::Vec2 &pos) {
  Parameter param;
  param.set(PARAM_POS, pos)
       .set(PARAM_SIZE, Size(100, 15))
       .set(PARAM_BLOCK_KIND, KIND_BLOCK);
  auto obj = GameLevel::instance().getObjectManager()->createObject(param);
  obj->setEnabled(GameLevel::instance().isGameEnabled());
  obj->getHelperNode()->setVisible(mGridNode->isVisible());
  return obj;
}

void EditorManager::changeKindOrShadowLayer(cocos2d::EventKeyboard::KeyCode key) {
  if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_SHIFT)) {
    int layer = (int) key - (int)GameInputs::KeyCode::KEY_1;
    if (layer >= ShadowManager::NUM_SHADOW_LAYERS) {
      return;
    }
    for (auto obj : mSelections) {
      obj->getRenderer()->setShadowLayer(layer);
    }
  } else {
    BlockKind kind = (BlockKind)((int) key - (int)GameInputs::KeyCode::KEY_1 + 1);
    for (auto obj : mSelections) {
      obj->changeKind(kind);
    }
  }
}

void EditorManager::pathEditorMode(bool mode) {
  if (mPathEditMode == mode) {
    return;
  }
  if (mode && mSelections.empty()) {
    return;
  }

  mPathEditMode = mode;
  UILayer::Layer->setEditModeName(mPathEditMode ? "Path Mode" : "");

  if (mPathEditMode) {
    if (!mGridNode->isVisible()) {
      toggleHelpersVisible();
    }
    addPathPoint();
  }
}

void EditorManager::groupObjects() {
  if (mSelections.size() < 2) {
    return;
  }
  auto head = mSelections[0];
  if (head->hasChildren()) {
    head->removeAllChildren();
  } else {
    for (size_t i = 1; i < mSelections.size(); ++i) {
      head->addChild(mSelections[i]->getID());
    }
  }
}

void EditorManager::addPathPoint() {
  CC_ASSERT(mPathEditMode);

  Vec2 move = GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_SHIFT) ?
    Vec2(0, 50) : Vec2(50, 0);

  auto selection = mSelections.front();
  if (!selection->hasComponent(COMPONENT_PATH)) {
    selection->addComponent<PathComponent>();
  }
  auto path = selection->getComponent<PathComponent>();

  if (path->empty()) {
    auto pos = selection->getRenderer()->getPosition();
    path->push(pos);
    path->push(pos + move);
  } else {
    auto pos = path->getBackPos();
    path->push(pos + move);
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

  // Path mode.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_F, [this](GameInputs::KeyCode key) {
    if (!mPathEditMode) {
      pathEditorMode(true);
    } else {
      addPathPoint();
    }
  });
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_ESCAPE, [this](GameInputs::KeyCode key) {
    pathEditorMode(false);
  });

  // Load.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_O, [this](GameInputs::KeyCode key) {
    if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_CTRL)) {
      openMapFile();
    }
  });

  // Save.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_S, [this](GameInputs::KeyCode key) {
    if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_CTRL)) {
      saveMapFile();
    }
  });

  // New.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_N, [this](GameInputs::KeyCode key) {
    if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_CTRL)) {
      newMapFile();
    }
  });

  // Open in text editor.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_E, [this](GameInputs::KeyCode key) {
    if (GameInputs::instance().isPressing(EventKeyboard::KeyCode::KEY_CTRL)) {
      PathLib::openInSystem(GameLevel::instance().getCurrentLevelFile().c_str());
    }
  });

  // Save remote maps.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_F4, [this](GameInputs::KeyCode key) {
    HttpHelper::getAllMaps();
  });
  
  // Reload.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_F5, [this](GameInputs::KeyCode key) {
    GameLevel::instance().load(GameLevel::instance().getCurrentLevelFile());
  });

  // Set Kind.
  for (int k = (int)GameInputs::KeyCode::KEY_1;
       k <= (int)GameInputs::KeyCode::KEY_1 + KIND_MAX - 1; ++k) {
    gameInputs.addKeyboardEvent((GameInputs::KeyCode) k,
                                CC_CALLBACK_1(EditorManager::changeKindOrShadowLayer, this));
  }

  // Move.
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_UP_ARROW,
                              CC_CALLBACK_1(EditorManager::moveThings, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_DOWN_ARROW,
                              CC_CALLBACK_1(EditorManager::moveThings, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_LEFT_ARROW,
                              CC_CALLBACK_1(EditorManager::moveThings, this));
  gameInputs.addKeyboardEvent(GameInputs::KeyCode::KEY_RIGHT_ARROW,
                              CC_CALLBACK_1(EditorManager::moveThings, this));

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

  // Group.
  gameInputs.addKeyboardEvent(EventKeyboard::KeyCode::KEY_J, [this](GameInputs::KeyCode key) {
    groupObjects();
  });

  // Delete.
  auto deleteCallback = [this](GameInputs::KeyCode key) {
    if (mSelections.empty()) {
      return;
    }
    if (mPathEditMode) {
      auto sel = mSelections.front();
      if (!sel->hasComponent(COMPONENT_PATH)) {
        return;
      }
      auto path = sel->getComponent<PathComponent>();
      path->pop();
      if (path->empty()) {
        pathEditorMode(false);
      }
    } else {
      for (auto obj : mSelections) {
        GameLevel::instance().getObjectManager()->deleteObject(obj->getID());
      }
      mSelections.clear();
    }
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
}
