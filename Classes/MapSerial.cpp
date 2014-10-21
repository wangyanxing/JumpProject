//
//  MapSerial.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/16/14.
//
//

#include "MapSerial.h"
#include "GameScene.h"
#include "HttpHelper.h"
#include "PathLib.h"
#include "UILayer.h"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#   include <windows.h>
#   include <Lmcons.h>
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#   include <unistd.h>
#   include <sys/types.h>
#endif

#include <sstream>

#define INDENT_1 ss<<"  ";
#define INDENT_2 ss<<"    ";
#define INDENT_3 ss<<"      ";
#define INDENT_4 ss<<"        ";
#define INDENT_5 ss<<"          ";
#define RT_LINE ss<<","<<std::endl;

void pushwarning() {
    CCLOG("Invalid map file!");
}

#define SHOW_WARNING else{pushwarning();}

using namespace std;
using namespace rapidjson;

#include <regex>
#include <iostream>

using namespace cocos2d;

std::string colorStr(const Color3B& color) {
    char temp[100];
    sprintf(temp, "\"#%02X%02X%02X\"",color.r,color.g,color.b);
    return temp;
}

Color3B str2Color(std::string hex) {
    
    int rgb[3];
    stringstream ss;
    string str;
    
    // Drop a hash if the value has one
    if (hex[0] == '#') {
        hex.erase(0,1);
    }
    
    int size = hex.size();
    for (int i = 0; i < 3; i++) {
        // Determine 3 or 6 character format.
        if (size == 3) { str = string(2, hex[i]); }
        else if (size == 6) { str = hex.substr(i * 2, 2); }
        else { break; }
        
        ss << std::hex << str;
        ss >> rgb[i];
        ss.clear();
    }

    return Color3B(rgb[0],rgb[1],rgb[2]);
}

std::string vec2Str(const Vec2& v) {
    char temp[100];
    sprintf(temp, "\"%g,%g\"",v.x,v.y);
    return temp;
}

Vec2 str2Vec(const std::string& str) {
    auto v = DiPathLib::StringSplit(str, ",");
    if(v.size() != 2) {
        CCLOG("Invalid vec2: %s", str.c_str());
        return Vec2::ZERO;
    }
    return Vec2(atof(v[0].c_str()), atof(v[1].c_str()));
}

std::string size2Str(const Size& v) {
    char temp[100];
    sprintf(temp, "\"%g,%g\"",v.width,v.height);
    return temp;
}

Size str2Size(const std::string& str) {
    auto v = DiPathLib::StringSplit(str, ",");
    if(v.size() != 2) {
        CCLOG("Invalid vec2: %s", str.c_str());
        return Size::ZERO;
    }
    return Size(atof(v[0].c_str()), atof(v[1].c_str()));
}


std::string bool2Str(bool v) {
    return v ? "true" : "false";
}

#if 0
bool str2Bool(const string& v) {
    return v == "true" ? true : false;
}
#endif

std::string kind2Str(BlockKind v) {
    static std::string names[] = {
        "\"HERO\"",
        "\"BLOCK\"",
        "\"DEATH\"",
        "\"DEATH_CIRCLE\"",
        "\"BUTTON\"",
        "\"PUSHABLE\"",
    };
    return names[v];
}

BlockKind str2Kind(const string& v) {
    static std::map<std::string, BlockKind> kinds = {
        {"HERO",KIND_HERO},
        {"BLOCK",KIND_BLOCK},
        {"DEATH",KIND_DEATH},
        {"DEATH_CIRCLE",KIND_DEATH_CIRCLE},
        {"BUTTON",KIND_BUTTON},
        {"PUSHABLE",KIND_PUSHABLE}
    };
    
    if(!kinds.count(v)) {
        CCLOG("Invalid kind: %s", v.c_str());
        return KIND_BLOCK;
    }
    return kinds[v];
}

std::string direction2Str(Button::PushDir v) {
    static std::string names[] = {
        "\"UP\"",
        "\"DOWN\"",
        "\"LEFT\"",
        "\"RIGHT\""
    };
    return names[v];
}

