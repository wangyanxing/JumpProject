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

class GameLogic {
public:
  static GameLogic *Game;

  static cocos2d::PhysicsWorld *PhysicsWorld;

  GameLogic(cocos2d::Layer *parent);

  ~GameLogic();

  BlockBase *createBlock(const cocos2d::Vec2 &pos, BlockKind kind);

  void showGameScene(bool val);

  void blockTraversal(const std::function<void(BlockBase *)> &func);

  void createFixedBlocks();

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

  void setBackGradientCenter(const cocos2d::Vec2 &p);

  void setBackGradientColor(const cocos2d::Color3B &colorSrc,
                            const cocos2d::Color3B &colorDst);

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
  
  void showWinCurtain();

#if !EDITOR_MODE
  void showBeginCurtain();
#endif

public:
  cocos2d::Layer *mParentLayer{nullptr};

  std::map<BlockBase *, std::vector < BlockBase * >> mGroups;

  Hero *mHero{nullptr};

  cocos2d::PhysicsShape *mHeroShape{nullptr};

  std::map<int, BlockBase *> mBlocks;

  std::set<BlockBase *> mSelections;

  bool mMoveLeft{false};

  bool mMoveRight{false};

  BlockBase *mSelectionHead{nullptr};

  std::map<cocos2d::Node *, BlockBase *> mBlockTable;

#if USE_SHADOW
  ShadowManager *mShadows{nullptr};

  cocos2d::Node *mShadowNode{nullptr};
#endif

  cocos2d::Color3B mBackgroundColor;

  cocos2d::Sprite *mBack{nullptr};

  bool mDeadFlag{false};

  bool mWinFlag{false};

  bool mGameMode{false};

  cocos2d::Vec2 mSpawnPos;

  cocos2d::Vec2 mGradientCenter{0, 0};

  cocos2d::Color3B mGradientColorSrc;

  cocos2d::Color3B mGradientColorDst;

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
};

#endif /* defined(__JumpEdt__LogicManager__) */
