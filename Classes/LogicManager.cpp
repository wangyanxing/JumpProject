//
//  LogicManager.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "LogicManager.h"
#include "Blocks.h"
#include "Hero.h"
#include "GameUtils.h"
#include "Shadows.h"
#include "Button.h"
#include "Shake.h"
#include "VisibleRect.h"
#include "SpriteSoft.h"
#include "TimeEvent.h"
#include "BlockRenderer.h"
#include "Palette.h"

#if EDITOR_MODE
#   include "EditorScene.h"
#else
#   include "GameScene.h"
#endif

#define DIE_FX_TAG 1001

USING_NS_CC;

GameLogic *GameLogic::Game = nullptr;
PhysicsWorld *GameLogic::PhysicsWorld = nullptr;

GameLogic::GameLogic(cocos2d::Layer *parent) {
  mParentLayer = parent;
  Game = this;

  mBackgroundColor = Color3B(30, 181, 199);

  mSpawnPos = VisibleRect::center();

  mHero = new Hero();
  mHero->mRestoreSize.setSize(30, 30);
  mHero->mRestorePosition = VisibleRect::center();
  mHero->setKind(KIND_HERO);
  mHero->setVisible(false);
  mHeroShape = mHero->getRenderer()->getPhysicsBody()->getShapes().front();
  mHero->mShadowLayerID = 1;
  
#if EDITOR_MODE
  createFixedBlocks();
#endif

  enableGame(false);
}

void GameLogic::cleanBackground() {
#if USE_BACKGROUND && EDITOR_MODE
  if (mBack) {
    mBack->removeFromParent();
  }
#endif
}

void GameLogic::initBackground() {
#if USE_BACKGROUND
  mBack = GameUtils::createRect(VisibleRect::getVisibleRect(), Color3B(30, 181, 199));
  mParentLayer->addChild(mBack, ZORDER_BACK);
#endif
}

Node *GameLogic::createParticle(const Vec2 &pos) {
  auto parent = Node::create();
  auto pg = Sprite::create("images/glow.png");
  auto p = Sprite::create("images/particle.png");

  float fadeTime = 10 + rand() % 10;
  fadeTime /= 10;

  pg->runAction(RepeatForever::create(Sequence::create(FadeOut::create(fadeTime),
                                                       FadeIn::create(fadeTime),
                                                       NULL)));
  p->setScale(0.5);

  parent->addChild(pg, 0);
  parent->addChild(p, 1);

  float scale = 3 + rand() % 2;
  scale /= 10;

  parent->setScale(scale);
  parent->setPosition(pos);

  parent->setCameraMask((unsigned short) CameraFlag::USER2);
  mParentLayer->addChild(parent, ZORDER_PARTICLE_STAR);
  return parent;
}

GameLogic::~GameLogic() {
  clean();
  CC_SAFE_DELETE(mHero);
}

#if USE_SHADOW
void GameLogic::addShadowGroup() {
  mShadowNode.push_back(Node::create());
  mShadows.push_back(new ShadowManager());
  if (!mShadowNode.back()->getParent()) {
    mParentLayer->addChild(mShadowNode.back(), ZORDER_SHADOW_1);
  }
}

void GameLogic::initShadowGroup(int groupId) {
  mShadows[groupId]->init(mShadowNode[groupId]);
}
#endif

