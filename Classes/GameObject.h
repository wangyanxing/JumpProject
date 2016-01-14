//
//  GameObject.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#ifndef GameObject_h
#define GameObject_h

#include "GameTypes.h"

class GameRenderer;

class GameObject {
public:
  friend class ObjectManager;

  GameObject();

  ~GameObject();

  void update(float dt);

  void setRenderer(GameRenderer *renderer);

  GameRenderer *getRenderer() {
    return mRenderer;
  }

  int getID() {
    return mID;
  }

private:
  int mID{0};

  GameRenderer *mRenderer{nullptr};
};

#endif /* GameObject_h */