Button::PushDir str2Direction(const string& v) {
    static std::map<std::string, Button::PushDir> kinds = {
        {"UP",Button::DIR_UP},
        {"DOWN",Button::DIR_DOWN},
        {"LEFT",Button::DIR_LEFT},
        {"RIGHT",Button::DIR_RIGHT}
    };
    
    if(!kinds.count(v)) {
        CCLOG("Invalid push direction: %s", v.c_str());
        return Button::DIR_DOWN;
    }
    return kinds[v];
}

void MapSerial::saveMap(const char* file) {
    
    time_t rawtime;
    struct tm * ptm;
    time ( &rawtime );
    ptm = gmtime ( &rawtime );
    std::string timestr = asctime(ptm);
    timestr.resize(timestr.size()-1);
    
    std::string author = "unknown";
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    TCHAR username[UNLEN+1];
    DWORD size = UNLEN + 1;
    GetUserName((TCHAR*)username, &size);
    author = username;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    author = getlogin();
#endif
    
    stringstream ss;
    ss << "{\n";
    
    INDENT_1 ss << "\"author\": " << "\"" << author << "\""; RT_LINE
    INDENT_1 ss << "\"time\": " << "\"" << timestr << "\""; RT_LINE
    INDENT_1 ss << "\"backgroundColor\": " << colorStr(GameScene::Scene->mBackgroundColor); RT_LINE
    INDENT_1 ss << "\"heroColor\": " << colorStr(GameScene::Scene->mBlockColors[0]); RT_LINE
    INDENT_1 ss << "\"normalBlockColor\": " << colorStr(GameScene::Scene->mBlockColors[1]); RT_LINE
    INDENT_1 ss << "\"deathBlockColor\": " << colorStr(GameScene::Scene->mBlockColors[2]); RT_LINE
    INDENT_1 ss << "\"deathCircleColor\": " << colorStr(GameScene::Scene->mBlockColors[3]); RT_LINE
    INDENT_1 ss << "\"buttonColor\": " << colorStr(GameScene::Scene->mBlockColors[4]); RT_LINE
    INDENT_1 ss << "\"pushableBlockColor\": " << colorStr(GameScene::Scene->mBlockColors[5]); RT_LINE
    INDENT_1 ss << "\"spawnPosition\": " << vec2Str(GameScene::Scene->mSpawnPoint->getPosition()); RT_LINE
    
    INDENT_1 ss << "\"blocks\": [ \n";
    
    for(auto it = GameScene::Scene->mBlocks.begin(); it != GameScene::Scene->mBlocks.end(); ++it) {
        auto b = it->second;
        
        if(it != GameScene::Scene->mBlocks.begin()) {
            INDENT_2 ss << "},\n";
        }
        
        INDENT_2 ss << "{ \n";
        INDENT_3 ss << "\"id\": " << b->mID; RT_LINE
        INDENT_3 ss << "\"size\": " << size2Str(b->getSize()); RT_LINE
        INDENT_3 ss << "\"position\": " << vec2Str(b->getPosition()); RT_LINE
        INDENT_3 ss << "\"pickable\": " << bool2Str(b->mCanPickup); RT_LINE
        INDENT_3 ss << "\"rotatespeed\": " << b->mRotationSpeed; RT_LINE
        
        if(b->mKind == KIND_BUTTON) {
            INDENT_3 ss << "\"direction\": " << direction2Str(b->mButton->mDir); RT_LINE
            INDENT_3 ss << "\"canRestore\": " << bool2Str(b->mButton->mCanRestore); RT_LINE
            
            INDENT_3 ss << "\"pushedEvent\": " << "\""<< b->mButton->mPushedEvent << "\""; RT_LINE
            INDENT_3 ss << "\"restoredEvent\": " << "\""<< b->mButton->mRestoredEvent << "\""; RT_LINE
            INDENT_3 ss << "\"pushingEvent\": " << "\""<< b->mButton->mPushingEvent << "\""; RT_LINE
        }
        
        // group
        auto ig = GameScene::Scene->mGroups.find(b);
        if(ig != GameScene::Scene->mGroups.end() && !ig->second.empty()) {
            INDENT_3 ss << "\"groupMembers\": [";
            for (size_t i = 0; i < ig->second.size(); ++i) {
                ss << ig->second[i]->mID;
                if(i != ig->second.size()-1) ss << ", ";
            }
            ss << "],\n";
        }
        
        // path
        if(!b->mPath.empty()) {
            INDENT_3 ss << "\"pathSpeed\": " << b->mPath.mSpeed; RT_LINE
            INDENT_3 ss << "\"pingpong\": " << bool2Str(b->mPath.mPingPong); RT_LINE
            INDENT_3 ss << "\"pause\": " << bool2Str(b->mPath.mPause); RT_LINE
            INDENT_3 ss << "\"pathWaitTime\": " << b->mPath.mPathWaitTime; RT_LINE
            INDENT_3 ss << "\"pathes\": [ \n";
            
            for (size_t i = 0; i < b->mPath.getNumPoints(); ++i) {
                INDENT_4 ss << "{ \n";
                const auto& p = b->mPath.getPoint(i);
                INDENT_5 ss << "\"position\": " << vec2Str(p.pt); RT_LINE
                INDENT_5 ss << "\"waittime\": " << p.waitTime << "\n";
                INDENT_4 ss << "}";
                if(i != b->mPath.getNumPoints()-1) RT_LINE
                    else ss << "\n";
            }
            
            INDENT_3 ss << "],\n";
        }
        INDENT_3 ss << "\"kind\": " << kind2Str(b->mKind) << "\n";
    }
    if(!GameScene::Scene->mBlocks.empty()) {
        INDENT_2 ss << "}\n";
    }
    
    INDENT_1 ss << "]\n";
    
    ss << "}";
    
    auto fp = fopen(file, "w+");
    if(!fp) {
        CCLOG("Warning: cannot access the map file : %s", file);
        return;
    }
    fprintf(fp, "%s", ss.str().c_str());
    fclose(fp);
    
    // update file
    GameScene::Scene->mCurFileName = file;
    UILayer::Layer->setFileName(file);
    UILayer::Layer->addMessage("File saved");
    
    // save to history
    UserDefault::getInstance()->setStringForKey("lastedit", file);
    UserDefault::getInstance()->flush();
    
    // save to server
    std::string filename = file;
    
    const size_t last_slash_idx = filename.find_last_of("\\/");
    if (std::string::npos != last_slash_idx) {
        filename.erase(0, last_slash_idx + 1);
    }
    
    HttpHelper::updateMap(filename, author, timestr, ss.str());
}

