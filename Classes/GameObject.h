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
#include "JsonParser.h"
#include "JsonWriter.h"

class GameObject {
public:
  friend class ObjectManager;

  typedef std::unordered_map<ComponentType, GameComponent*, EnumClassHash> ComponentMap;
  
  typedef std::unordered_map<ComponentCommand, GameComponent*, EnumClassHash> ComponentCommandMap;

  GameObject();

  ~GameObject();

  void update(float dt);
  
  void beforeRender(float dt);

  void reset();

  void addComponentCommand(ComponentCommand command, GameComponent *component);

  void removeComponentCommand(ComponentCommand command);

  void runCommand(ComponentCommand command, const Parameter &param);

  bool hasCommand(ComponentCommand command);

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

  void load(JsonValueT &json);

  void save(JsWriter &writer);

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

  bool isEnabled() const {
    return mEnabled;
  }

  void setEnabled(bool val);

  bool isRemovable() const {
    return mRemovable;
  }

private:
  void release();

private:
  int mID{0};

  GameRenderer *mRenderer{nullptr};

  ComponentMap mComponents;

  ComponentCommandMap mComponentCommands;

  BlockKind mKind{KIND_BLOCK};

  bool mEnabled{false};

  bool mRemovable{true};
};

#endif /* GameObject_h */
