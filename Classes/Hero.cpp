//
//  Hero.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/14/15.
//
//

#include "Hero.h"
#include "LogicManager.h"
#include "BlockRenderer.h"

USING_NS_CC;

void Hero::initShader() {
  BlockBase::initShader();
}

void Hero::updateMovement(float dt) {
  Vec2 linkMove;
  if (mLinkingID != -1) {
    auto b = GameLogic::Game->findBlock(mLinkingID);
    if (b) {
      linkMove.y = b->mMovementThisFrame.y;
    }
    mLinkingID = -1;
  }

  mVelocity += mJumpVelocity;
  mJumpVelocity.set(0, 0);

  // Linear damping
  float damping = 5;
  mVelocity.x *= std::min(std::max(1.0f - dt * damping, 0.0f), 1.0f);

  auto lastpos = getRenderer()->getPosition() + mVelocity * dt + linkMove;
  getRenderer()->setPosition(lastpos);
}

void Hero::initPhysics() {
  getRenderer()->removePhysicsBody();

  PhysicsBody *pbody = nullptr;
  pbody = PhysicsBody::createBox(getSize());
  pbody->setDynamic(true);
  pbody->setRotationEnable(false);
  pbody->setMoment(0);
  pbody->setMass(0.8);
  pbody->setContactTestBitmask(1);
  pbody->setGroup(1);
  pbody->setGravityEnable(false);
  pbody->setScaleEnabled(false);
  getRenderer()->setPhysicsBody(pbody);

  mEnableGravity = true;
}
