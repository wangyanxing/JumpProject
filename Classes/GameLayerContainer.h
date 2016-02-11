//
//  GameLayerContainer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/5/16.
//
//

#ifndef GameLayerContainer_h
#define GameLayerContainer_h

#include "Prerequisites.h"

class GameLogic;

struct GamePostUpdater {
  GamePostUpdater(GameLayerContainer *ct);
  void update(float dt);
  GameLayerContainer *container;
};

class GameLayerContainer : public cocos2d::Layer {
public:
  static cocos2d::Scene *createPhysicsScene();

  GameLayerContainer();

  virtual ~GameLayerContainer();

  virtual bool init() override;

  virtual void onEnter() override;

  virtual void update(float dt) override;

  virtual void postUpdate(float dt);

  virtual void clean();

  virtual void onWinGame() = 0;

  virtual void afterLoad() {}

  virtual void preWinGame() {}

  virtual void onGameEnabled(bool val) {}

  virtual void onCameraUpdate(const cocos2d::Vec2 &relative) {}

  virtual int8_t getCameraDepth() {
    return -10;
  }

  cocos2d::Camera *getCamera() {
    return mCamera;
  }

  cocos2d::Node *getBlockRoot() {
    return mBlockRoot;
  }

#if !USE_REFACTOR
  GameLogic *getGame() {
    return mGame;
  }
  
protected:
  bool onCollisionDetected(cocos2d::PhysicsContact &contact,
                           cocos2d::PhysicsContactPreSolve &solve);

  GameLogic *mGame{nullptr};
#endif

  cocos2d::Node *mBlockRoot{nullptr};

  cocos2d::Camera *mCamera{nullptr};

  GamePostUpdater mPostUpdater;
};

#endif /* GameLayerContainer_h */
