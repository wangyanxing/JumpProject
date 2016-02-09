//
//  GameObject.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#include "GameObject.h"
#include "GameLevel.h"
#include "GameRenderer.h"
#include "PhysicsShape.h"
#include "PhysicsComponent.h"
#include "InputComponent.h"
#include "PathComponent.h"
#include "ButtonComponent.h"
#include "EditorComponent.h"
#include "SimpleRenderer.h"
#include "DeathRenderer.h"
#include "GameLevel.h"
#include "GameLayerContainer.h"
#include "DeathRotatorRenderer.h"
#include "JsonFormat.h"
#include "EditorManager.h"
#include "BlockKindConfigs.h"
#include "ObjectManager.h"
#include "MathTools.h"

USING_NS_CC;

GameObject::GameObject() {
}

GameObject::~GameObject() {
  release();
}

void GameObject::addComponentCommand(ComponentCommand command, GameComponent *component) {
  mComponentCommands[command] = component;
}

void GameObject::removeComponentCommand(ComponentCommand command) {
  CC_ASSERT(mComponentCommands.count(command));
  mComponentCommands.erase(command);
}

void GameObject::runCommand(ComponentCommand command, const Parameter &param) {
  auto it = mComponentCommands.find(command);
  if (it == mComponentCommands.end()) {
    CCLOGERROR("Cannot find the command!");
    return;
  }

  if (!mEnabled && !it->second->forceUpdate()) {
    return;
  }
  it->second->runCommand(command, param);
}

bool GameObject::hasCommand(ComponentCommand command) {
  return mComponentCommands.count(command);
}

GameRenderer *GameObject::setRenderer(RendererType renderType) {
  GameRenderer *renderer = nullptr;
  switch (renderType) {
    case RENDERER_RECT:
      renderer = new SimpleRenderer(this);
      break;
    case RENDERER_DEATH:
      renderer = new DeathRenderer(this);
      break;
    case RENDERER_DEATH_CIRCLE:
      renderer = new DeathRotatorRenderer(this);
      break;
    default:
      CCLOGWARN("Invalid renderer type!");
  }
  return setRenderer(renderer);
}

GameRenderer *GameObject::setRenderer(GameRenderer *renderer) {
  CC_SAFE_DELETE(mRenderer);
  mRenderer = renderer;
  return mRenderer;
}

void GameObject::update(float dt) {
  mLastPosition = mRenderer->getPosition();

  if (mEnabled) {
    mRenderer->update(dt);
  }
  for (auto component : mComponents) {
    if (mEnabled || component.second->forceUpdate()) {
      component.second->update(dt);
    }
  }
}

void GameObject::beforeRender(float dt) {
  for (auto component : mComponents) {
    if (mEnabled || component.second->forceUpdate()) {
      component.second->beforeRender(dt);
    }
  }
  updateChildren();
  updateHelpers();
}

void GameObject::addChild(int childID) {
  mChildren.insert(childID);
}

void GameObject::removeChild(int childID) {
  mChildren.erase(childID);
}

void GameObject::removeAllChildren() {
  mChildren.clear();
}

bool GameObject::hasChildren() const {
  return !mChildren.empty();
}

void GameObject::updateChildren() {
  auto movement = mRenderer->getPosition() - mLastPosition;
  for (auto id : mChildren) {
    auto child = GameLevel::instance().getObjectManager()->getObjectByID(id);
    if (child) {
      auto pos = child->getRenderer()->getPosition();
      child->getRenderer()->setPosition(pos + movement);
    }
  }
}

void GameObject::load(JsonValueT &json) {
  mRenderer->load(json);

  if (json.HasMember(PATH_ARRAY)) {
    addComponent(COMPONENT_PATH);
  }

  if (json.HasMember(LEVEL_BLOCK_CHILDREN)) {
    auto size = json[LEVEL_BLOCK_CHILDREN].Size();
    for (auto i = 0; i < size; ++i) {
      addChild(json[LEVEL_BLOCK_CHILDREN][i].GetInt());
    }
  }

  for (auto comp : mComponents) {
    comp.second->load(json);
  }
}

void GameObject::clone(GameObject *obj) {
  mKind = obj->mKind;
  mRemovable = obj->mRemovable;
}

void GameObject::reset() {
  mRenderer->reset();
  for (auto comp : mComponents) {
    comp.second->reset();
  }
}

void GameObject::changeKind(BlockKind kind) {
  if (kind == mKind) {
    return;
  }
  mKind = kind;

  auto physicsConfig = BlockKindConfigs::getPhysicsConfig(mKind);
  auto rendererConfig = BlockKindConfigs::getRendererConfig(mKind);
  auto components = BlockKindConfigs::getComponents(mKind);

  auto size = mRenderer->getOriginalSize();
  if (mKind == KIND_DEATH_CIRCLE) {
    size.width = std::max(size.width, size.height);
    size.height = size.width;
  }

  Parameter param;
  param.set(PARAM_POS, mRenderer->getOriginalPosition())
       .set(PARAM_SIZE, size)
       .set(PARAM_COLOR_INDEX, mRenderer->getColorIndex())
       .set(PARAM_IMAGE, rendererConfig.defaultTexture);

  setRenderer(rendererConfig.type)
    ->init(param)
    ->addToParent(GameLevel::instance().getGameLayer()->getBlockRoot(), rendererConfig.zorder);

  getRenderer()->setShadowEnabled(rendererConfig.shadowEnabled);

  for (auto &component : mComponents) {
    CC_SAFE_DELETE(component.second);
  }
  mComponents.clear();

  if (physicsConfig.type != PHYSICS_NONE) {
    addComponent<PhysicsComponent>()
       ->setPhysicsType(physicsConfig.type)
       ->setShape(physicsConfig.shapeType);

    if (mKind == KIND_DEATH || mKind == KIND_DEATH_CIRCLE) {
      getComponent<PhysicsComponent>()->setCollisionEvents({"die"});
    }
  }

  for (auto comp : components) {
    addComponent(comp);
  }

#if EDITOR_MODE
  addComponent(COMPONENT_EDITOR);
#endif
}

