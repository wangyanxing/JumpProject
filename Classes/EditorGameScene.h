//
//  EditorGameScene.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#ifndef EditorGameScene_h
#define EditorGameScene_h

#include "Prerequisites.h"
#include "GameLayerContainer.h"

#if USE_REFACTOR

class EditorGameScene : public GameLayerContainer {
public:
  EditorGameScene();

  ~EditorGameScene();

public:
  CREATE_FUNC(EditorGameScene);

  bool init() override;

  void onEnter() override;

  void update(float dt) override;

  void postUpdate(float dt) override;

  void processInput();

  void clean() override;

  void onWinGame() override;

  int8_t getCameraDepth() override {
    return 0;
  }

  void keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);

  void keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
};

#endif

#endif /* EditorGameScene_h */
