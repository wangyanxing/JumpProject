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
#include "MathTools.h"
#include "GameRenderer.h"
#include "GameEvents.h"
#include "RendererActions.h"

USING_NS_CC;

#define PUSH_DISTANCE_THRESHOLD 0.85f
#define RESTORE_FRAME_INTERVAL 10
#define RESTORE_ACTION_TAG 5
#define RESTORE_DURATION 0.2

ButtonComponent::ButtonComponent(GameObject *parent) : GameComponent(parent) {
  mParent->addComponentCommand(COMMAND_COLLISION, this);
}

ButtonComponent::~ButtonComponent() {
  mParent->removeComponentCommand(COMMAND_COLLISION);
}

void ButtonComponent::update(float dt) {
  if (mState == PUSHED && mCanRestore &&
      Director::getInstance()->getTotalFrames() - mLastPushFrameNum > RESTORE_FRAME_INTERVAL) {
    changeState(RESTORING);
    auto buttonRenderer = getParent()->getRenderer();
    auto action = Sequence::create(RendererMoveTo::create(buttonRenderer,
                                                          RESTORE_DURATION,
                                                          buttonRenderer->getOriginalPosition()),
                                   CallFunc::create([this]() {
      changeState(IDLE);
    }), nullptr);
    action->setTag(RESTORE_ACTION_TAG);
    buttonRenderer->getNode()->runAction(action);
  }
}

Vec2 ButtonComponent::getDirectionVec() {
  static Vec2 directions[] = {
    {0, -1},
    {0,  1},
    {1,  0},
    {-1, 0}
  };
  return directions[mDirection];
}

void ButtonComponent::load(JsonValueT &json) {
  mDirection = json[BUTTON_DIR].getEnum<ButtonDirection>();
  mCanRestore = json[BUTTON_CAN_RESTORE].GetBool();
  mPushedEvents = json[BUTTON_PUSHED_EVENT].getStringVec();
  mRestoredEvents = json[BUTTON_RESTORED_EVENT].getStringVec();
  mPushingEvents = json[BUTTON_PUSHING_EVENT].getStringVec();
}

void ButtonComponent::clone(GameComponent *otherComp) {
  CC_ASSERT(otherComp->getComponentType() == getComponentType());
  ButtonComponent *other = static_cast<ButtonComponent*>(otherComp);

  mDirection = other->mDirection;
  mCanRestore = other->mCanRestore;
  mPushedEvents = other->mPushedEvents;
  mRestoredEvents = other->mRestoredEvents;
  mPushingEvents = other->mPushingEvents;
}

void ButtonComponent::save(JsWriter &writer) {
  writer.String(BUTTON_DIR);
  writer.Enum<ButtonDirection>(mDirection);

  writer.String(BUTTON_CAN_RESTORE);
  writer.Bool(mCanRestore);

  writer.String(BUTTON_PUSHED_EVENT);
  writer.StartArray();
  for (auto &s : mPushedEvents) {
    writer.String(s);
  }
  writer.EndArray();

  writer.String(BUTTON_RESTORED_EVENT);
  writer.StartArray();
  for (auto &s : mRestoredEvents) {
    writer.String(s);
  }
  writer.EndArray();

  writer.String(BUTTON_PUSHING_EVENT);
  writer.StartArray();
  for (auto &s : mPushingEvents) {
    writer.String(s);
  }
  writer.EndArray();
}

void ButtonComponent::reset() {
  GameComponent::reset();
  mState = IDLE;
}

void ButtonComponent::callEvents(const std::vector<std::string>& events) {
  for (auto &str : events) {
    if (!str.empty()) {
      GameEvents::instance().callSingleEvent(str, mParent);
    }
  }
}

void ButtonComponent::changeState(ButtonState newState) {
  if (mState != newState) {
    if (newState == PUSHED) {
      callEvents(mPushedEvents);
    } else if (mState == RESTORING && newState == IDLE) {
      callEvents(mRestoredEvents);
    }
    mState = newState;
  } else if (mState == PUSHING) {
    callEvents(mPushingEvents);
  }
}

void ButtonComponent::runCommand(ComponentCommand type, const Parameter &param) {
  if (!isEnabled()) {
    return;
  }

  CC_ASSERT(type == COMMAND_COLLISION);
  // Mark the frame number.
  mLastPushFrameNum = Director::getInstance()->getTotalFrames();

  auto info = param.get<const CollisionInfo*>(PARAM_COLLISION_INFO);
  auto dirVec = getDirectionVec();
  auto shape1 = info->obj1->getShape(), shape2 = info->obj2->getShape();
  auto buttonRenderer = info->obj2->getParent()->getRenderer();

  if (mState == RESTORING) {
    buttonRenderer->getNode()->stopActionByTag(RESTORE_ACTION_TAG);
  }
  
  float deltaWidth = 0.5f * (shape1->getBounds().size.width + shape2->getBounds().size.width) -
      fabs(shape1->getPosition().x - shape2->getPosition().x);
  float deltaHeight = 0.5f * (shape1->getBounds().size.height + shape2->getBounds().size.height) -
      fabs(shape1->getPosition().y - shape2->getPosition().y);
  Vec2 delta(deltaWidth, deltaHeight);
  
  if (MathTools::vec2Equal(dirVec, info->normal)) {
    // Push it.
    auto pos = shape2->getPosition();
    pos.x -= delta.x * info->normal.x;
    pos.y -= delta.y * info->normal.y;
    shape2->onPositionSet(pos);

    float disToOrigin = buttonRenderer->getOriginalPosition().distance(pos);
    float threshold = PUSH_DISTANCE_THRESHOLD *
        ((mDirection == BUTTON_UP || mDirection == BUTTON_DOWN) ?
         shape2->getBounds().size.height :
         shape2->getBounds().size.width);
    changeState(disToOrigin > threshold ? PUSHED : PUSHING);
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
