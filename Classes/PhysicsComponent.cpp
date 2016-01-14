//
//  PhysicsComponent.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "PhysicsComponent.h"
#include "PhysicsShape.h"

PhysicsComponent::PhysicsComponent(GameObject *parent) : GameComponent(parent) {

}

PhysicsComponent::~PhysicsComponent() {
  
}

void PhysicsComponent::update(float dt) {
  
}

void PhysicsComponent::setShape(PhysicsShape *shape) {
  CC_SAFE_DELETE(mShape);
  mShape = shape;
}