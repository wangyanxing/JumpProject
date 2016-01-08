//
//  LogicManager.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__LogicManager__
#define __JumpEdt__LogicManager__

#include "cocos2d.h"
#include "Defines.h"
#include "SceneSprite.h"

class BlockBase;
class Hero;
class ShadowManager;
class LightBeam;
class TimeEvent;
class BlockRenderer;
class GameLayerContainer;

class GameLogic {
public:
  static GameLogic *Game;

  static cocos2d::PhysicsWorld *PhysicsWorld;

  GameLogic(GameLayerContainer *parent);

  ~GameLogic();

  BlockBase *createBlock(const cocos2d::Vec2 &pos, BlockKind kind);

  void showGameScene(bool val);

  void blockTraversal(const std::function<void(BlockBase *)> &func);

  void deleteBlock(BlockBase *b);

  bool onContactPreSolve(cocos2d::PhysicsContact &contact, cocos2d::PhysicsContactPreSolve &solve);

  void update(float dt);

  void clean();

  void enableGame(bool val, bool force = false);

  void postUpdate(float dt);

  void updateGame(float dt);

  void setBackgroundColor(const cocos2d::Color3B &color);

  void updateHeroSpawnPos();

  BlockBase *findBlock(int id);

  void jump();

  void die();

  void win();

  cocos2d::Node *createParticle(const cocos2d::Vec2 &pos);

  void loadFxFromList();

  void loadStarFromList();
  
  void loadSpritesFromList();

  void clearFx();

  void clearStars();
  
  void clearSprites();

  void updateCamera(cocos2d::Camera *cam, bool forceUpdate = false);

  void updateBounds();

  void restoreBackgroundPos();
  
  void initBackground();
  
  void cleanBackground();
  
  void showWinCurtain();

#if !EDITOR_MODE
  void showBeginCurtain();
#endif

  GameLayerContainer *getGameLayer() {
    return mParentLayer;
  }

public:
  std::map<BlockBase *, std::vector<BlockBase *>> mGroups;

  Hero *mHero{nullptr};

  cocos2d::PhysicsShape *mHeroShape{nullptr};

  std::map<int, BlockBase *> mBlocks;

  std::set<BlockBase *> mSelections;

  bool mMoveLeft{false};

  bool mMoveRight{false};

  BlockBase *mSelectionHead{nullptr};

  std::map<cocos2d::Node *, BlockBase *> mBlockTable;

#if USE_SHADOW
  int mNumShadowGroup{0};
  
  std::vector<ShadowManager*> mShadows;

  std::vector<cocos2d::Node*> mShadowNode;
  
  void addShadowGroup();
  
  void initShadowGroup(int groupId);
#endif

  cocos2d::Color3B mBackgroundColor;

#if USE_BACKGROUND
  cocos2d::Sprite *mBack{nullptr};
#endif

  bool mDeadFlag{false};

  bool mWinFlag{false};

  bool mGameMode{false};

  cocos2d::Vec2 mSpawnPos;

  float mGameTimer{0};

  std::function<void()> mWinGameEvent;

  std::vector <TimeEvent> mTimeEvents;

  std::vector <std::string> mFxList;

  std::vector <cocos2d::ParticleBatchNode *> mFxNodes;

  std::vector <cocos2d::Vec2> mStarList;
  
  std::vector <SceneSprite> mSpriteList;

  std::vector<cocos2d::Node *> mStarNodes;

  bool mRejectInput{false};

  bool mJumpFlag{false};

  cocos2d::Rect mBounds;
  
  cocos2d::DrawNode* mCurtain{nullptr};

private:
  void initCurtainPos();

  GameLayerContainer *mParentLayer{nullptr};
};

#endif /* defined(__JumpEdt__LogicManager__) */
