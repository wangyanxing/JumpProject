//
//  GameLayerContainer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/5/16.
//
//

#ifndef GameLayerContainer_h
#define GameLayerContainer_h

#include "cocos2d.h"

class GameLogic;
class GameLayerContainer;

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

  virtual void preWinGame() {}

  virtual int8_t getCameraDepth() {
    return -10;
  }

  cocos2d::Camera *getCamera() {
    return mCamera;
  }

  GameLogic *getGame() {
    return mGame;
  }

protected:
  bool onCollisionDetected(cocos2d::PhysicsContact &contact,
                           cocos2d::PhysicsContactPreSolve &solve);

private:
  GameLogic *mGame{nullptr};

  cocos2d::Camera *mCamera{nullptr};

  GamePostUpdater mPostUpdater;
};


#endif /* GameLayerContainer_h */
