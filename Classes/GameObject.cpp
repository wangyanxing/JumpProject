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

void GameObject::setRenderer(GameRenderer *renderer) {
  CC_SAFE_DELETE(mRenderer);
  mRenderer = renderer;
}

void GameObject::update(float dt) {
  if (mRenderer) {
    mRenderer->update(dt);
  }
}
