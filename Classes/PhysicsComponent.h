//
//  PhysicsComponent.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef PhysicsComponent_h
#define PhysicsComponent_h

#include "GameComponent.h"

class PhysicsComponent : public GameComponent {
public:
  PhysicsComponent(GameObject *parent);

  virtual ~PhysicsComponent();

public:
  virtual void update(float dt) override;

  void setShape(PhysicsShape *shape);

  ComponentType getType() override {
    return COMPONENT_PHYSICS;
  }

  PhysicsShape *getShape() {
    return mShape;
  }

  PhysicsType getPhysicsType() {
    return mPhysicsType;
  }

protected:
  PhysicsType mPhysicsType{PHYSICS_NONE};

  PhysicsShape *mShape{nullptr};
};

#endif /* PhysicsComponent_h */
