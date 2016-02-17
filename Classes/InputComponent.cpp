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
#include "GameRenderer.h"

USING_NS_CC;

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

    // Jump effect.
    auto node = mParent->getRenderer()->getNode();
    float scale = node->getScaleX();
    if(node->getNumberOfRunningActions() == 0) {
      node->runAction(Sequence::create(ScaleTo::create(0.2,scale * 0.6,scale * 1.4),
                                       ScaleTo::create(0.2,scale,scale),
                                       nullptr));
    }
  }
}
