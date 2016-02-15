//
//  GameEvents.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/18/16.
//
//

#ifndef GameEvents_h
#define GameEvents_h

#include "Prerequisites.h"
#include "Singleton.h"

class GameEvents {
public:
  DECL_SIMPLE_SINGLETON(GameEvents);

  /**
   * Event argument.
   */
  struct Arg {
    Arg(const std::string &defaultVal, bool opt) : value(defaultVal), optional(opt) {}

    std::string value;

    bool optional{false};

    bool getBool() const;

    int getInt() const;

    float getFloat() const;

    GameObject *getGameObject() const;
  };

  /**
   * Event structure.
   */
  struct Event {
    std::string command;

    std::vector<Arg> args;

    std::function<void(const std::vector<Arg>&, GameObject*)> func;

    int getMinimumArgs() const;
  };

  void callEvents(std::vector<std::string>& events, GameObject *caller);

  void callSingleEvent(const std::string &event, GameObject *caller);

private:
  GameEvents();

private:
  std::map<std::string, Event> sEventLists;
};

#endif /* GameEvents_h */
