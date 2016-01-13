//
//  GameLevel.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "GameLevel.h"
#include "ObjectManager.h"

void GameLevel::init() {
  CC_ASSERT(!mObjectManager);
  mObjectManager = new ObjectManager(this);
}

void GameLevel::release() {
  CC_SAFE_DELETE(mObjectManager);
}

void GameLevel::update(float dt) {

}