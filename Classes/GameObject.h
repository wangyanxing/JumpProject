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

class GameObject {
public:
  friend class ObjectManager;

  typedef std::map<ComponentType, GameComponent*> ComponentMap;

  GameObject();

  ~GameObject();

  void update(float dt);

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

private:
  void release();

private:
  int mID{0};

  GameRenderer *mRenderer{nullptr};

  ComponentMap mComponents;

  BlockKind mKind{KIND_BLOCK};
};

#endif /* GameObject_h */
