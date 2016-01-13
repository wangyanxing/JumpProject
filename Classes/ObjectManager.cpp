//
//  ObjectManager.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "ObjectManager.h"
#include "GameObject.h"
#include "SimpleRenderer.h"

USING_NS_CC;

void ObjectManager::cleanUp() {
  mIDCounter = 0;
  for (auto &obj : mObjects) {
    CC_SAFE_DELETE(obj.second);
  }
  mObjects.clear();
}

GameRenderer *ObjectManager::createRenderer(GameObject *parent, RendererType type) {
  switch (type) {
    case RENDERER_RECT:
      return new SimpleRenderer(parent);
    default:
      CCLOGWARN("Invalid renderer type!");
      return nullptr;
  }
}

GameObject *ObjectManager::createObject(Parameter& param) {
  CHECK_PARAM(PARAM_RENDERER);
  RendererType rendererType = GET_PARAM(PARAM_RENDERER, RendererType);

  GameObject *obj = new GameObject();
  obj->mID = mIDCounter++;
  obj->setRenderer(rendererType);
  obj->getRenderer()->init(param);

  return obj;
}

GameObject *ObjectManager::getObjectByID(int id) {
  auto it = mObjects.find(id);
  return it == mObjects.end() ? nullptr : it->second;
}