void MapSerial::loadLastEdit() {
    auto file = UserDefault::getInstance()->getStringForKey("lastedit");
    if(file.empty()) return;
    loadMap(file.c_str());
}

void MapSerial::saveMap() {
    
    std::string fullpath = getMapDir();
    
    std::vector<std::string> out;
    auto filter = "JSON file(json)|*.json|All files (*.*)|*.*";
    DiPathLib::SaveFileDialog(nullptr, "Save map", fullpath+"/local", "", filter, 0, out);
    
    if (out.empty()) {
        return;
    }
    
    auto filename = out[0];
    
    saveMap(filename.c_str());
}

void MapSerial::loadMap(const char* filename) {
    int maxID = 0;
    
    auto fp = fopen(filename, "r");
    if(!fp) {
        CCLOG("Warning: cannot access the map file : %s", filename);
        return;
    }
    
    fseek(fp, 0, SEEK_END);
    auto fsize = ftell(fp);
    rewind(fp);
    char* buffer = new char[fsize];
    fread(buffer, 1, fsize, fp);
    fclose(fp);
    
    Document d;
    d.Parse<kParseDefaultFlags>(buffer);
    
    std::map<BlockBase*, std::vector<int>> pregroups;
    
    GameScene::Scene->clean(false);//
    
    if(d["backgroundColor"].IsString()) {
        GameScene::Scene->setBackgroundColor(str2Color(d["backgroundColor"].GetString()));
    }SHOW_WARNING
    
    if(d["heroColor"].IsString()) {
        GameScene::Scene->mBlockColors[0] = str2Color(d["heroColor"].GetString());
    }SHOW_WARNING
    
    if(d["normalBlockColor"].IsString()) {
        GameScene::Scene->mBlockColors[1] = str2Color(d["normalBlockColor"].GetString());
    }SHOW_WARNING
    
    if(d["deathBlockColor"].IsString()) {
        GameScene::Scene->mBlockColors[2] = str2Color(d["deathBlockColor"].GetString());
    }SHOW_WARNING
    
    if(d["deathCircleColor"].IsString()) {
        GameScene::Scene->mBlockColors[3] = str2Color(d["deathCircleColor"].GetString());
    }SHOW_WARNING
    
    if(d["buttonColor"].IsString()) {
        GameScene::Scene->mBlockColors[4] = str2Color(d["buttonColor"].GetString());
    }SHOW_WARNING
    
    if(d["pushableBlockColor"].IsString()) {
        GameScene::Scene->mBlockColors[5] = str2Color(d["pushableBlockColor"].GetString());
    }SHOW_WARNING
    
    if(d["spawnPosition"].IsString()) {
        GameScene::Scene->mSpawnPoint->setPosition(str2Vec(d["spawnPosition"].GetString()));
    }SHOW_WARNING
    
    if(d["blocks"].IsArray()) {
        auto size = d["blocks"].Size();
        
        for(auto i = 0; i < size; ++i) {
            auto& var = d["blocks"][i];
            
            int id = 0;
            Size size;
            Vec2 pos;
            bool pickable = true;
            int rotSpeed = 0;
            BlockKind kind = KIND_BLOCK;
            
            if(var["id"].IsInt()){
                id = var["id"].GetInt();
                maxID = std::max(id, maxID);
            }SHOW_WARNING
            
            if(var["size"].IsString()){
                size = str2Size(var["size"].GetString());
                size.width = std::max(size.width, 0.5f);
                size.height = std::max(size.height, 0.5f);
            }SHOW_WARNING
            
            if(var["position"].IsString()){
                pos = str2Vec(var["position"].GetString());
            }SHOW_WARNING
            
            if(var["pickable"].IsBool()){
                pickable = var["pickable"].GetBool();
            }SHOW_WARNING
            
            if(var["rotatespeed"].IsNumber()){
                rotSpeed = var["rotatespeed"].GetInt();
            }
            
            if(var["kind"].IsString()){
                kind = str2Kind(var["kind"].GetString());
            }SHOW_WARNING
            
            BlockBase* block = new BlockBase();
            block->create(pos,size);
            block->addToScene(GameScene::Scene);
            block->setKind(kind);
            block->mCanPickup = pickable;
            block->mID = id;
            block->mRotationSpeed = rotSpeed;
            block->updateIDLabel();
            block->reset();
            
            GameScene::Scene->mBlockTable[block->getSprite()] = block;
            GameScene::Scene->mBlocks[block->mID] = block;
            
            if(kind == KIND_BUTTON) {
                if(var["direction"].IsString()){
                    block->mButton->mDir = str2Direction(var["direction"].GetString());
                    block->mButton->updateHelper();
                }SHOW_WARNING
                
                if(var["canRestore"].IsBool()){
                    block->mButton->mCanRestore = var["canRestore"].GetBool();
                    block->mButton->updateHelper();
                }SHOW_WARNING
                
                if(var["pushedEvent"].IsString()){
                    block->mButton->mPushedEvent = var["pushedEvent"].GetString();
                }
                if(var["restoredEvent"].IsString()){
                    block->mButton->mRestoredEvent = var["restoredEvent"].GetString();
                }
                if(var["pushingEvent"].IsString()){
                    block->mButton->mPushingEvent = var["pushingEvent"].GetString();
                }
            }
            
            if(var["pathSpeed"].IsNumber()) {
                block->mPath.mSpeed = var["pathSpeed"].GetDouble();
            }
            
            if(var["pingpong"].IsBool()) {
                block->mPath.mPingPong = var["pingpong"].GetBool();
            }
            
            if(var["pause"].IsBool()) {
                block->mPath.mPause = var["pause"].GetBool();
            }
            
            if(var["pathWaitTime"].IsNumber()) {
                block->mPath.mPathWaitTime = var["pathWaitTime"].GetDouble();
            }
            
            if(var["pathes"].IsArray()) {
                auto pathsize = var["pathes"].Size();
                for(auto j = 0; j < pathsize; ++j) {
                    auto& pa = var["pathes"][j];
                    Vec2 pos = Vec2::ZERO;
                    float waittime = -1;
                    
                    if(pa["position"].IsString()) {
                        pos = str2Vec(pa["position"].GetString());
                    }SHOW_WARNING
                    
                    if(pa["waittime"].IsNumber()) {
                        waittime = pa["waittime"].GetDouble();
                    }SHOW_WARNING
                    
                    block->mPath.push(pos, waittime);
                }
            }
            
            if(var["groupMembers"].IsArray()) {
                auto memberSize = var["groupMembers"].Size();
                for(auto j = 0; j < memberSize; ++j) {
                    if(var["groupMembers"][j].IsNumber()) {
                        auto id = var["groupMembers"][j].GetInt();
                        pregroups[block].push_back(id);
                    }SHOW_WARNING
                }
            }
        }
    }SHOW_WARNING
    
    CC_ASSERT(GameScene::Scene->mGroups.empty());
    // process groups
    for(auto gi : pregroups) {
        for(auto idi : gi.second) {
            auto m = GameScene::Scene->findBlock(idi);
            CC_ASSERT(m);
            GameScene::Scene->mGroups[gi.first].push_back(m);
        }
    }
    
    delete[] buffer;
    BlockBase::mIDCounter = maxID + 1;
    GameScene::Scene->mCurFileName = filename;
    
    // update file
    UILayer::Layer->setFileName(filename);
    //UILayer::Layer->addMessage("File loaded");
    
    GameScene::Scene->enableGame(false,true);
}

