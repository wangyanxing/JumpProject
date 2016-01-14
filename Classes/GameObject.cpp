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
  release();
}

void GameObject::setRenderer(GameRenderer *renderer) {
  CC_SAFE_DELETE(mRenderer);
  mRenderer = renderer;
}

void GameObject::update(float dt) {
  if (mRenderer) {
    mRenderer->update(dt);
  }

  for (auto component : mComponents) {
    component.second->update(dt);
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
      break;
    case COMPONENT_PATH:
    case COMPONENT_BUTTON:
    case COMPONENT_ROTATOR:
    default:
      CCLOGERROR("Invalid component type");
  }
  mComponents[type] = component;
  return component;
}

void GameObject::removeComponent(ComponentType type) {
  auto it = mComponents.find(type);
  if (it != mComponents.end()) {
    CC_SAFE_DELETE(it->second);
    mComponents.erase(it);
  }
}

bool GameObject::hasComponent(ComponentType type) {
  return mComponents.find(type) != mComponents.end();
}

void GameObject::release() {
  CC_SAFE_DELETE(mRenderer);

  for (auto &component : mComponents) {
    CC_SAFE_DELETE(component.second);
  }
  mComponents.clear();
}
