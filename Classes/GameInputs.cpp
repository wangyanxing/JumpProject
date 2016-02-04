//
//  GameInputs.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/18/16.
//
//

#include "GameInputs.h"
#include "GameLevel.h"
#include "GameLayerContainer.h"

USING_NS_CC;

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

void GameInputs::setLastMousePosition(const cocos2d::Vec2 &mousePos) {
  mLastMousePosition = mousePos;
}

MouseEvent GameInputs::convertMouseEvent(EventMouse *mouse) {
  auto mousePoint = Vec2(mouse->getCursorX(), mouse->getCursorY());
  auto visRect = Director::getInstance()->getOpenGLView()->getVisibleRect();
  auto height = visRect.origin.y + visRect.size.height;
  auto gameLayer = GameLevel::instance().getGameLayer();

  MouseEvent ret;
  ret.button = mouse->getMouseButton();
  ret.movement = mouse->getLocationInView() - mLastMousePosition;
  ret.posInScreen = mousePoint;
  ret.posInScreen.y += height;
  ret.posInScreen = gameLayer->convertToNodeSpace(ret.posInScreen);
  ret.posInMap = ret.posInScreen;
  ret.posInMap += gameLayer->getCamera()->getPosition() - visRect.size / 2;
  return ret;
}
