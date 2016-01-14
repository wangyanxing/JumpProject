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

  virtual ComponentType getType() = 0;

  virtual void update(float dt) = 0;

protected:
  GameObject *mParent{nullptr};
};

#endif /* Component_h */
