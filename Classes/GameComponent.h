//
//  Compoment.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef Component_h
#define Component_h

#include "Prerequisites.h"

class GameComponent {
public:
  GameComponent(GameObject *parent): mParent(parent) {}

  virtual ~GameComponent() {}

  virtual void update(float dt) = 0;

  GameObject *getParent() {
    return mParent;
  }

protected:
  GameObject *mParent{nullptr};
};

#endif /* Component_h */
