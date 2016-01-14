//
//  PhysicsComponent.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef PhysicsComponent_h
#define PhysicsComponent_h

#include "Component.h"

class PhysicsComponent : Component {
public:
  PhysicsComponent(GameObject *parent);

  virtual ~PhysicsComponent();

public:
  virtual void update(float dt) override;
};

#endif /* PhysicsComponent_h */
