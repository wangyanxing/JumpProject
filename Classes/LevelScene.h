//
//  LevelScene.h
//  JumpEdt
//
//  Created by Yanxing Wang.
//
//

#ifndef __JumpEdt__LevelScene__
#define __JumpEdt__LevelScene__

#include "Prerequisites.h"
#include "ui/CocosGUI.h"

#if !EDITOR_MODE

class LevelScene : public cocos2d::Scene {
public:
  virtual bool init();

  virtual void onEnter();

  CREATE_FUNC(LevelScene);

  static LevelScene *getInstance();

  void touchEvent(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType type);

private:
  static LevelScene *instance;
};

#endif
#endif
