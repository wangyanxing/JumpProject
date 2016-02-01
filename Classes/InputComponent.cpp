//
//  InputComponent.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/18/16.
//
//

#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "GameObject.h"
#include "GameConfig.h"

InputComponent::InputComponent(GameObject *parent) : GameComponent(parent) {
  mParent->addComponentCommand(COMMAND_INPUT, this);
}

InputComponent::~InputComponent() {
  mParent->removeComponentCommand(COMMAND_INPUT);
}

void InputComponent::runCommand(ComponentCommand type, const Parameter &param) {
  if (!isEnabled()) {
    return;
  }

  CC_ASSERT(type == COMMAND_INPUT);
  auto input = param.get<InputType>(PARAM_INPUT);
  auto physics = mParent->getComponent<PhysicsComponent>();
  
  if (input == INPUT_LEFT) {
    physics->setAccelerationX(-GameConfig::instance().MoveAcceleration);
  } else if (input == INPUT_RIGHT) {
    physics->setAccelerationX(GameConfig::instance().MoveAcceleration);
  } else if (input == INPUT_JUMP) {
    physics->setAccelerationY(GameConfig::instance().JumpAcceleration);
  }
}
