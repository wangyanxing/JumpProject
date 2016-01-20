//
//  GameLevel.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "GameLevel.h"
#include "ObjectManager.h"
#include "PhysicsManager.h"
#include "GameObject.h"
#include "ColorPalette.h"
#include "JsonParser.h"
#include "JsonFormat.h"
#include "ColorPalette.h"
#include "GameLayerContainer.h"
#include "GameConfig.h"

USING_NS_CC;

void GameLevel::init() {
  GameConfig::instance().load();
  
  CC_ASSERT(!mObjectManager);
  mObjectManager = new ObjectManager();
  mPhysicsManager = new PhysicsManager();
}

void GameLevel::release() {
  CC_SAFE_DELETE(mObjectManager);
  CC_SAFE_DELETE(mPhysicsManager);
  CC_SAFE_DELETE(mPalette);
}

void GameLevel::update(float dt) {
  for (auto &obj : mObjectManager->mObjects) {
    obj.second->update(dt);
  }

  // Collision detection.
  mPhysicsManager->update(dt);
}

void GameLevel::beforeRender(float dt) {
  mPhysicsManager->beforeRender(dt);
  for (auto &obj : mObjectManager->mObjects) {
    obj.second->beforeRender(dt);
  }
}

GameObject *GameLevel::getHero() {
  auto hero = getObjectManager()->getObjectByID(0);
  CC_ASSERT(hero && hero->getKind() == KIND_HERO);
  return hero;
}

void GameLevel::createHero(const cocos2d::Vec2 &pos) {
  Parameter param;
  param.set(PARAM_BLOCK_KIND, KIND_HERO)
       .set(PARAM_POS, pos)
       .set(PARAM_SIZE, Size(GameConfig::instance().HeroSize, GameConfig::instance().HeroSize));

  auto hero = getObjectManager()->createObject(param);
  CC_ASSERT(hero->getID() == 0);
}

void GameLevel::load(const std::string &levelFile) {
  unload();
  
  JsonParser parser(levelFile);
  if (!parser) {
    CCLOGERROR("Cannot load the level file: %s", levelFile.c_str());
  }

  CCLOG("Loading level file: %s", levelFile.c_str());
  auto& doc = parser.getCurrentDocument();

  std::string paletteFile = doc[LEVEL_PALETTE_FILE].GetString();
  CC_SAFE_DELETE(mPalette);
  mPalette = new ColorPalette(paletteFile);

  mGameLayer->setColor(mPalette->getBackgroundColor());

  auto spawnPos = doc[LEVEL_SPAWN_POS].GetVec2();
  createHero(spawnPos);

  parser.parseArray(doc, LEVEL_BLOCK_ARRAY, [&](JsonSizeT i, JsonValueT& val) {
    mObjectManager->createObject(val);
  });

  enableGame(true);
}

void GameLevel::unload() {
  mObjectManager->cleanUp();
}

void GameLevel::enableGame(bool enable) {
  for (auto &obj : mObjectManager->mObjects) {
    obj.second->setEnabled(enable);
  }
}
