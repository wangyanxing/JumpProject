//
//  InputManager.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/18/16.
//
//

#ifndef GameInputs_h
#define GameInputs_h

#include "Prerequisites.h"
#include "Singleton.h"

class GameInputs {
public:
  DECL_SIMPLE_SINGLETON(GameInputs);

  typedef cocos2d::EventKeyboard::KeyCode KeyCode;

  void keyPressed(KeyCode key, cocos2d::Event *event);

  void keyReleased(KeyCode key, cocos2d::Event *event);

  bool isPressing(KeyCode key) {
    return mPressingKeys.count(key);
  }

private:
  std::set<KeyCode> mPressingKeys;
};

#endif /* GameInputs_h */
