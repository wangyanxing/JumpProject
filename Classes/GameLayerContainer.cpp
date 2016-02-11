//
//  GameLayerContainer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/5/16.
//
//

#include "GameLayerContainer.h"
#include "LogicManager.h"
#include "GameLevel.h"

USING_NS_CC;

GamePostUpdater::GamePostUpdater(GameLayerContainer *ct): container(ct) {
}

void GamePostUpdater::update(float dt) {
  container->postUpdate(dt);
}

cocos2d::Scene *GameLayerContainer::createPhysicsScene() {
  auto scene = Scene::createWithPhysics();
  scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
  GameLogic::PhysicsWorld = scene->getPhysicsWorld();
  return scene;
}

GameLayerContainer::GameLayerContainer() : mPostUpdater(this) {
}

GameLayerContainer::~GameLayerContainer() {
  getScheduler()->unscheduleAllForTarget(&mPostUpdater);

#if !USE_REFACTOR
  CC_SAFE_DELETE(mGame);
#endif
}

bool GameLayerContainer::init() {
  Layer::init();

  mBlockRoot = Node::create();
  mBlockRoot->setCameraMask((unsigned short) CameraFlag::USER2);
  mCamera = Camera::create();

#if !USE_REFACTOR
  CC_SAFE_DELETE(GameLogic::Game);
  mGame = new GameLogic(this);
  mGame->mWinGameEvent = [this] { onWinGame(); };
#endif

  mCamera->setCameraFlag(CameraFlag::USER2);
  mCamera->setDepth(getCameraDepth());
  addChild(mCamera);
  addChild(mBlockRoot, ZORDER_BLOCK_ROOT);
  setCameraMask((unsigned short) CameraFlag::USER2);
  return true;
}

void GameLayerContainer::onEnter() {
  Layer::onEnter();
  Director::getInstance()->getScheduler()->scheduleUpdate(this, -10, false);

#if USE_REFACTOR
  Director::getInstance()->getScheduler()->scheduleUpdate(&mPostUpdater, -5, false);
#else
  Director::getInstance()->getScheduler()->scheduleUpdate(&mPostUpdater, 100, false);
#endif

#if !USE_REFACTOR
  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactPreSolve = CC_CALLBACK_2(GameLayerContainer::onCollisionDetected, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
#endif
}

#if !USE_REFACTOR
bool GameLayerContainer::onCollisionDetected(cocos2d::PhysicsContact &contact,
                                             cocos2d::PhysicsContactPreSolve &solve) {
  return mGame->onContactPreSolve(contact, solve);
}
#endif

void GameLayerContainer::update(float dt) {
#if !USE_REFACTOR
  mGame->update(UPDATE_DT);
  mGame->updateCamera(mCamera);
#else
  GameLevel::instance().update(UPDATE_DT);
  GameLevel::instance().updateCamera(mCamera);
#endif
}

void GameLayerContainer::postUpdate(float dt) {
#if !USE_REFACTOR
  mGame->postUpdate(UPDATE_DT);
#else
  GameLevel::instance().beforeRender(UPDATE_DT);
#endif
}

void GameLayerContainer::clean() {
#if !USE_REFACTOR
  mGame->clean();
#endif
}
