//
//  ButtonComponent.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/20/16.
//
//

#include "ButtonComponent.h"
#include "JsonFormat.h"
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsShape.h"
#include "PhysicsManager.h"
#include "GameUtils.h"

USING_NS_CC;

ButtonComponent::ButtonComponent(GameObject *parent) : GameComponent(parent) {
  mParent->addComponentCommand(COMMAND_COLLISION, this);
}

ButtonComponent::~ButtonComponent() {
  mParent->removeComponentCommand(COMMAND_COLLISION);
}

void ButtonComponent::update(float dt) {
  
}

Vec2 ButtonComponent::getDirectionVec() {
  static Vec2 directions[] = {
    {0, -1},
    {0, 1},
    {1, 0},
    {-1, 0}
  };
  return directions[mDirection];
}

void ButtonComponent::load(JsonValueT &json) {
  mDirection = json[BUTTON_DIR].getEnum<ButtonDirection>();
  mPushedEvents = json[BUTTON_PUSHED_EVENT].getStringVec();
  mRestoredEvents = json[BUTTON_RESTORED_EVENT].getStringVec();
  mPushingEvents = json[BUTTON_PUSHING_EVENT].getStringVec();
}

void ButtonComponent::reset() {
}

void ButtonComponent::runCommand(ComponentCommand type, const Parameter &param) {
  CC_ASSERT(type == COMMAND_COLLISION);
  auto info = param.get<const CollisionInfo*>(PARAM_COLLISION_INFO);
  auto dirVec = getDirectionVec();
  
  auto shape1 = info->obj1->getShape(), shape2 = info->obj2->getShape();
  float halfHeight = 0.5f * (shape1->getBounds().size.height + shape2->getBounds().size.height);
  float deltaHeight = fabs(shape1->getPosition().y - shape2->getPosition().y);
  float halfWidth = 0.5f * (shape1->getBounds().size.width + shape2->getBounds().size.width);
  float deltaWidth = fabs(shape1->getPosition().x - shape2->getPosition().x);
  Vec2 delta(halfWidth - deltaWidth, halfHeight - deltaHeight);
  
  if (GameUtils::vec2Equal(dirVec, info->normal)) {
    // Push it.
    auto pos = shape2->getPosition();
    pos.x -= delta.x * info->normal.x;
    pos.y -= delta.y * info->normal.y;
    shape2->onPositionSet(pos);
  } else {
    info->obj1->markSleep();
    
    auto vel = info->obj2->getVelocity();
    if (info->normal.x != 0) {
      vel.x = 0;
    } else {
      vel.y = 0;
    }
    info->obj2->setVelocity(vel);
    
    auto pos = shape1->getPosition();
    pos.x += delta.x * info->normal.x;
    pos.y += delta.y * info->normal.y;
    shape1->onPositionSet(pos);
  }
}