bool GameLogic::onContactPreSolve(cocos2d::PhysicsContact &contact,
                                  cocos2d::PhysicsContactPreSolve &solve) {
  auto iA = mBlockTable.find(contact.getShapeA()->getBody()->getNode());
  auto iB = mBlockTable.find(contact.getShapeB()->getBody()->getNode());
  if (iA == mBlockTable.end() || iB == mBlockTable.end()) {
    CCLOGWARN("Cannot get the block from the node.");
    return true;
  }

  BlockBase *blockA = iA->second;
  BlockBase *blockB = iB->second;

  PhysicsShape *pusherShape = mHeroShape;
  if (blockA != mHero && blockB != mHero) {
    pusherShape = blockA->pushable() ? contact.getShapeA() : contact.getShapeB();
  }

  auto data = contact.getContactData();
  auto normal = data->normal;
  auto otherShape = pusherShape == contact.getShapeA() ? contact.getShapeB() : contact.getShapeA();

  bool onMovingPlatform = false;
  bool onButton = false;

  if (pusherShape == contact.getShapeA()) {
    normal *= -1;
  }

  auto otherNode = otherShape->getBody()->getNode();

  BlockBase *otherBlock = nullptr;
  BlockBase *thisBlock = nullptr;
  auto i = mBlockTable.find(otherNode);
  if (i != mBlockTable.end()) {
    otherBlock = i->second;
  } else {
    return false;
  }

  if (pusherShape == mHeroShape) {
    thisBlock = mHero;
  } else {
    auto it = mBlockTable.find(pusherShape->getBody()->getNode());
    thisBlock = it->second;
  }

  BlockBase *pushedObject = nullptr;
  BlockBase *pushObject = nullptr;

  if (!otherBlock || !otherBlock->pushable() ||
      (normal.y > 0.9 || normal.y < -0.9 || (otherBlock->pushable() && !otherBlock->mCanPush))) {
    otherBlock->mCanPush = true;
    pushObject = thisBlock;
    pushedObject = otherBlock;
  } else {
    // Pushing
    pushObject = otherBlock;
    pushedObject = thisBlock;
    if (pushedObject == mHero) {
      mHero->mPushing = true;
    }
  }

  auto phyPosPush = pushObject->getRenderer()->getPhysicsBody()->getPosition();
  auto phyPosPushed = pushedObject->getRenderer()->getPhysicsBody()->getPosition();
  auto pushedSize = pushedObject->getSize();
  Size pushSize = pushObject == mHero ? mHero->mRestoreSize : pushObject->getSize();

  if (otherBlock->mKind == KIND_DEATH || otherBlock->mKind == KIND_DEATH_CIRCLE) {
    if (thisBlock == mHero && !mWinFlag) {
      if (otherBlock->mPreciseTrigger) {
        float disX = fabs(thisBlock->getPosition().x - otherBlock->getPosition().x);
        float disY = fabs(thisBlock->getPosition().y - otherBlock->getPosition().y);
        if (disX < 10 && disY < 18) {
          otherBlock->callTriggerEvent();
        }
      } else {
        otherBlock->callTriggerEvent();
      }
    }
    return false;
  }
  if (otherBlock->mKind == KIND_BLOCK) {
    auto p = phyPosPush;
    auto mv = otherBlock->mMovementThisFrame;
    if (normal.y > 0.9) {
      mv.y = 0;
      if (mv != Vec2::ZERO) {
        p += mv;
        onMovingPlatform = true;
        pushObject->setPosition(p);
      }
    } else if (normal.y < -0.9) {
      onMovingPlatform = true;
    }
  } else if (otherBlock->mKind == KIND_BUTTON) {
    if (otherBlock->mButton->push(normal, pushObject)) {
      return false;
    }
    if (otherBlock->mButton->mDir == Button::DIR_UP) {
      onButton = true;
    }
    if (!blockA->canPush() && !blockB->canPush()) {
      return false;
    }
    if (normal.y > 0.9) {
      auto p = pushObject->getRenderer()->getPhysicsBody()->getPosition();
      if (otherBlock->mMovementThisFrame != Vec2::ZERO) {
        p += otherBlock->mMovementThisFrame;
        onMovingPlatform = true;
        pushObject->setPosition(p);
      }
    }
  }

  if (!blockA->canPush() && !blockB->canPush()) {
    if (normal.x > 0.9 || normal.x < -0.9) {
      if (blockA->pushable()) {
        blockA->mCanPush = false;
      } else if (blockB->pushable()) {
        blockB->mCanPush = false;
      } else {
        return true;
      }
    } else if (normal.y > 0.9 || normal.y < -0.9) {
      if (!blockA->pushable() && !blockB->pushable()) {
        return true;
      }
    }
  }

  if (pushObject == mHero) {
    float xdis = std::abs(phyPosPushed.x - phyPosPush.x);
    float ydis = std::abs(phyPosPushed.y - phyPosPush.y);
    if (std::abs(xdis * 2 - (pushSize.width + pushedSize.width)) < 5 &&
        std::abs(ydis * 2 - (pushSize.height + pushedSize.height)) < 5) {
      return false;
    }
  }

  if (normal.x > 0.9 || normal.x < -0.9) {
    auto h = pushedSize.width / 2 + pushSize.width / 2;
    if (onMovingPlatform) {
      h += 1;
    }

    float oldYPos = pushObject->getPosition().y;

    if (phyPosPush.x < phyPosPushed.x) {
      if (pushObject == mHero) {
        mHero->mPushRightFlag = true;
      }
      pushObject->setPosition(phyPosPushed.x - h, oldYPos);
    } else {
      if (pushObject == mHero) {
        mHero->mPushLeftFlag = true;
      }
      pushObject->setPosition(phyPosPushed.x + h, oldYPos);
    }
  }

  if (normal.y > 0.9 || normal.y < -0.9) {
    if (pushObject == mHero && normal.y > 0.9) {
      mHero->mCanJump = true;
    }

    auto h = pushedSize.height / 2 + pushSize.height / 2;
    if (onMovingPlatform && pushObject == mHero) {
      h -= 1;
    }

    if (normal.y < -0.9) {
      h += onButton ? -1 : 1;
      if (onMovingPlatform && pushObject == mHero) {
        mHero->mJumpVelocity.y = -100;
      }
      pushObject->setPositionY(phyPosPushed.y - h);
    } else {
      pushObject->setPositionY(phyPosPushed.y + h);
      if (pushObject == mHero) {
        mHero->mLinkingID = pushedObject->mID;
      }
    }
    pushObject->mVelocity.y = 0;
    pushObject->forceUpdatePhysicsPosition();
  }

  pushObject->getRenderer()->getPhysicsBody()->getShapes().at(0)->_forceUpdateShape();
  return false;
}

