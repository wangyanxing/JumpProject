//
//  GameObject.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#ifndef GameObject_h
#define GameObject_h

#include "Prerequisites.h"
#include "Parameter.h"

class GameObject {
public:
  friend class ObjectManager;

  typedef std::unordered_map<ComponentType, GameComponent*, EnumClassHash> ComponentMap;
  
  typedef std::unordered_map<ComponentCommand, GameComponent*, EnumClassHash> ComponentCommandMap;

  GameObject();

  ~GameObject();

  void update(float dt);
  
  void postUpdate(float dt);

  void addComponandCommand(ComponentCommand command, GameComponent *component);

  void removeComponandCommand(ComponentCommand command);

  void runCommand(ComponentCommand command, const Parameter &param);

  GameRenderer *setRenderer(GameRenderer *renderer);

  GameRenderer *setRenderer(RendererType renderType);

  GameRenderer *getRenderer() {
    return mRenderer;
  }

  int getID() {
    return mID;
  }

  BlockKind getKind() {
    return mKind;
  }

  GameComponent* getComponent(ComponentType type);

  template<typename T>
  T* getComponent() {
    auto comp = getComponent(T::getType());
    return static_cast<T*>(comp);
  }

  GameComponent* addComponent(ComponentType type);
  
  template<typename T>
  T* addComponent() {
    auto comp = addComponent(T::getType());
    return static_cast<T*>(comp);
  }

  void removeComponent(ComponentType type);

  bool hasComponent(ComponentType type);

  void traverseComponents(std::function<void(GameComponent*)> func);

private:
  void release();

private:
  int mID{0};

  GameRenderer *mRenderer{nullptr};

  ComponentMap mComponents;

  ComponentCommandMap mComponentCommands;

  BlockKind mKind{KIND_BLOCK};
};

#endif /* GameObject_h */
