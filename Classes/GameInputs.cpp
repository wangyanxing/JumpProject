//
//  GameInputs.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/18/16.
//
//

#include "GameInputs.h"

void GameInputs::keyPressed(KeyCode key, cocos2d::Event *event) {
  mPressingKeys.insert(key);
}

void GameInputs::keyReleased(KeyCode key, cocos2d::Event *event) {
  mPressingKeys.erase(key);
}