void GameLogic::showGameScene(bool val) {
#if USE_BACKGROUND
  mBack->setVisible(val);
#endif
}

void GameLogic::createFixedBlocks() {
  auto width = VisibleRect::right().x;
  auto height = VisibleRect::top().y;
  {
    // Bottom
    BlockBase *block = new BlockBase();
    block->mCanDelete = false;
    block->setRestoreRect(Rect(width / 2, -BORDER_FRAME_SIZE / 2, width, BORDER_FRAME_SIZE));
    block->setKind(BlockKind::KIND_BLOCK);
    block->mCastShadow = false;
    mBlocks[block->mID] = block;
  }
  {
    // Top
    BlockBase *block = new BlockBase();
    block->mCanDelete = false;
    block->setRestoreRect(Rect(width / 2, height + BORDER_FRAME_SIZE / 2, width, BORDER_FRAME_SIZE));
    block->setKind(BlockKind::KIND_BLOCK);
    block->mCastShadow = false;
    mBlocks[block->mID] = block;
  }
  {
    // Left
    BlockBase *block = new BlockBase();
    block->mCanDelete = false;
    block->setRestoreRect(Rect(-BORDER_FRAME_SIZE / 2, height / 2,
                       BORDER_FRAME_SIZE, height + BORDER_FRAME_SIZE * 2));
    block->setKind(BlockKind::KIND_BLOCK);
    block->mCastShadow = false;
    mBlocks[block->mID] = block;
  }
  {
    // Right
    BlockBase *block = new BlockBase();
    block->mCanDelete = false;
    block->setRestoreRect(Rect(width + BORDER_FRAME_SIZE / 2, height / 2,
                       BORDER_FRAME_SIZE, height + BORDER_FRAME_SIZE * 2));
    block->setKind(BlockKind::KIND_BLOCK);
    block->mCastShadow = false;
    mBlocks[block->mID] = block;
  }
}

