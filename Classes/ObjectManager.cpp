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
  int colorIndex = DEFAULT_COLOR_ID;
  if (json.HasMember(LEVEL_BLOCK_PALETTE_ID)) {
    colorIndex = json[LEVEL_BLOCK_PALETTE_ID].GetInt();
  }
  
  Parameter param;
  param.set(PARAM_RENDERER, RENDERER_RECT)
       .set(PARAM_POS, json[LEVEL_BLOCK_POS].GetVec2())
       .set(PARAM_SIZE, json[LEVEL_BLOCK_SIZE].GetSize())
       .set(PARAM_COLOR_INDEX, colorIndex);

  GameObject *obj = new GameObject();
  obj->mID = json[LEVEL_BLOCK_ID].GetInt();
  obj->mKind = json[LEVEL_BLOCK_KIND].getEnum<BlockKind>();

  auto physicsConfig = BlockKindConfigs::getPhysicsConfig(obj->mKind);
  auto rendererConfig = BlockKindConfigs::getRendererConfig(obj->mKind);

  if (!rendererConfig.defaultTexture.empty()) {
    param.set(PARAM_IMAGE, rendererConfig.defaultTexture);
  }

  obj->setRenderer(rendererConfig.type)
     ->init(param)
     ->addToParent(GameLevel::instance().getGameLayer()->getBlockRoot(),
                   rendererConfig.zorder);

  if (physicsConfig.type != PHYSICS_NONE) {
    obj->addComponent<PhysicsComponent>()
       ->setPhysicsType(physicsConfig.type)
       ->setShape(physicsConfig.shapeType);
  }

  CC_ASSERT(!mObjects.count(obj->mID));
  mObjects[obj->mID] = obj;
  mIDCounter = std::max(obj->mID + 1, mIDCounter);
  return obj;
}

GameObject *ObjectManager::createObject(Parameter &param) {
  GameObject *obj = new GameObject();
  obj->mID = mIDCounter++;
  obj->mKind = param.get<BlockKind>(PARAM_BLOCK_KIND, KIND_BLOCK);
  
  auto physicsConfig = BlockKindConfigs::getPhysicsConfig(obj->mKind);
  auto rendererConfig = BlockKindConfigs::getRendererConfig(obj->mKind);
  
  obj->setRenderer(param.get<RendererType>(PARAM_RENDERER, RENDERER_RECT))
     ->init(param)
     ->addToParent(GameLevel::instance().getGameLayer()->getBlockRoot(),
                   rendererConfig.zorder);

  obj->addComponent<PhysicsComponent>()
     ->setPhysicsType(physicsConfig.type)
     ->setShape(physicsConfig.shapeType);

  CC_ASSERT(!mObjects.count(obj->mID));
  mObjects[obj->mID] = obj;
  return obj;
}

GameObject *ObjectManager::getObjectByID(int id) {
  auto it = mObjects.find(id);
  return it == mObjects.end() ? nullptr : it->second;
}
