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
#include "LogicManager.h"
#include "ShaderLayer.h"

class GameLogic;
class BlockBase;

class GameScene : public ShaderLayer {
public:
  GameScene() = default;

  ~GameScene();

  static GameScene* Scene;

  static cocos2d::Scene* createScene();

  struct PostUpdater {
    void update(float dt) {
      GameLogic::Game->postUpdate(dt);
    }
  };

  virtual void onEnter() override;

  virtual void onEnterTransitionDidFinish() override;

  virtual bool init();

  void update(float dt);

  bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);

  void enableGame(bool v);

  void loadChooseLevel(const std::string& name);

  void enterGame(const std::string& name, bool absPath);

  void showDieFullScreenAnim();

  CREATE_FUNC(GameScene);

private:

  void onTouch(const cocos2d::Vec2& pos);
  void onEndTouch(const cocos2d::Vec2& pos);

  void createControlPad();
  void createMenuButtons();

  void toMainMenu();

  void onWinGame();

  void showHideMenu(bool force = false);

private:

  GameLogic* mGame{ nullptr };

  PostUpdater mPostUpdater;

  cocos2d::Sprite* mLeftButton{ nullptr };
  cocos2d::Sprite* mRightButton{ nullptr };
  cocos2d::Sprite* mJumpButton{ nullptr };

  cocos2d::Label* mTimerLabel{ nullptr };
  cocos2d::MenuItemImage* mBackMenu{ nullptr };
  cocos2d::MenuItemImage* mRestartMenu{ nullptr };

  bool mCanJump{ true };
};

#endif /* defined(__JumpEdt__GameScene__) */
