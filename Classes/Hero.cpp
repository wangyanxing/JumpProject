//
//  Hero.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/14/15.
//
//

#include "Hero.h"
#include "LogicManager.h"

USING_NS_CC;

void Hero::initShader() {
  BlockBase::initShader();

#if 0
  mTrailFx = ParticleSystemQuad::create("fx/trail.plist");
  mTrailFxNode = ParticleBatchNode::createWithTexture(mTrailFx->getTexture());
  mTrailFxNode->addChild(mTrailFx);
  mTrailFxNode->setPosition(getPosition());
  GameLogic::Game->mParentLayer->addChild(mTrailFxNode, getZOrder() - 2);
#endif
}

void Hero::updateMovement(float dt) {
  Vec2 linkMove;
  if(mLinkingID != -1) {
    auto b = GameLogic::Game->findBlock(mLinkingID);
    if(b) {
      linkMove.y = b->mMovementThisFrame.y;
    }
    mLinkingID = -1;
  }

  mVelocity += mJumpVelocity;
  mJumpVelocity.set(0,0);

  mVelocity += mForceFieldVelocity;
  mForceFieldVelocity.set(0,0);

  // Linear damping
  float damping = 5;
  mVelocity.x *= std::min(std::max(1.0f - dt * damping, 0.0f), 1.0f);

  auto lastpos = mSprite->getPosition() + mVelocity * dt + linkMove;
  mSprite->setPosition(lastpos);
}

void Hero::initPhysics() {
  if (mSprite->getPhysicsBody()) {
    mSprite->removeComponent(mSprite->getPhysicsBody());
  }

  PhysicsBody* pbody = nullptr;
#if 0
  pbody = PhysicsBody::createBox(mSprite->getContentSize());
#else
  pbody = PhysicsBody::createBox(getSize());
#endif
  pbody->setDynamic(true);
  pbody->setRotationEnable(false);
  pbody->setMoment(0);
  pbody->setMass(0.8);
  pbody->setContactTestBitmask(1);
  pbody->setGroup(1);
  pbody->setGravityEnable(false);
  pbody->setScaleEnabled(false);
  mSprite->setPhysicsBody(pbody);

  mEnableGravity = true;
  mEnableForceField = true;
}
