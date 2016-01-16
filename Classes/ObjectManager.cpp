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
#include "JsonFormat.h"

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
  param.set(PARAM_RENDERER, RENDERER_RECT);
  param.set(PARAM_POS, json[LEVEL_BLOCK_POS].GetVec2());
  param.set(PARAM_SIZE, json[LEVEL_BLOCK_SIZE].GetSize());

  int colorIndex = DEFAULT_COLOR_ID;
  if (json.HasMember(LEVEL_BLOCK_PALETTE_ID)) {
    colorIndex = json[LEVEL_BLOCK_PALETTE_ID].GetInt();
  }
  param.set(PARAM_COLOR_INDEX, colorIndex);

  GameObject *obj = new GameObject();
  obj->mID = json[LEVEL_BLOCK_ID].GetInt();
  obj->mKind = json[LEVEL_BLOCK_KIND].getEnum<BlockKind>();

  auto physicsConfig = BlockKindConfigs::getPhysicsConfig(obj->mKind);
  auto rendererConfig = BlockKindConfigs::getRendererConfig(obj->mKind);

  if (!rendererConfig.defaultTexture.empty()) {
    param.set(PARAM_IMAGE, rendererConfig.defaultTexture);
  }

  obj->setRenderer(rendererConfig.type);
  obj->getRenderer()->init(param);
  obj->getRenderer()->addToParent(GameLevel::instance().getGameLayer()->getBlockRoot(),
                                  rendererConfig.zorder);

  if (physicsConfig.type != PHYSICS_NONE) {
    obj->addComponent(COMPONENT_PHYSICS);
    obj->getComponent<PhysicsComponent>()->setShape(physicsConfig.shapeType);
  }

  CC_ASSERT(!mObjects.count(obj->mID));
  mObjects[obj->mID] = obj;
  mIDCounter = std::max(obj->mID, mIDCounter);
  return obj;
}

GameObject *ObjectManager::createObject(Parameter &param) {
  RendererType rendererType = param.get<RendererType>(PARAM_RENDERER);

  GameObject *obj = new GameObject();
  obj->mID = ++mIDCounter;
  obj->setRenderer(rendererType);
  obj->getRenderer()->init(param);
  obj->getRenderer()->addToParent(GameLevel::instance().getGameLayer()->getBlockRoot(),
                                  ZORDER_BLOCK);

  obj->addComponent(COMPONENT_PHYSICS);
  obj->getComponent<PhysicsComponent>()->setShape(PHYSICS_SHAPE_RECT);

  CC_ASSERT(!mObjects.count(obj->mID));
  mObjects[obj->mID] = obj;
  return obj;
}

GameObject *ObjectManager::getObjectByID(int id) {
  auto it = mObjects.find(id);
  return it == mObjects.end() ? nullptr : it->second;
}
