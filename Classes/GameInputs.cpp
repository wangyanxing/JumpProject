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

  auto it = mKeyboardEvents.find(key);
  if (it != mKeyboardEvents.end()) {
    it->second(key);
  }
}

void GameInputs::keyReleased(KeyCode key, cocos2d::Event *event) {
  mPressingKeys.erase(key);
}

void GameInputs::addKeyboardEvent(KeyCode key, KeyboardFunc callback) {
  mKeyboardEvents[key] = callback;
}

void GameInputs::removeKeyboardEvent(KeyCode key) {
  mKeyboardEvents.erase(key);
}
