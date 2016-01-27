//
//  GameLevel.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef GameLevel_h
#define GameLevel_h

#include "Prerequisites.h"
#include "Singleton.h"

class GameLevel {
  DECL_SINGLETON(GameLevel)

public:
  void init();

  void release();

  void update(float dt);
  
  void beforeRender(float dt);

  void load(const std::string &levelFile);

  void unload();

  void enableGame(bool enable);
  
  bool isGameEnabled() const {
    return mGameEnabled;
  }

  ObjectManager *getObjectManager() {
    return mObjectManager;
  }

  PhysicsManager *getPhysicsManager() {
    return mPhysicsManager;
  }

  ColorPalette *getPalette() {
    return mPalette;
  }

  void setGameLayer(GameLayerContainer *layer) {
    mGameLayer = layer;
  }

  GameLayerContainer *getGameLayer() {
    return mGameLayer;
  }

  int getNumShadowGroup() const {
    return mNumShadowGroup;
  }

  cocos2d::Rect getBounds() const {
    return mBounds;
  }
  
  GameObject *getHero();
  
private:
  void createHero(const cocos2d::Vec2 &pos);

  void updateCamera(cocos2d::Camera *cam, bool forceUpdate);

  void updateBounds();

private:
  ObjectManager *mObjectManager{nullptr};

  GameLayerContainer *mGameLayer{nullptr};

  PhysicsManager *mPhysicsManager{nullptr};

  ColorPalette *mPalette{nullptr};
  
  bool mGameEnabled{false};

  int mNumShadowGroup{0};

  cocos2d::Rect mBounds;
};

#endif /* GameLevel_h */
