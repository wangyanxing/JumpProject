//
//  GameObject.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#include "GameObject.h"
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
  if (!mEnabled) {
    return;
  }
  auto it = mComponentCommands.find(command);
  if (it == mComponentCommands.end()) {
    CCLOGERROR("Cannot find the command!");
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
  if (!mEnabled) {
    return;
  }

  mRenderer->update(dt);
  for (auto component : mComponents) {
    component.second->update(dt);
  }
}

void GameObject::beforeRender(float dt) {
  if (mEnabled) {
    for (auto component : mComponents) {
      component.second->beforeRender(dt);
    }
  }

  updateHelpers();
}

void GameObject::load(JsonValueT &json) {
  mRenderer->load(json);

  if (json.HasMember(PATH_ARRAY)) {
    addComponent(COMPONENT_PATH);
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

  writer.EndObject();
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
}
