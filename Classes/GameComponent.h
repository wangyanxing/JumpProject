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

#define DECLARE_COMP_TYPE(t) \
  static ComponentType getType() {return t;}

/**
 * Game component, can be dynamically added or removed into an object.
 */
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
