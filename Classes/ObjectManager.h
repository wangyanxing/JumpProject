//
//  ObjectManager.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef ObjectManager_h
#define ObjectManager_h

#include "Prerequisites.h"
#include "Singleton.h"
#include "Parameter.h"

class ObjectManager {
public:
  friend class GameLevel;
  typedef std::map<int, GameObject*> ObjectMap;

  ObjectManager(GameLevel *level);

  ~ObjectManager();

  void cleanUp();

  GameObject *createObject(Parameter& params);

  GameObject *getObjectByID(int id);

private:
  GameRenderer *createRenderer(GameObject *parent, RendererType type);

private:
  GameLevel *mParentLevel{nullptr};

  int mIDCounter{0};

  ObjectMap mObjects;
};

#endif /* ObjectManager_h */
