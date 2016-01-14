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

class Component {
public:
  Component(GameObject *parent): mParent(parent) {}

  virtual ~Component() {}

  virtual ComponentType getType() = 0;

  virtual void update(float dt) = 0;

protected:
  GameObject *mParent{nullptr};
};

#endif /* Component_h */
