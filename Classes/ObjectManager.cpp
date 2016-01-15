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
#include "GameLevel.h"
#include "GameLayerContainer.h"
#include "PhysicsComponent.h"
#include "BlockKindConfigs.h"

USING_NS_CC;

ObjectManager::ObjectManager() {
}

ObjectManager::~ObjectManager() {
}

void ObjectManager::cleanUp() {
  mIDCounter = 0;
  for (auto &obj : mObjects) {
    CC_SAFE_DELETE(obj.second);
  }
  mObjects.clear();
}

GameObject *ObjectManager::createObject(JsonValueT &json) {
  Parameter param;
  param[PARAM_RENDERER] = RENDERER_RECT;
  param[PARAM_POS] = json["position"].GetVec2();
  param[PARAM_SIZE] = json["size"].GetSize();

  int colorIndex = DEFAULT_COLOR_ID;
  if (json.HasMember("paletteIndex")) {
    colorIndex = json["paletteIndex"].GetInt();
  }
  param[PARAM_COLOR_INDEX] = colorIndex;

  GameObject *obj = new GameObject();
  obj->mID = json["id"].GetInt();
  obj->mKind = json["kind"].getEnum<BlockKind>();

  auto physicsConfig = BlockKindConfigs::getPhysicsConfig(obj->mKind);
  auto rendererConfig = BlockKindConfigs::getRendererConfig(obj->mKind);

  if (!rendererConfig.defaultTexture.empty()) {
    param[PARAM_IMAGE] = rendererConfig.defaultTexture;
  }

  obj->setRenderer(rendererConfig.type);
  obj->getRenderer()->init(param);
  obj->getRenderer()->addToParent(GameLevel::instance().getGameLayer()->getBlockRoot(),
                                  ZORDER_BLOCK);

  if (physicsConfig.type != PHYSICS_NONE) {
    obj->addComponent(COMPONENT_PHYSICS);
    obj->getComponent<PhysicsComponent>()->setShape(physicsConfig.shapeType);
  }

  mIDCounter = std::max(obj->mID, mIDCounter);
  return obj;
}

GameObject *ObjectManager::createObject(Parameter &param) {
  CHECK_PARAM(PARAM_RENDERER);
  RendererType rendererType = GET_PARAM(PARAM_RENDERER, RendererType);

  GameObject *obj = new GameObject();
  obj->mID = ++mIDCounter;
  obj->setRenderer(rendererType);
  obj->getRenderer()->init(param);
  obj->getRenderer()->addToParent(GameLevel::instance().getGameLayer()->getBlockRoot(),
                                  ZORDER_BLOCK);

  obj->addComponent(COMPONENT_PHYSICS);
  obj->getComponent<PhysicsComponent>()->setShape(PHYSICS_SHAPE_RECT);
  return obj;
}

GameObject *ObjectManager::getObjectByID(int id) {
  auto it = mObjects.find(id);
  return it == mObjects.end() ? nullptr : it->second;
}
