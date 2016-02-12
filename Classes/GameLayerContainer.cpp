//
//  GameLayerContainer.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/5/16.
//
//

#include "GameLayerContainer.h"
#include "GameLevel.h"

USING_NS_CC;

GamePostUpdater::GamePostUpdater(GameLayerContainer *ct): container(ct) {
}

void GamePostUpdater::update(float dt) {
  container->postUpdate(dt);
}

GameLayerContainer::GameLayerContainer() : mPostUpdater(this) {
}

GameLayerContainer::~GameLayerContainer() {
  getScheduler()->unscheduleAllForTarget(&mPostUpdater);
}

bool GameLayerContainer::init() {
  Layer::init();

  mBlockRoot = Node::create();
  mBlockRoot->setCameraMask((unsigned short) CameraFlag::USER2);
  mCamera = Camera::create();

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
  Director::getInstance()->getScheduler()->scheduleUpdate(&mPostUpdater, -5, false);
}

void GameLayerContainer::update(float dt) {
  GameLevel::instance().update(UPDATE_DT);
  GameLevel::instance().updateCamera(mCamera);
}

void GameLayerContainer::postUpdate(float dt) {
  GameLevel::instance().beforeRender(UPDATE_DT);
}

void GameLayerContainer::clean() {
}
