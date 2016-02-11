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

void InputComponent::update(float dt) {
  auto physics = mParent->getComponent<PhysicsComponent>();
  if (mPressingLeft) {
    physics->setAccelerationX(-GameConfig::instance().MoveAcceleration);
  } else if (mPressingRight) {
    physics->setAccelerationX(GameConfig::instance().MoveAcceleration);
  }
}

void InputComponent::runCommand(ComponentCommand type, const Parameter &param) {
  if (!isEnabled()) {
    return;
  }

  CC_ASSERT(type == COMMAND_INPUT);
  auto input = param.get<InputType>(PARAM_INPUT);
  auto pressed = param.get<bool>(PARAM_INPUT_STATUS);
  
  if (input == INPUT_LEFT) {
    mPressingLeft = pressed;
  } else if (input == INPUT_RIGHT) {
    mPressingRight = pressed;
  } else if (input == INPUT_JUMP && pressed) {
    auto physics = mParent->getComponent<PhysicsComponent>();
    physics->setAccelerationY(GameConfig::instance().JumpAcceleration);
  }
}
