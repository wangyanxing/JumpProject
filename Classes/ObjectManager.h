//
//  ObjectManager.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef ObjectManager_h
#define ObjectManager_h

#include "Singleton.h"
#include "Parameter.h"
#include "GameTypes.h"

class GameObject;
class GameRenderer;

class ObjectManager {
  DECL_SINGLETON(ObjectManager)

public:
  typedef std::map<int, GameObject*> ObjectMap;

  void cleanUp();

  GameObject *createObject(Parameter& params);

  GameObject *getObjectByID(int id);

private:
  GameRenderer *createRenderer(GameObject *parent, RendererType type);

private:
  int mIDCounter{0};

  ObjectMap mObjects;
};

#endif /* ObjectManager_h */
