//
//  GameScene.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/27/14.
//
//

#ifndef __JumpEdt__GameScene__
#define __JumpEdt__GameScene__

#include "cocos2d.h"
#include "Defines.h"
#include "LogicManager.h"
#include "GameLayerContainer.h"

#if !EDITOR_MODE

class GameLogic;
class BlockBase;

class GameScene : public GameLayerContainer {
public:
  GameScene();

  virtual ~GameScene();

  virtual void onEnter() override;

  virtual void onEnterTransitionDidFinish() override;

  virtual bool init() override;

  void update(float dt) override;

  void enableGame(bool v);

  void loadChooseLevel(const std::string &name);

  void enterGame(const std::string &name, bool absPath);

  CREATE_FUNC(GameScene);

private:
  void onTouch(const cocos2d::Vec2 &pos);

  void onEndTouch(const cocos2d::Vec2 &pos);

  void createControlPad();

  void createMenuButtons();

  void toMainMenu();

  void onWinGame() override;

  void preWinGame() override;

  void showHideMenu(bool force = false);

private:
  cocos2d::Sprite *mLeftButton{nullptr};

  cocos2d::Sprite *mRightButton{nullptr};

  cocos2d::Sprite *mJumpButton{nullptr};

  cocos2d::Label *mTimerLabel{nullptr};

  cocos2d::MenuItemImage *mBackMenu{nullptr};

  cocos2d::MenuItemImage *mRestartMenu{nullptr};

  bool mCanJump{true};
};

#endif

#endif /* defined(__JumpEdt__GameScene__) */
