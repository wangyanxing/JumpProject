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

void GameLevel::init() {
  CC_ASSERT(!mObjectManager);
  mObjectManager = new ObjectManager();
  mPhysicsManager = new PhysicsManager();
}

void GameLevel::release() {
  CC_SAFE_DELETE(mObjectManager);
  CC_SAFE_DELETE(mPhysicsManager);
}

void GameLevel::update(float dt) {
  for (auto &obj : mObjectManager->mObjects) {
    obj.second->update(dt);
  }
  mPhysicsManager->update(dt);
}
