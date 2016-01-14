//
//  GameObject.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#include "GameObject.h"
#include "GameRenderer.h"
#include "PhysicsComponent.h"

USING_NS_CC;

GameObject::GameObject() {

}

GameObject::~GameObject() {
  
}

void GameObject::setRenderer(GameRenderer *renderer) {
  CC_SAFE_DELETE(mRenderer);
  mRenderer = renderer;
}

void GameObject::update(float dt) {
  if (mRenderer) {
    mRenderer->update(dt);
  }
}

GameComponent* GameObject::getComponent(ComponentType type) {
  auto it = mComponents.find(type);
  return it == mComponents.end() ? nullptr : it->second;
}

GameComponent* GameObject::addComponent(ComponentType type) {
  auto component = getComponent(type);
  CC_SAFE_DELETE(component);

  switch (type) {
    case COMPONENT_PHYSICS:
      component = new PhysicsComponent(this);
    case COMPONENT_PATH:
    case COMPONENT_BUTTON:
    case COMPONENT_ROTATOR:
    default:
      CCLOGWARN("Invalid component type");
      return nullptr;
  }
}
