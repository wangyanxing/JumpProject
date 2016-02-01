//
//  GameEvents.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/18/16.
//
//

#include "GameEvents.h"
#include "GameObject.h"
#include "GameRenderer.h"
#include "GameLevel.h"
#include "ObjectManager.h"
#include "RendererActions.h"
#include "GameConfig.h"

USING_NS_CC;

bool GameEvents::Arg::getBool() const {
  return value == "true" ? true : false;
}

int GameEvents::Arg::getInt() const {
  return atoi(value.c_str());
}

float GameEvents::Arg::getFloat() const {
  return atof(value.c_str());
}

GameObject *GameEvents::Arg::getGameObject() const {
  return GameLevel::instance().getObjectManager()->getObjectByID(getInt());
}

int GameEvents::Event::getMinimumArgs() const {
  int nonOpt = 0;
  for (auto &a : args) {
    nonOpt += a.optional ? 0 : 1;
  }
  return nonOpt;
}

GameEvents::GameEvents() {
  {
    Event e;
    e.command = "open_door";
    e.args = {
      {"",     false},    // ID
      {"250",  true},  // SPEED
      {"true", true}, // DIR
    };
    e.func = [&](const std::vector<Arg> &args, GameObject *block) {
      auto target = args[0].getGameObject();
      if (!target) {
        CCLOGWARN("Bad ID: %d", args[0].getInt());
        return;
      }
      auto renderer = target->getRenderer();
      renderer->getNode()->runAction(OpenCloseDoor::create(renderer,
                                                           args[1].getFloat() / 1000.0f,
                                                           OpenCloseDoor::OPEN,
                                                           args[2].getBool()));
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "close_door";
    e.args = {
      {"",     false},    // ID
      {"250",  true},  // SPEED, in ms
      {"true", true}, // DIR
    };
    e.func = [&](const std::vector<Arg> &args, GameObject *block) {
      auto target = args[0].getGameObject();
      if (!target) {
        CCLOGWARN("Bad ID: %d", args[0].getInt());
        return;
      }
      auto renderer = target->getRenderer();
      renderer->getNode()->runAction(OpenCloseDoor::create(renderer,
                                                           args[1].getFloat() / 1000.0f,
                                                           OpenCloseDoor::CLOSE,
                                                           args[2].getBool()));
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "exit";

    e.func = [&](const std::vector<Arg> &args, GameObject *block) {
      // TODO: Win game event.
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "die";

    e.func = [&](const std::vector<Arg> &args, GameObject *block) {
      GameLevel::instance().die();
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "dummy";

    e.func = [&](const std::vector<Arg> &args, GameObject *block) {};
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "show";
    e.args = {
      {"", false},    // ID
    };
    e.func = [&](const std::vector<Arg> &args, GameObject *block) {
      auto target = args[0].getGameObject();
      if (!target) {
        CCLOGWARN("Bad ID: %d", args[0].getInt());
        return;
      }
      target->getRenderer()->setVisible(true);
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "hide";
    e.args = {
      {"", false},    // ID
    };
    e.func = [&](const std::vector<Arg> &args, GameObject *block) {
      auto target = args[0].getGameObject();
      if (!target) {
        CCLOGWARN("Bad ID: %d", args[0].getInt());
        return;
      }
      target->getRenderer()->setVisible(false);
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "pause_path";
    e.args = {
      {"", false},    // ID
    };
    e.func = [&](const std::vector<Arg> &args, GameObject *block) {
      auto target = args[0].getGameObject();
      if (!target) {
        CCLOGWARN("Bad ID: %d", args[0].getInt());
        return;
      }
      // TODO: Pause path.
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "resume_path";
    e.args = {
      {"", false},    // ID
    };
    e.func = [&](const std::vector<Arg> &args, GameObject *block) {
     auto target = args[0].getGameObject();
      if (!target) {
        CCLOGWARN("Bad ID: %d", args[0].getInt());
        return;
      }
      // TODO: Resume path.
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "fade_in";
    e.args = {
      { "", false },   // ID
      { "0.5", true }, // TIME
    };
    e.func = [&](const std::vector<Arg>& args, GameObject *block){
      auto target = args[0].getGameObject();
      if (!target) {
        CCLOGWARN("Bad ID: %d", args[0].getInt());
        return;
      }
      target->getRenderer()->getNode()->runAction(FadeIn::create(args[1].getFloat()));
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "fade_out";
    e.args = {
      { "", false },   // ID
      { "0.5", true }, // TIME
    };
    e.func = [&](const std::vector<Arg>& args, GameObject *block){
      auto target = args[0].getGameObject();
      if (!target) {
        CCLOGWARN("Bad ID: %d", args[0].getInt());
        return;
      }
      target->getRenderer()->getNode()->runAction(FadeOut::create(args[1].getFloat()));
    };
    sEventLists[e.command] = e;
  }
  {
    Event e;
    e.command = "anim_up_down";
    e.args = {
      { "20", true },  // DISTANCE
      { "0.3", true }, // MOVE TIME
      { "1", true },   // STOP TIME
    };
    e.func = [&](const std::vector<Arg>& args, GameObject *block){
      float moveTime = args[1].getFloat();
      Vec2 delta(0, args[0].getFloat());
      float stopTime = args[2].getFloat();
      auto action = RepeatForever::create(Sequence::create(DelayTime::create(stopTime),
                                                           MoveBy::create(moveTime, delta),
                                                           MoveBy::create(moveTime, -delta),
                                                           MoveBy::create(moveTime, delta),
                                                           MoveBy::create(moveTime, -delta),
                                                           NULL));
      block->getRenderer()->getNode()->runAction(action);
    };
    sEventLists[e.command] = e;
  }
}

void GameEvents::callEvents(std::vector<std::string>& events, GameObject *caller) {
  for (auto& e : events) {
    if (!e.empty()) {
      callSingleEvent(e.c_str(), caller);
    }
  }
}

void GameEvents::callSingleEvent(const std::string &event, GameObject *caller) {
  if (!GameLevel::instance().isGameEnabled()) {
    return;
  }

  std::string e = event;
  auto splits = PathLib::stringSplit(e, " ");
  if (splits.empty()) {
    CCLOGWARN("Calling empty event!");
    return;
  }

  auto eit = sEventLists.find(splits[0]);
  if (eit == sEventLists.end()) {
    CCLOGWARN("Cannot locate event: %s!", splits[0].c_str());
    return;
  }

  if (splits.size() > eit->second.args.size() + 1) {
    CCLOGWARN("Too many arguments for the event calling: \"%s\"!", event.c_str());
    return;
  }

  if (splits.size() < eit->second.getMinimumArgs() + 1) {
    CCLOG("Too few arguments for the event calling: \"%s\"!", event.c_str());
    return;
  }

  std::vector<Arg> args;
  for (size_t i = 0; i < eit->second.args.size(); ++i) {
    auto arg = eit->second.args[i]; //copy
    if (i + 1 < splits.size())
      arg.value = splits[i + 1];
    args.push_back(arg);
  }

  // Call
  eit->second.func(args, caller);
}
