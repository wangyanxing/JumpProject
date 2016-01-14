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

  void setRenderer(GameRenderer *renderer);

  GameRenderer *getRenderer() {
    return mRenderer;
  }

  int getID() {
    return mID;
  }

  GameComponent* getComponent(ComponentType type);

  GameComponent* addComponent(ComponentType type);

  void removeComponent(ComponentType type);

  bool hasComponent(ComponentType type);

private:
  void release();

private:
  int mID{0};

  GameRenderer *mRenderer{nullptr};

  ComponentMap mComponents;
};

#endif /* GameObject_h */
