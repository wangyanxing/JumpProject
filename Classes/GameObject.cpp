//
//  GameObject.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#include "GameObject.h"
#include "GameRenderer.h"

GameObject::GameObject() {

}

GameObject::~GameObject() {
  
}

GameRenderer *GameObject::setRenderer(RendererType type) {
  return nullptr;
}

void GameObject::update(float dt) {
  if (mRenderer) {
    mRenderer->update(dt);
  }
}
