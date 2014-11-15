//
//  Events.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/18/14.
//
//

#include "Events.h"
#include "EditorScene.h"
#include "LogicManager.h"
#include "PathLib.h"
#include "Blocks.h"

#include <string>
#include <map>
#include <functional>

namespace JE {
    
    struct Arg {
        Arg(const std::string& defaultVal, bool opt) : value(defaultVal), optional(opt) {}
        std::string value;
        bool optional{ false };
        
        bool getBool() const {
            return value == "true" ? true : false;
        }
        
        int getInt() const {
            return atoi(value.c_str());
        }
        
        float getFloat() const {
            return atof(value.c_str());
        }
    };
    
    struct Event {
        std::string command;
        std::vector<Arg> args;
        std::function<void(const std::vector<Arg>& args, BlockBase* block)> func;
        
        int getMinimumArgs() {
            int nonOpt = 0;
            for(auto& a : args) {
                nonOpt += a.optional ? 0 : 1;
            }
            return nonOpt;
        }
    };
}

static std::map<std::string, JE::Event> EventLists;

void initEvents() {
    
    using JE::Event;
    using JE::Arg;
    
    if(!EventLists.empty()) return;
    {
        Event e;
        e.command = "open_door";
        e.args = {
            {"", false},    // ID
            {"250", true},  // SPEED
            {"true", true}, // DIR
        };
        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            auto targetBlock = GameLogic::Game->findBlock(args[0].getInt());
            if (!targetBlock) {
                CCLOG("Bad ID: %d", args[0].getInt());
                return;
            }
            targetBlock->openDoor(args[1].getFloat(), args[2].getBool());
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "close_door";
        e.args = {
            { "", false },    // ID
            { "250", true },  // SPEED
            { "true", true }, // DIR
        };
        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            auto targetBlock = GameLogic::Game->findBlock(args[0].getInt());
            if (!targetBlock) {
                CCLOG("Bad ID: %d", args[0].getInt());
                return;
            }
            targetBlock->closeDoor(args[1].getFloat(), args[2].getBool());
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "exit";

        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            if (block->mTriggerEventContinueTime > 0.9f){
                GameLogic::Game->mWinFlag = true;
                block->mTriggerEventContinueTime = 0.0f;
            } else{
                GameLogic::Game->mHero->getSprite()->setOpacity(255.0f*(1.0f - block->mTriggerEventContinueTime)/1.0f);
                block->mHeroOpacityChanged = true;
            }
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "die";

        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            GameLogic::Game->mDeadFlag = true;
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "show";
        e.args = {
            {"", false},    // ID
        };
        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            auto targetBlock = GameLogic::Game->findBlock(args[0].getInt());
            if (!targetBlock) {
                CCLOG("Bad ID: %d", args[0].getInt());
                return;
            }
            
            targetBlock->setVisible(true);
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "hide";
        e.args = {
            {"", false},    // ID
        };
        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            auto targetBlock = GameLogic::Game->findBlock(args[0].getInt());
            if (!targetBlock) {
                CCLOG("Bad ID: %d", args[0].getInt());
                return;
            }
            
            targetBlock->setVisible(false);
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "moveTo";
        e.args = {
            { "", false },    // ID
            { "100", true },  // SPEED
            { "100", true }, // DIR
        };
        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            auto targetBlock = GameLogic::Game->findBlock(args[0].getInt());
            if (!targetBlock) {
                CCLOG("Bad ID: %d", args[0].getInt());
                return;
            }
            targetBlock->setPosition(args[1].getFloat(), args[2].getFloat());
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "pause_path";
        e.args = {
            {"", false},    // ID
        };
        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            auto targetBlock = GameLogic::Game->findBlock(args[0].getInt());
            if (!targetBlock) {
                CCLOG("Bad ID: %d", args[0].getInt());
                return;
            }
            if (targetBlock->mPath.empty()) {
                CCLOG("The object(ID=%d) has no path!", args[0].getInt());
                return;
            }
            targetBlock->mPath.mPause = true;
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "resume_path";
        e.args = {
            {"", false},    // ID
        };
        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            auto targetBlock = GameLogic::Game->findBlock(args[0].getInt());
            if (!targetBlock) {
                CCLOG("Bad ID: %d", args[0].getInt());
                return;
            }
            if (targetBlock->mPath.empty()) {
                CCLOG("The object(ID=%d) has no path!", args[0].getInt());
                return;
            }
            targetBlock->mPath.mPause = false;
        };
        EventLists[e.command] = e;
    }
    {
        Event e;
        e.command = "clear_stage";
        e.func = [&](const std::vector<Arg>& args, BlockBase* block){
            printf("clearstage\n");
        };
        EventLists[e.command] = e;
    }
}

void Events::callEvent(const char* event, BlockBase* block=NULL) {
    initEvents();
    
    if(!event) {
        CCLOG("Calling null event!");
        return;
    }
    
    std::string e = event;
    auto splits = DiPathLib::StringSplit(e, " ");
    if(splits.empty()) {
        CCLOG("Calling empty event!");
        return;
    }
    
    auto eit = EventLists.find(splits[0]);
    if(eit == EventLists.end()) {
        CCLOG("Cannot locate event: %s!", splits[0].c_str());
        return;
    }
    
    if(splits.size() > eit->second.args.size() + 1) {
        CCLOG("Too many arguments for the event calling: \"%s\"!", event);
        return;
    }
    
    if(splits.size() < eit->second.getMinimumArgs() + 1) {
        CCLOG("Too few arguments for the event calling: \"%s\"!", event);
        return;
    }
    
    std::vector<JE::Arg> args;
    
    for(size_t i = 0; i < eit->second.args.size(); ++i) {
        auto arg = eit->second.args[i]; //copy
        if(i+1 < splits.size())
            arg.value = splits[i+1];
        args.push_back(arg);
    }
    
    // call
    eit->second.func(args, block);
}

