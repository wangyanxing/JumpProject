//
//  MapSerial.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/16/14.
//
//

#include "MapSerial.h"
#include "EditorScene.h"
#include "GameScene.h"
#include "HttpHelper.h"
#include "Shadows.h"
#include "PathLib.h"
#include "UILayer.h"
#include "UIColorEditor.h"
#include "VisibleRect.h"

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
    
    int size = (int)hex.size();
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

std::string followMode2Str(FollowMode v) {
    static std::string names[] = {
        "\"CENTER\"",
        "\"UP\"",
        "\"DOWN\"",
        "\"LEFT\"",
        "\"RIGHT\""
    };
    return names[v];
}

FollowMode str2FollowMode(const string& v) {
    static std::map<std::string, FollowMode> modes = {
        {"CENTER",F_CENTER},
        {"UP",F_UP},
        {"DOWN",F_DOWN},
        {"LEFT",F_LEFT},
        {"RIGHT",F_RIGHT}
    };
    
    if(!modes.count(v)) {
        CCLOG("Invalid kind: %s", v.c_str());
        return F_CENTER;
    }
    return modes[v];
}

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

void MapSerial::savePalette(const char* file){
	time_t rawtime;
	struct tm * ptm;
	time(&rawtime);
	ptm = gmtime(&rawtime);
	std::string timestr = asctime(ptm);
	timestr.resize(timestr.size() - 1);

	std::string author = "unknown";

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	TCHAR username[UNLEN + 1];
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

	INDENT_1 ss << "\"palette\": [ \n";

	for (auto it = GameLogic::Game->mPalette.begin(); it != GameLogic::Game->mPalette.end(); ++it){
		if (it != GameLogic::Game->mPalette.begin()){
			ss << ", \n";
		}
		INDENT_2 ss << "{\n";
		INDENT_3 ss << "\"index\": " << it->first << ", \n";
		INDENT_3 ss << "\"color\": " << colorStr(it->second) << " \n";
		INDENT_2 ss << "}";
	}

	INDENT_1 ss << "] \n";

	ss << "}";

	auto fp = fopen(file, "w+");
	if (!fp) {
		CCLOG("Warning: cannot access the map file : %s", file);
		return;
	}
	fprintf(fp, "%s", ss.str().c_str());
	fclose(fp);
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
    float ratio = VisibleRect::right().x / VisibleRect::top().y;
    INDENT_1 ss << "\"ratio\": " << ratio; RT_LINE
    
	INDENT_1 ss << "\"backgroundColor\": " << colorStr(GameLogic::Game->mBackgroundColor); RT_LINE
#if 0
    INDENT_1 ss << "\"heroColor\": " << colorStr(GameLogic::Game->mBlockColors[0]); RT_LINE
#endif
    INDENT_1 ss << "\"heroColorIndex\": " << GameLogic::Game->mHero->mPaletteIndex; RT_LINE
	INDENT_1 ss << "\"normalBlockColor\": " << colorStr(GameLogic::Game->mBlockColors[1]); RT_LINE
	INDENT_1 ss << "\"deathBlockColor\": " << colorStr(GameLogic::Game->mBlockColors[2]); RT_LINE
	INDENT_1 ss << "\"deathCircleColor\": " << colorStr(GameLogic::Game->mBlockColors[3]); RT_LINE
	INDENT_1 ss << "\"buttonColor\": " << colorStr(GameLogic::Game->mBlockColors[4]); RT_LINE
	INDENT_1 ss << "\"pushableBlockColor\": " << colorStr(GameLogic::Game->mBlockColors[5]); RT_LINE
	INDENT_1 ss << "\"spawnPosition\": " << vec2Str(GameLogic::Game->mSpawnPos); RT_LINE
	INDENT_1 ss << "\"lightPosition\": " << vec2Str(GameLogic::Game->mShadows->mOriginLightPos); RT_LINE
	INDENT_1 ss << "\"lightMoving\": " << bool2Str(GameLogic::Game->mShadows->mShadowMovingEnable); RT_LINE
	INDENT_1 ss << "\"shadowDarkness\": " << GameLogic::Game->mShadows->mShadowDarkness; RT_LINE
    INDENT_1 ss << "\"useSoftShadow\": " << bool2Str(GameLogic::Game->mShadows->mUseSoftShadow); RT_LINE
	INDENT_1 ss << "\"gradientCenter\": " << vec2Str(GameLogic::Game->mGradientCenter); RT_LINE
	INDENT_1 ss << "\"gradientColorSrc\": " << colorStr(GameLogic::Game->mGradientColorSrc); RT_LINE
	INDENT_1 ss << "\"gradientColorDst\": " << colorStr(GameLogic::Game->mGradientColorDst); RT_LINE

	INDENT_1 ss << "\"paletteFile\": \"" << GameLogic::Game->mPaletteFileName << "\""; RT_LINE
	savePalette(GameLogic::Game->mPaletteFileName.c_str());

    INDENT_1 ss << "\"blocks\": [ \n";
    
    for (auto it = GameLogic::Game->mBlocks.begin(); it != GameLogic::Game->mBlocks.end(); ++it) {
    auto b = it->second;

    if (it != GameLogic::Game->mBlocks.begin()) {
        INDENT_2 ss << "},\n";
    }

    INDENT_2 ss << "{ \n";
	INDENT_3 ss << "\"id\": " << b->mID; RT_LINE
		INDENT_3 ss << "\"size\": " << size2Str(b->mRestoreSize); RT_LINE
		INDENT_3 ss << "\"position\": " << vec2Str(b->mRestorePosition); RT_LINE
		INDENT_3 ss << "\"pickable\": " << bool2Str(b->mCanPickup); RT_LINE
		INDENT_3 ss << "\"rotatespeed\": " << b->mRotationSpeed; RT_LINE
        INDENT_3 ss << "\"shadowLength\": " << b->mShadowLength; RT_LINE
        INDENT_3 ss << "\"shadowEnable\": " << bool2Str(b->mCastShadow); RT_LINE
		INDENT_3 ss << "\"paletteIndex\": " << b->mPaletteIndex; RT_LINE
        INDENT_3 ss << "\"flipUV\": " << bool2Str(b->mUVFlipped); RT_LINE
        INDENT_3 ss << "\"textureName\": \"" << b->mTextureName << "\""; RT_LINE
        if (!b->mUserData.empty()) {
            INDENT_3 ss << "\"userData\": \"" << b->mUserData << "\""; RT_LINE
        }

        if (b->mKind == KIND_DEATH_CIRCLE || b->mKind == KIND_DEATH)
        {
        INDENT_3 ss << "\"triggerEvents\": [";
        for (size_t i = 0; i < b->mTriggerEvents.size(); i++)
        {
            ss << "\"" + b->mTriggerEvents[i] + "\"";
            if (i != b->mTriggerEvents.size() - 1) ss << ", ";
        }

        ss << "],\n";
        }

        if(b->mKind == KIND_BUTTON) {
            INDENT_3 ss << "\"direction\": " << direction2Str(b->mButton->mDir); RT_LINE
            INDENT_3 ss << "\"canRestore\": " << bool2Str(b->mButton->mCanRestore); RT_LINE
            
            INDENT_3 ss << "\"pushedEvent\": " << "\""<< b->mButton->mPushedEvent << "\""; RT_LINE
            INDENT_3 ss << "\"restoredEvent\": " << "\""<< b->mButton->mRestoredEvent << "\""; RT_LINE
            INDENT_3 ss << "\"pushingEvent\": " << "\""<< b->mButton->mPushingEvent << "\""; RT_LINE
        }
        
        // group
        auto ig = GameLogic::Game->mGroups.find(b);
        if(ig != GameLogic::Game->mGroups.end() && !ig->second.empty()) {
            INDENT_3 ss << "\"groupFollowMode\": " << followMode2Str(b->mFollowMode); RT_LINE
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
                INDENT_5 ss << "\"width\": " << p.width;RT_LINE
                INDENT_5 ss << "\"height\": " << p.height;RT_LINE
                INDENT_5 ss << "\"waittime\": " << p.waitTime << "\n";
                INDENT_4 ss << "}";
                if(i != b->mPath.getNumPoints()-1) RT_LINE
                    else ss << "\n";
            }
            
            INDENT_3 ss << "],\n";
        }
        INDENT_3 ss << "\"kind\": " << kind2Str(b->mKind) << "\n";
    }
    if(!GameLogic::Game->mBlocks.empty()) {
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
#if EDITOR_MODE
    EditorScene::Scene->mCurFileName = file;
#endif
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

#if EDITOR_MODE
void MapSerial::saveMap() {
    
    std::string fullpath = getMapDir();
    
    std::vector<std::string> out;
    auto filter = "JSON file(json)|*.json|All files (*.*)|*.*";
    DiPathLib::SaveFileDialog(nullptr, "Save map", fullpath+"/local", "", filter, 0, out);
    
    if (out.empty()) {
        return;
    }
    
    auto filename = out[0];
    
#   if EDITOR_RATIO == EDITOR_IPAD_MODE
    if(!DiPathLib::EndsWith(filename, "_pad.json"))
        DiPathLib::ReplaceString(filename, ".json", "_pad.json");
#   elif EDITOR_RATIO == EDITOR_IP4_MODE
    if(!DiPathLib::EndsWith(filename, "_ip4.json"))
        DiPathLib::ReplaceString(filename, ".json", "_ip4.json");
#   endif
    
    saveMap(filename.c_str());
}
#endif

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
    
#if EDITOR_MODE
    EditorScene::Scene->clean(false);//
#else
    GameLogic::Game->clean();
#endif
    
    if(d["backgroundColor"].IsString()) {
        GameLogic::Game->setBackgroundColor(str2Color(d["backgroundColor"].GetString()));
    }SHOW_WARNING
    
#if 0
	if (d["heroColor"].IsString()) {
		GameLogic::Game->mBlockColors[0] = str2Color(d["heroColor"].GetString());
	}
#endif
    if (d["heroColorIndex"].IsInt()) {
        GameLogic::Game->mHero->setColor(d["heroColorIndex"].GetInt());
    }

	if (d["normalBlockColor"].IsString()) {
		GameLogic::Game->mBlockColors[1] = str2Color(d["normalBlockColor"].GetString());
	}SHOW_WARNING

	if (d["deathBlockColor"].IsString()) {
		GameLogic::Game->mBlockColors[2] = str2Color(d["deathBlockColor"].GetString());
	}SHOW_WARNING

	if (d["deathCircleColor"].IsString()) {
		GameLogic::Game->mBlockColors[3] = str2Color(d["deathCircleColor"].GetString());
	}SHOW_WARNING

	if (d["buttonColor"].IsString()) {
		GameLogic::Game->mBlockColors[4] = str2Color(d["buttonColor"].GetString());
	}SHOW_WARNING

	if (d["pushableBlockColor"].IsString()) {
		GameLogic::Game->mBlockColors[5] = str2Color(d["pushableBlockColor"].GetString());
	}SHOW_WARNING

	if (d["spawnPosition"].IsString()) {
		GameLogic::Game->mSpawnPos = str2Vec(d["spawnPosition"].GetString());
	}SHOW_WARNING

	if (d["lightPosition"].IsString()) {
		GameLogic::Game->mShadows->mLightPos = str2Vec(d["lightPosition"].GetString());
		GameLogic::Game->mShadows->mOriginLightPos = GameLogic::Game->mShadows->mLightPos;
	}

	if (d["lightMoving"].IsBool()) {
		GameLogic::Game->mShadows->mShadowMovingEnable = d["lightMoving"].GetBool();
	}

	if (d["shadowDarkness"].IsNumber()) {
		GameLogic::Game->mShadows->mShadowDarkness = d["shadowDarkness"].GetDouble();
	}
    
    if (d["useSoftShadow"].IsBool()) {
        GameLogic::Game->mShadows->mUseSoftShadow = d["useSoftShadow"].GetBool();
    }

	Vec2 gradientCenter(0, 0);
	Color3B colorSrc(50, 201, 219);
	Color3B colorDst(30, 181, 199);
	if (d["gradientCenter"].IsString()) {
		gradientCenter = str2Vec(d["gradientCenter"].GetString());
	}
	if (d["gradientColorSrc"].IsString()) {
		colorSrc = str2Color(d["gradientColorSrc"].GetString());
	}
	if (d["gradientColorDst"].IsString()) {
		colorDst = str2Color(d["gradientColorDst"].GetString());
	}

	GameLogic::Game->setBackGradientCenter(gradientCenter);
	GameLogic::Game->setBackGradientColor(colorSrc, colorDst);

	std::string palletteFileName = GameLogic::Game->mPaletteFileName;
	if (d["paletteFile"].IsString()){
		palletteFileName = d["paletteFile"].GetString();
	}

	palletteFileName = FileUtils::getInstance()->fullPathForFilename(palletteFileName.c_str());
	auto fpPallette = fopen(palletteFileName.c_str(), "r");

	if (!fpPallette) {
		CCLOG("Warning: cannot access the palette file : %s\nUsing default value", filename);
	}
	else {
		fseek(fpPallette, 0, SEEK_END);
		auto pFsize = ftell(fpPallette);
		rewind(fpPallette);
		char* fpBuffer = new char[pFsize];
		fread(fpBuffer, 1, pFsize, fpPallette);
		fclose(fpPallette);

		Document dPalette;
		dPalette.Parse<kParseDefaultFlags>(fpBuffer);

		if (dPalette["palette"].IsArray()){
			auto size = dPalette["palette"].Size();
			if (size > 0){
				GameLogic::Game->mPalette.clear();
	#if EDITOR_MODE
				UIColorEditor::colorEditor->cleanColors();
	#endif
			}

			for (auto i = 0; i < size; i++){
				auto& palette = dPalette["palette"][i];
				if (palette["index"].IsInt() && palette["color"].IsString()){
					GameLogic::Game->mPalette.insert(std::pair<int, Color3B>(palette["index"].GetInt(), str2Color(palette["color"].GetString())));
	#if EDITOR_MODE
					UIColorEditor::colorEditor->addColor(palette["index"].GetInt(), str2Color(palette["color"].GetString()));
	#endif
				}

			}

	#if EDITOR_MODE
			UIColorEditor::colorEditor->updateColorButtonDisplay();
	#endif
		}
	}

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
            std::string textureName = "images/saw3.png";
            std::string userData;
			int paletteIndex = -1;
            bool flipuv = false;
            std::string triggerEvent = "";
            float shadowLeng = 100;
            bool shadowEnable = true;
            
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
            
            if (var["flipUV"].IsBool()){
                flipuv = var["flipUV"].GetBool();
            }

            if (var["textureName"].IsString()){
                textureName = var["textureName"].GetString();
            }
            
            if (var["userData"].IsString()){
                userData = var["userData"].GetString();
            }

			if (var["paletteIndex"].IsNumber()){
				paletteIndex = var["paletteIndex"].GetInt();
			}
            
            if (var["shadowLength"].IsNumber()) {
                shadowLeng = var["shadowLength"].GetDouble();
            }
            
            if (var["shadowEnable"].IsBool()) {
                shadowEnable = var["shadowEnable"].GetBool();
            }
            
#if EDITOR_MODE
            if (!pickable) {
                float width = VisibleRect::right().x;
                float height = VisibleRect::top().y;
                float frameSize = 10;
                
                if(id == 1) {
                    pos = Vec2(width/2,frameSize/2);
                    size = Size(width, frameSize);
                } else if(id == 2) {
                    pos = Vec2(width/2,height - frameSize/2);
                    size = Size(width, frameSize);
                } else if(id == 3) {
                    pos = Vec2(frameSize/2,height/2);
                    size = Size(frameSize, height);
                } else if(id == 4) {
                    pos = Vec2(width - frameSize/2,height/2);
                    size = Size(frameSize, height);
                }
            }
#endif
			
            BlockBase* block = new BlockBase();
            block->create(pos,size);
#if EDITOR_MODE
            block->addToScene(EditorScene::Scene);
#else
            block->addToScene(GameScene::Scene);
#endif
            block->mTextureName = textureName;
			if (paletteIndex!=-1)
				block->setColor(paletteIndex);

            if (kind == KIND_DEATH_CIRCLE || kind == KIND_DEATH)
            {
                if (var["triggerEvents"].IsArray())
                {
                    auto triggerEventSize = var["triggerEvents"].Size();

                    for (auto i = 0; i < triggerEventSize; i++)
                    {
                        std::string triggerEvent = var["triggerEvents"][i].GetString();
                        block->mTriggerEvents.push_back(triggerEvent);
                    }
                }
            }

            block->setKind(kind);
            block->mCanPickup = pickable;
            block->mID = id;
            block->mRotationSpeed = rotSpeed;
            block->mShadowLength = shadowLeng;
            block->mCastShadow = shadowEnable;
            block->mUVFlipped = flipuv;
            block->mUserData = userData;
            
#if EDITOR_MODE
            block->updateIDLabel();
#endif
            block->reset();
            
            GameLogic::Game->mBlockTable[block->getSprite()] = block;
            GameLogic::Game->mBlocks[block->mID] = block;
            
            if(kind == KIND_BUTTON) {
                if(var["direction"].IsString()){
                    block->mButton->mDir = str2Direction(var["direction"].GetString());
#if EDITOR_MODE
                    block->mButton->updateHelper();
#endif
                }SHOW_WARNING
                
                if(var["canRestore"].IsBool()){
                    block->mButton->mCanRestore = var["canRestore"].GetBool();
#if EDITOR_MODE
                    block->mButton->updateHelper();
#endif
                }
                
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
                    float width = 1, height = 1;
                    
                    if(pa["position"].IsString()) {
                        pos = str2Vec(pa["position"].GetString());
                    }SHOW_WARNING
                    
                    if(pa["waittime"].IsNumber()) {
                        waittime = pa["waittime"].GetDouble();
                    }SHOW_WARNING
                    
                    if(pa["width"].IsNumber()) {
                        width = pa["width"].GetDouble();
                    }
                    
                    if(pa["height"].IsNumber()) {
                        height = pa["height"].GetDouble();
                    }
                    
                    block->mPath.push(pos, waittime, width, height);
                }
            }
            
            if(var["groupMembers"].IsArray()) {
                if(var["groupFollowMode"].IsString()) {
                    block->mFollowMode = str2FollowMode(var["groupFollowMode"].GetString());
                }SHOW_WARNING
                
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
    
    CC_ASSERT(GameLogic::Game->mGroups.empty());
    // process groups
    for(auto gi : pregroups) {
        for(auto idi : gi.second) {
            auto m = GameLogic::Game->findBlock(idi);
            CC_ASSERT(m);
            GameLogic::Game->mGroups[gi.first].push_back(m);
        }
    }
    
    delete[] buffer;
    BlockBase::mIDCounter = maxID + 1;
#if EDITOR_MODE
    std::string fixedfilename = filename;
    
#   if EDITOR_RATIO == EDITOR_IPAD_MODE
    if(!DiPathLib::EndsWith(fixedfilename, "_pad.json"))
        DiPathLib::ReplaceString(fixedfilename, ".json", "_pad.json");
#   elif EDITOR_RATIO == EDITOR_IP4_MODE
    if(!DiPathLib::EndsWith(fixedfilename, "_ip4.json"))
        DiPathLib::ReplaceString(fixedfilename, ".json", "_ip4.json");
#   endif
    
    EditorScene::Scene->mCurFileName = fixedfilename;
    EditorScene::Scene->mSpawnPoint->setPosition(GameLogic::Game->mSpawnPos);
    EditorScene::Scene->mLightPoint->setPosition(GameLogic::Game->mShadows->mOriginLightPos);
    EditorScene::Scene->mGradientCenterPoint->setPosition(GameLogic::Game->mGradientCenter);
    
    UILayer::Layer->setFileName(fixedfilename.c_str());
    UILayer::Layer->addMessage("File loaded");
#endif
    
#if EDITOR_MODE
    EditorScene::Scene->enableGame(false,true);
#endif
}

#if EDITOR_MODE
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
#endif

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
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        fullpath = FileUtils::getInstance()->getWritablePath();
        fullpath += m.name;
#else
        fullpath += "/remote/";
        fullpath += m.name;
#endif
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