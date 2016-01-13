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
  GameObject();

  ~GameObject();

  void update(float dt);

  GameRenderer *setRenderer(RendererType type);

private:
  GameRenderer *mRenderer{nullptr};
};

#endif /* GameObject_h */
