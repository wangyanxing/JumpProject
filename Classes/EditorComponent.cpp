//
//  EditorComponent.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 2/3/16.
//
//

#include "EditorComponent.h"
#include "EditorManager.h"
#include "PhysicsComponent.h"
#include "PhysicsShape.h"
#include "GameObject.h"
#include "GameRenderer.h"
#include "GameLevel.h"

#if EDITOR_MODE

USING_NS_CC;

EditorComponent::EditorComponent(GameObject *parent) : GameComponent(parent) {
  mParent->addComponentCommand(COMMAND_EDITOR, this);
}

EditorComponent::~EditorComponent() {
  releaseHelpers();
  mParent->removeComponentCommand(COMMAND_EDITOR);
}

void EditorComponent::update(float dt) {
}

void EditorComponent::beforeRender(float dt) {
  if (!mSelectionHelper) {
    initHelpers();
  }
  drawSelectionHelper();
}

void EditorComponent::initHelpers() {
  mSelectionHelper = DrawNode::create();
  mSelectionHelper->setCameraMask((unsigned short) CameraFlag::USER2);
  mSelectionHelper->setVisible(false);
  EditorManager::instance().getEditorRootNode()->addChild(mSelectionHelper,
                                                          ZORDER_EDT_OBJ_SELECTION);
}

void EditorComponent::releaseHelpers() {
  if (mSelectionHelper) {
    mSelectionHelper->removeFromParent();
  }
}

void EditorComponent::drawSelectionHelper() {
  static const Color4F fillColor(0.17f, 0.96f, 0.003f, 0.6f);
  static const Color4F outlineColor(0.86f, 0.1f, 0.9f, 1.0f);
  static const Color4F fillColorFirst(0.92f, 0.31f, 0.84f, 0.8f);
  static const Color4F outlineColorFirst(0.96f, 0.51f, 0.13f, 1.0f);

  mSelectionHelper->clear();
  PhysicsComponent *physics = mParent->getComponent<PhysicsComponent>();
  if (physics) {
    physics->getShape()->debugDraw(mSelectionHelper,
                                   mIsFirstSelection ? fillColorFirst : fillColor,
                                   mIsFirstSelection ? outlineColorFirst : outlineColor);
  } else {
    auto rect = mParent->getRenderer()->getBoundingBox();
    Vec2 seg[4] = {
      {-rect.size.width * 0.5f,  rect.size.height * 0.5f},
      { rect.size.width * 0.5f,  rect.size.height * 0.5f},
      { rect.size.width * 0.5f, -rect.size.height * 0.5f},
      {-rect.size.width * 0.5f, -rect.size.height * 0.5f}
    };
    mSelectionHelper->drawPolygon(seg, 4, mIsFirstSelection ? fillColorFirst : fillColor, 1,
                                  mIsFirstSelection ? outlineColorFirst : outlineColor);
  }
  mSelectionHelper->setPosition(mParent->getRenderer()->getPosition());
}

void EditorComponent::runCommand(ComponentCommand type, const Parameter &param) {
  CC_ASSERT(type == COMMAND_EDITOR);
  if (!mSelectionHelper) {
    initHelpers();
  }
  
  auto cmd = param.get<EditorCommand>(PARAM_EDITOR_COMMAND);

  if (GameLevel::instance().isGameEnabled() &&
      (cmd == EDITOR_CMD_MOVE || cmd == EDITOR_CMD_RESIZE || cmd == EDITOR_CMD_ROTATE) &&
      mParent->hasComponent(COMPONENT_PATH)) {
    return;
  }

  if (cmd == EDITOR_CMD_SELECT) {
    mIsFirstSelection = param.get<bool>(PARAM_FIRST_SELECTION);
    mSelectionHelper->setVisible(true);
  } else if (cmd == EDITOR_CMD_UNSELECT) {
    mSelectionHelper->setVisible(false);
  } else if (cmd == EDITOR_CMD_MOVE) {
    auto movement = param.get<Vec2>(PARAM_MOUSE_MOVEMENT);
    mParent->getRenderer()->move(movement);
  } else if (cmd == EDITOR_CMD_RESIZE) {
    auto delta = param.get<Vec2>(PARAM_SIZE_DELTA);
    mParent->getRenderer()->reSize(delta);
  } else if (cmd == EDITOR_CMD_ROTATE) {
    auto size = mParent->getRenderer()->getSize();
    mParent->getRenderer()->setSize({size.height, size.width});
  }
}

#endif
