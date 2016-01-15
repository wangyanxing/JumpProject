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
#include "JsonParser.h"

class ObjectManager {
public:
  friend class GameLevel;
  typedef std::map<int, GameObject*> ObjectMap;

  ObjectManager();

  ~ObjectManager();

  void cleanUp();

  GameObject *createObject(Parameter &params);

  GameObject *createObject(JsonValueT &json);

  GameObject *getObjectByID(int id);

private:
  int mIDCounter{0};

  ObjectMap mObjects;
};

#endif /* ObjectManager_h */
