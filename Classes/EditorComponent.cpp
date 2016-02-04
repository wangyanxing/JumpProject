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
  mParent->removeComponentCommand(COMMAND_EDITOR);
}

void EditorComponent::update(float dt) {
}

void EditorComponent::beforeRender(float dt) {
}

void EditorComponent::updateHelpers() {
  static const Color4F fillColor(0.92f, 0.95f, 0.1f, 0.3f);
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
  EditorManager::instance().getEditorRootNode()->addChild(mSelectionHelper,
                                                          ZORDER_EDT_OBJ_SELECTION);
}

void EditorComponent::runCommand(ComponentCommand type, const Parameter &param) {
  CC_ASSERT(type == COMMAND_EDITOR);
}
