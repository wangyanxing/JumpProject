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
  static const Color4F fillColor(0.92f, 0.95f, 0.1f, 0.6f);
  static const Color4F outlineColor(0.86f, 0.1f, 0.9f, 1.0f);

  if (!mSelectionHelper) {
    initHelpers();
  }

  mSelectionHelper->clear();
  PhysicsComponent *physics = mParent->getComponent<PhysicsComponent>();
  if (physics) {
    physics->getShape()->debugDraw(mSelectionHelper, fillColor, outlineColor);
  } else {
    auto rect = mParent->getRenderer()->getBoundingBox();
    Vec2 seg[4] = {
      {-rect.size.width * 0.5f,  rect.size.height * 0.5f},
      { rect.size.width * 0.5f,  rect.size.height * 0.5f},
      { rect.size.width * 0.5f, -rect.size.height * 0.5f},
      {-rect.size.width * 0.5f, -rect.size.height * 0.5f}
    };
    mSelectionHelper->drawPolygon(seg, 4, fillColor, 1, outlineColor);
  }
  mSelectionHelper->setPosition(mParent->getRenderer()->getPosition());
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

void EditorComponent::runCommand(ComponentCommand type, const Parameter &param) {
  CC_ASSERT(type == COMMAND_EDITOR);
  auto cmd = param.get<EditorCommand>(PARAM_EDITOR_COMMAND);
  if (cmd == EDITOR_CMD_SELECT) {
    mSelectionHelper->setVisible(true);
  } else if (cmd == EDITOR_CMD_UNSELECT) {
    mSelectionHelper->setVisible(false);
  } else if (cmd == EDITOR_CMD_MOVE) {
    auto movement = param.get<Vec2>(PARAM_MOUSE_MOVEMENT);
    mParent->getRenderer()->move(movement);
  }
}
