//
//  GameLevel.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef GameLevel_h
#define GameLevel_h

#include "Singleton.h"

class ObjectManager;
class GameLayerContainer;

class GameLevel {
  DECL_SINGLETON(GameLevel)

public:
  void init();

  void release();

  void update(float dt);  

  ObjectManager *getObjectManager() {
    return mObjectManager;
  }

  void setGameLayer(GameLayerContainer *layer) {
    mGameLayer = layer;
  }

  GameLayerContainer *getGameLayer() {
    return mGameLayer;
  }

private:
  ObjectManager *mObjectManager{nullptr};

  GameLayerContainer *mGameLayer{nullptr};
};

#endif /* GameLevel_h */
