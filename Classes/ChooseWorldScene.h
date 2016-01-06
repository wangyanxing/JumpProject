//
//  ChooseWorldScene.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/5/16.
//
//

#ifndef ChooseWorldScene_h
#define ChooseWorldScene_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Defines.h"
#include "GameLayerContainer.h"

#if !EDITOR_MODE

class GameLogic;

class ChooseWorldScene : public GameLayerContainer {
public:
  ChooseWorldScene(){}

  virtual bool init() override;

  CREATE_FUNC(ChooseWorldScene);

  static ChooseWorldScene *getInstance();

  void touchEvent(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType type);

  void update(float dt) override;

  void onWinGame() override {}

private:
  static ChooseWorldScene *instance;
};

#endif

#endif /* ChooseWorldScene_h */