void MapSerial::loadMap(bool local) {
    
    std::string fullpath = getMapDir();
    std::vector<std::string> out;
    auto filter = "JSON file(json)|*.json|All files (*.*)|*.*";
    DiPathLib::OpenFileDialog(nullptr, "Open map", fullpath+(local?"/local":"/remote"), "", filter, 0, out);
    
    if (out.empty()) {
        return;
    }
    
    auto filename = out[0];
    
    loadMap(filename.c_str());
}

void MapSerial::saveRemoteMaps() {
    HttpHelper::getAllMaps();
}

void MapSerial::afterLoadRemoteMaps() {
    std::smatch base_match;
    std::regex rx("^(\\S)*\\.json$");
    
    for(auto& m : HttpHelper::sAllMaps) {
        auto ret = std::regex_match(m.name, base_match, rx);
        if(!ret) {
            m.name += ".json";
        }
        
        std::string fullpath = getMapDir();
        
        if (fullpath.size() == 0) {
            CCLOG("Warning: cannot locate the maps folder!");
            continue;
        }
        
        fullpath += "/remote/";
        fullpath += m.name;
        
        auto fp = fopen(fullpath.c_str(), "w+");
        
        if (!fp) {
            CCLOG("Warning: cannot access the map file : %s", fullpath.c_str());
            continue;
        }
        
        fprintf(fp, "%s", m.content.c_str());
        
        fclose(fp);
    }
}

const char* MapSerial::getMapDir() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    static std::string fullpath;
    auto env = getenv("XCODE_PROJ_DIR"); // manually added
    if(env) {
        fullpath = env;
        fullpath += "/../Resources/maps";
    }else {
        fullpath = FileUtils::getInstance()->fullPathForFilename("maps");
    }
#else
    static std::string fullpath = FileUtils::getInstance()->fullPathForFilename("maps");
#endif
    return fullpath.c_str();
}