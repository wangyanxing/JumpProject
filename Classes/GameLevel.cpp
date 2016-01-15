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

void GameLevel::init() {
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
  mPhysicsManager->update(dt);
}

void GameLevel::load(const std::string &levelFile) {
  JsonParser parser(levelFile);
  if (!parser) {
    CCLOGERROR("Cannot load the level file: %s", levelFile.c_str());
  }

  CCLOG("Loading level file: %s", levelFile.c_str());
  auto& doc = parser.getCurrentDocument();

  std::string paletteFile = doc[LEVEL_PALETTE_FILE].GetString();
  CC_SAFE_DELETE(mPalette);
  mPalette = new ColorPalette(paletteFile);

  auto spawnPos = doc[LEVEL_SPAWN_POS].GetVec2();

  parser.parseArray(doc, LEVEL_BLOCK_ARRAY, [&](JsonSizeT i, JsonValueT& val) {
    mObjectManager->createObject(val);
  });
}

void GameLevel::unload() {

}