BlockBase *GameLogic::findBlock(int id) {
  auto it = mBlocks.find(id);
  if (it != mBlocks.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

void GameLogic::jump() {
  if (mHero->mCanJump && !mRejectInput) {
    mHero->mJumpVelocity.y += JUMP_VOL;
#if USE_JUMP_EFFECT
    float scale = mHero->getRenderer()->getScaleX();
    auto node = mHero->getRenderer()->getNode();
    if(node->getNumberOfRunningActions() == 0) {
      node->runAction(Sequence::create(ScaleTo::create(0.2, scale*0.6, scale*1.4),
                                                     ScaleTo::create(0.2, scale, scale), NULL));
    }
#endif
    mHero->mCanJump = false;
  }
  mJumpFlag = false;
}

void GameLogic::win() {
  CCLOG("Win event triggered");
  showWinCurtain();
}

void GameLogic::die() {
  if (mParentLayer->getChildByTag(DIE_FX_TAG)) {
    mParentLayer->removeChildByTag(DIE_FX_TAG);
  }

  mHero->mRestorePosition = mSpawnPos;
  mHero->setPosition(mSpawnPos);
  mHero->getRenderer()->setOpacity(255);
  mDeadFlag = false;
}

void GameLogic::postUpdate(float dt) {
  if (mGameMode) {
    for (auto b : mBlocks) {
      b.second->postUpdate(dt);
    }
    mHero->postUpdate(dt);
  }
}

void GameLogic::updateGame(float dt) {
  if (mDeadFlag && !mRejectInput) {
    // Play dead effect
    ParticleSystem *m_emitter0 = ParticleSystemQuad::create("fx/diefx.plist");
    ParticleBatchNode *batch0 = ParticleBatchNode::createWithTexture(m_emitter0->getTexture());
    batch0->addChild(m_emitter0);
    batch0->setPosition(mHero->getPosition());
    batch0->setCameraMask((unsigned short) CameraFlag::USER2);
    mParentLayer->addChild(batch0, ZORDER_DIE_FX, DIE_FX_TAG);

    mRejectInput = true;
    mHero->getRenderer()->getNode()->runAction(Sequence::create(ScaleTo::create(0.2, 0.1, 0.1),
                                                   CallFunc::create([this] {
                                                       mHero->getRenderer()->setVisible(false);
                                                   }), NULL));

    mParentLayer->runAction(Sequence::create(DelayTime::create(0.4), CallFunc::create([this] {
#if EDITOR_MODE
        EditorScene::Scene->showDieFullScreenAnim();
#else
        GameScene::Scene->showDieFullScreenAnim();
#endif
        enableGame(true, true);
    }), NULL));

    return;
  }
  if (mWinFlag) {
    win();
    return;
  }

  mGameTimer += dt;

  if (mJumpFlag) {
    jump();
  }

  if (!mMoveLeft && !mMoveRight) {
    mHero->mCurrentMovingSpeed = 0;
  }

  if (!mRejectInput) {
    float speed = mHero->mPushing ? 80 : 200;
    mHero->mCurrentMovingSpeed += dt * 800;
    mHero->mCurrentMovingSpeed = std::min(speed, mHero->mCurrentMovingSpeed);
    if (mMoveLeft && !mHero->mPushLeftFlag) {
      mHero->moveX(dt * -mHero->mCurrentMovingSpeed);
    } else if (mMoveRight && !mHero->mPushRightFlag) {
      mHero->moveX(dt * mHero->mCurrentMovingSpeed);
    }
  }

  mHero->mPushing = false;
  mHero->mCanJump = false;
  mHero->mPushLeftFlag = false;
  mHero->mPushRightFlag = false;
}

void GameLogic::update(float dt) {
  if (mGameMode) {
    for (auto it = mTimeEvents.begin(); it != mTimeEvents.end(); ++it) {
      (*it).update(dt);
    }

    for (auto b : mBlocks) {
      b.second->preUpdate();
    }
    mHero->preUpdate();

    for (auto b : mBlocks) {
      b.second->update(dt);
    }
    mHero->update(dt);

    for (auto b : mBlocks) {
      b.second->updateMovement(dt);
    }
    mHero->updateMovement(dt);

    updateGame(dt);
  }

#if USE_SHADOW
  for (auto &s : mShadows) {
    s->update(dt);
  }
#endif
}

void GameLogic::deleteBlock(BlockBase *sel) {
  // Find it in groups
  for (auto &i : mGroups) {
    auto ii = std::find(i.second.begin(), i.second.end(), sel);
    if (ii != i.second.end()) {
      i.second.erase(ii);
    }
  }

  auto itg = mGroups.find(sel);
  if (itg != mGroups.end()) {
    mGroups.erase(itg);
  }

  auto it = mBlocks.begin();
  for (; it != mBlocks.end(); ++it)
    if (it->second == sel)
      break;

  if (it != mBlocks.end()) {
    auto tableit = mBlockTable.find(sel->getRenderer()->getNode());
    mBlockTable.erase(tableit);
    if (sel == mSelectionHead)
      mSelectionHead = nullptr;
    delete sel;
    mBlocks.erase(it);
  }

}

void GameLogic::enableGame(bool val, bool force) {
  if (mGameMode == val && !force) {
    return;
  }

  if (val) {
    mRejectInput = false;
  }

  mGameMode = val;

  mMoveLeft = false;
  mMoveRight = false;

#if USE_SHADOW
  for (auto& s : mShadows) {
    s->reset();
  }
#endif

  die();

  mHero->setVisible(mGameMode);
  mHero->mCanJump = false;
  mHero->setSize(mHero->mRestoreSize);
  mHero->reset();

  for (auto it = mTimeEvents.begin(); it != mTimeEvents.end(); ++it) {
    (*it).reset();
  }

  for (auto bc : mBlocks) {
    auto b = bc.second;
    if (val) {
      b->mPath.reset();
      b->mRotator.reset();
    }
    b->mRotator.mDisable = !val;
    b->mPath.mDisable = !val;
#if EDITOR_MODE
    b->mPath.mHelperNode->setVisible(!val);
    if (b->mButton) {
      b->mButton->showHelper(!val);
    }
#endif
    b->reset();
  }

  for (int i = 1; i <= 4; ++i) {
    if (mBlocks[i]) {
      mBlocks[i]->getRenderer()->setVisible(!val);
    }
  }
  mGameTimer = 0;
}

void GameLogic::setBackgroundColor(const cocos2d::Color3B &color) {
  mBackgroundColor = color;
#if USE_BACKGROUND
  mBack->setColor(mBackgroundColor);
#endif
}

BlockBase *GameLogic::createBlock(const cocos2d::Vec2 &pos, BlockKind kind) {
  BlockBase *block = new BlockBase();
  block->mRestorePosition = pos;
  block->mRestoreSize.setSize(100, 15);
  block->setKind(kind, true);
  mBlocks[block->mID] = block;
  return block;
}

void GameLogic::clearStars() {
#if EDITOR_MODE
  for(auto f : mStarNodes) {
    f->removeFromParent();
  }
#endif
  mStarNodes.clear();
  mStarList.clear();
}

void GameLogic::clearFx() {
#if EDITOR_MODE
  for (auto f : mFxNodes) {
    f->removeFromParent();
  }
#endif
  mFxNodes.clear();
  mFxList.clear();
}

void GameLogic::clearSprites() {
  for (auto &p : mSpriteList) {
    p.clean();
  }
  mSpriteList.clear();
}

void GameLogic::clean() {
  mTimeEvents.clear();
  mGroups.clear();

  for (auto &b : mBlocks) {
    CC_SAFE_DELETE(b.second);
  }
  mBlocks.clear();

  cleanBackground();
  clearStars();
  clearFx();
  clearSprites();

  enableGame(false);
  BlockBase::mIDCounter = 1;
  
#if USE_SHADOW
  for (auto &s : mShadows) {
    CC_SAFE_DELETE(s);
  }
  
#if EDITOR_MODE
  for (auto &n : mShadowNode) {
    n->removeFromParent();
  }
#endif
  
  mShadowNode.clear();
  mShadows.clear();
  mNumShadowGroup = 0;
#endif
}

void GameLogic::blockTraversal(const std::function<void(BlockBase *)> &func) {
  for (auto b : mBlocks) {
    func(b.second);
  }
}

void GameLogic::loadSpritesFromList() {
  for (auto &p : mSpriteList) {
    p.create();
    mParentLayer->addChild(p.getSprite(), p.ZOrder);
  }
}

void GameLogic::loadStarFromList() {
  for (auto f : mStarNodes) {
    f->removeFromParent();
  }
  mStarNodes.clear();

  for (auto p : mStarList) {
    mStarNodes.push_back(createParticle(p));
  }
}

void GameLogic::loadFxFromList() {
  for (auto f : mFxNodes) {
    f->removeFromParent();
  }
  mFxNodes.clear();

  for (auto i : mFxList) {
    ParticleSystem *ps = ParticleSystemQuad::create(i);
    ParticleBatchNode *batch = ParticleBatchNode::createWithTexture(ps->getTexture());
    batch->addChild(ps);
    mParentLayer->addChild(batch, ZORDER_PARTICLE);
    batch->setCameraMask((unsigned short) CameraFlag::USER2);
    mFxNodes.push_back(batch);
  }
}

void GameLogic::updateCamera(cocos2d::Camera *cam, bool forceUpdate) {
  if (!mGameMode && !forceUpdate) {
    return;
  }
  auto halfFrame = VisibleRect::getFrameSize() / 2;
  auto heroRel = mHero->getPosition() - halfFrame;
  auto newPos = Vec2(halfFrame) + heroRel;
#if EDITOR_MODE
  newPos.x = std::max(newPos.x, halfFrame.width);
  newPos.x = std::min(newPos.x, mBounds.size.width - halfFrame.width);
  newPos.y = std::max(newPos.y, halfFrame.height);
  newPos.y = std::min(newPos.y, mBounds.size.height + UI_LAYER_HIGHT - halfFrame.height);
  auto camRelative = newPos - VisibleRect::getVisibleRect().size / 2 - Vec2(0, UI_LAYER_HIGHT / 2);
#else
  auto center = VisibleRect::center();
  newPos.x = std::max(newPos.x, center.x);
  newPos.x = std::min(newPos.x, mBounds.size.width - center.x);
  newPos.y = std::max(newPos.y, center.y);
  newPos.y = std::min(newPos.y, mBounds.size.height - center.y);
  auto camRelative = newPos - VisibleRect::getVisibleRect().size / 2;
#endif
  
#if USE_BACKGROUND
  mBack->setPosition(VisibleRect::center() + camRelative);
#endif
  
#if EDITOR_MODE
  EditorScene::Scene->mGridNode->setPosition(camRelative);
#endif
  cam->setPosition(newPos);
}

void GameLogic::updateBounds() {
  float halfBorderSize = BORDER_FRAME_SIZE / 2;
  float left = mBlocks[BORDER_BLOCK_LEFT]->getPosition().x + halfBorderSize;
  float right = mBlocks[BORDER_BLOCK_RIGHT]->getPosition().x - halfBorderSize;
  float top = mBlocks[BORDER_BLOCK_TOP]->getPosition().y - halfBorderSize;
  float bottom = mBlocks[BORDER_BLOCK_BOTTOM]->getPosition().y + halfBorderSize;
  mBounds = Rect(left, bottom, right - left, top - bottom);
}

void GameLogic::restoreBackgroundPos() {
  auto visRect = VisibleRect::getVisibleRect();
#if USE_BACKGROUND
  mBack->setPosition(visRect.size.width / 2, visRect.size.height / 2);
#endif
  
#if EDITOR_MODE
  if (EditorScene::Scene->mGridNode) {
    EditorScene::Scene->mGridNode->setPosition(0, 0);
  }
#endif
}

#if !EDITOR_MODE
void GameLogic::showBeginCurtain() {
  updateCamera(GAME_CAMERA, true);
  initCurtainPos();

  GAME_CAMERA->setPositionY(GAME_CAMERA->getPositionY() - VIS_RECT_HEIGHT);
  GAME_CAMERA->runAction(Sequence::create(MoveBy::create(CURTAIN_MOVE_TIME,
                                                         Vec2(0, VIS_RECT_HEIGHT)),
                                          CallFunc::create([this]() {
    enableGame(true, true);
  }), NULL));
}
#endif

void GameLogic::showWinCurtain() {
  initCurtainPos();
  mGameMode = false;

#if !EDITOR_MODE
  if (GameScene::Scene->getTimerLabel()->getPositionY() < VisibleRect::top().y) {
    GameScene::Scene->showHideMenu(true);
  }
#endif

  GAME_CAMERA->runAction(Sequence::create(MoveBy::create(CURTAIN_MOVE_TIME,
                                                         Vec2(0, -VIS_RECT_HEIGHT)),
                                          CallFunc::create([this]() {
    mWinFlag = false;
    if (mWinGameEvent) {
      mWinGameEvent();
    }
  }), NULL));
}

void GameLogic::updateHeroSpawnPos() {
  GameLogic::Game->mHero->mRestorePosition = mSpawnPos;
  GameLogic::Game->mHero->setPosition(mSpawnPos);
}

void GameLogic::initCurtainPos() {
  float screenHeight = VisibleRect::getVisibleRect().size.height;
  float screenWidth = VisibleRect::getVisibleRect().size.width;
  auto color = Palette::getInstance()->getDefaultBlockColors(KIND_BLOCK);
  auto camPos = GAME_CAMERA->getPosition();

  if (mCurtain) {
    mCurtain->setVisible(true);
    mCurtain->clear();
    mCurtain->drawSolidRect(Vec2(camPos.x - screenWidth / 2, camPos.y - screenHeight * 1.5f),
                            Vec2(camPos.x + screenWidth / 2, camPos.y - screenHeight / 2),
                            Color4F(color));
  } else {
    mCurtain = DrawNode::create();
    mCurtain->drawSolidRect(Vec2(camPos.x - screenWidth / 2, camPos.y - screenHeight * 1.5f),
                            Vec2(camPos.x + screenWidth / 2, camPos.y - screenHeight / 2),
                            Color4F(color));
    mCurtain->setCameraMask((unsigned short) CameraFlag::USER2);
    mParentLayer->addChild(mCurtain, ZORDER_CURTAIN);
  }
}
