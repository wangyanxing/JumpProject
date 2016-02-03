//
//  EditorComponent.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 2/3/16.
//
//

#include "EditorComponent.h"
#include "EditorManager.h"
#include "GameObject.h"
#include "GameRenderer.h"

USING_NS_CC;

EditorComponent::EditorComponent(GameObject *parent) : GameComponent(parent) {
  mParent->addComponentCommand(COMMAND_EDITOR, this);
}

EditorComponent::~EditorComponent() {
  mParent->removeComponentCommand(COMMAND_EDITOR);
}

void EditorComponent::updateHelpers() {
  if (!mSelectionHelper) {
    initHelpers();
  }
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
