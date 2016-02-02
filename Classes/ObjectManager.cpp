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

  bool removable = true;
  if (json.HasMember(LEVEL_BLOCK_REMOVABLE)) {
    removable = json[LEVEL_BLOCK_REMOVABLE].GetBool();
  }
  
  Parameter param;
  param.set(PARAM_POS, json[LEVEL_BLOCK_POS].GetVec2())
       .set(PARAM_SIZE, json[LEVEL_BLOCK_SIZE].GetSize())
       .set(PARAM_COLOR_INDEX, colorIndex)
       .set(PARAM_BLOCK_KIND, json[LEVEL_BLOCK_KIND].getEnum<BlockKind>())
       .set(PARAM_REMOVABLE, removable);
  
  int id = json[LEVEL_BLOCK_ID].GetInt();
  mIDCounter = std::max(id + 1, mIDCounter);

  auto obj = createObjectImpl(param, id);
  obj->load(json);
  return obj;
}

GameObject *ObjectManager::createObject(Parameter &param) {
  return createObjectImpl(param, mIDCounter++);
}

GameObject *ObjectManager::createObjectImpl(Parameter &param, int id) {
  GameObject *obj = new GameObject();
  obj->mID = id;
  obj->mKind = param.get<BlockKind>(PARAM_BLOCK_KIND, KIND_BLOCK);
  obj->mRemovable = param.get<bool>(PARAM_REMOVABLE, true);
  
  auto physicsConfig = BlockKindConfigs::getPhysicsConfig(obj->mKind);
  auto rendererConfig = BlockKindConfigs::getRendererConfig(obj->mKind);
  auto components = BlockKindConfigs::getComponents(obj->mKind);
  
  if (!rendererConfig.defaultTexture.empty()) {
    param.set(PARAM_IMAGE, rendererConfig.defaultTexture);
  }
  
  obj->setRenderer(rendererConfig.type)
     ->init(param)
     ->addToParent(GameLevel::instance().getGameLayer()->getBlockRoot(), rendererConfig.zorder);

  if (physicsConfig.type != PHYSICS_NONE) {
    obj->addComponent<PhysicsComponent>()
       ->setPhysicsType(physicsConfig.type)
       ->setShape(physicsConfig.shapeType);
  }
  
  for (auto comp : components) {
    obj->addComponent(comp);
  }
  
  CC_ASSERT(!mObjects.count(obj->mID));
  mObjects[obj->mID] = obj;

  obj->initHelpers();
  return obj;
}

GameObject *ObjectManager::getObjectByID(int id) {
  auto it = mObjects.find(id);
  return it == mObjects.end() ? nullptr : it->second;
}