void GameObject::save(JsWriter &writer) {
  writer.StartObject();

  writer.String(LEVEL_BLOCK_ID);
  writer.Int(mID);

  writer.String(LEVEL_BLOCK_KIND);
  writer.Enum<BlockKind>(mKind);

  if (!mRemovable) {
    writer.String(LEVEL_BLOCK_REMOVABLE);
    writer.Bool(mRemovable);
  }

  mRenderer->save(writer);

  for (auto comp : mComponents) {
    comp.second->save(writer);
  }

  if (!mChildren.empty()) {
    writer.String(LEVEL_BLOCK_CHILDREN);
    writer.StartArray();
    for (auto id : mChildren) {
      writer.Int(id);
    }
    writer.EndArray();
  }

  writer.EndObject();
}

void GameObject::onMove(const cocos2d::Vec2 &delta) {
  for (auto comp : mComponents) {
    comp.second->onMove(delta);
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
    case COMPONENT_INPUT:
      component = new InputComponent(this);
      break;
    case COMPONENT_PATH:
      component = new PathComponent(this);
      break;
    case COMPONENT_BUTTON:
      component = new ButtonComponent(this);
      break;
    case COMPONENT_EDITOR:
      component = new EditorComponent(this);
      break;
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
  mChildren.clear();
  mHelperNode->removeFromParent();
}

void GameObject::setEnabled(bool val) {
  if (mEnabled == val) {
    return;
  }
  mEnabled = val;
  mRenderer->reset();
  for (auto component : mComponents) {
    component.second->reset();
  }
}

bool GameObject::containsPoint(const cocos2d::Vec2 &pt) {
  auto physics = getComponent<PhysicsComponent>();
  if (physics) {
    return physics->getShape()->containsPoint(pt);
  } else {
    return getRenderer()->getBoundingBox().containsPoint(pt);
  }
}

void GameObject::initHelpers() {
  CC_ASSERT(!mHelperNode);
  mHelperNode = Node::create();
  mHelperNode->setCameraMask((unsigned short) CameraFlag::USER2);
  mHelperNode->setVisible(false);

  auto root = EditorManager::instance().getEditorRootNode();
  root->addChild(mHelperNode, ZORDER_EDT_OBJ_HELPER);

  // Label.
  if (mKind != KIND_HERO) {
    std::string label = std::to_string(mID);
    auto idLabel = Label::createWithSystemFont(label,
                                               "Arial",
                                               28,
                                               Size::ZERO,
                                               TextHAlignment::CENTER,
                                               TextVAlignment::CENTER);
    idLabel->enableShadow(Color4B(50, 50, 50, 200));
    idLabel->setCameraMask((unsigned short) CameraFlag::USER2);
    idLabel->setScale(0.5f);
    mHelperNode->addChild(idLabel, ZORDER_EDT_HELPER_LABEL);

    // Group.
    mGroupHelper = DrawNode::create();
    mGroupHelper->setCameraMask((unsigned short) CameraFlag::USER2);
    mHelperNode->addChild(mGroupHelper, 100);
  }
}

void GameObject::updateHelpers() {
  if (!mHelperNode->isVisible()) {
    return;
  }

  mHelperNode->setPosition(getRenderer()->getNode()->getPosition());
  for (auto component : mComponents) {
    component.second->updateHelpers();
  }

  if (mGroupHelper) {
    mGroupHelper->clear();
    auto pos = getRenderer()->getPosition();
    for (auto id : mChildren) {
      auto child = GameLevel::instance().getObjectManager()->getObjectByID(id);
      if (child) {
        auto childPos = child->getRenderer()->getPosition() - pos;
        mGroupHelper->drawLine(Vec2::ZERO, childPos, Color4F::GREEN);

        Vec2 dir = childPos.getNormalized();
        Vec2 c = dir * 20;
        float squartSum = sqrtf(dir.x * dir.x * + dir.y * dir.y);
        Vec2 ped1(dir.y / squartSum, -dir.x / squartSum);
        Vec2 ped2(-dir.y / squartSum, dir.x / squartSum);
        mGroupHelper->drawSolidCircle(childPos, 3, 0, 8, 1, 1, Color4F::GREEN);
        mGroupHelper->drawTriangle(Vec2::ZERO,
                                   c + ped1.getNormalized() * 5,
                                   c + ped2.getNormalized() * 5,
                                   Color4F::GREEN);
      }
    }
  }
}
