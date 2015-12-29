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
#include "TimeEvent.h"
#include "Palette.h"
#include "Hero.h"
#include "ControlPad.h"
#include "BlockRenderer.h"

#include "MapSerialUtils.inl"

void MapSerial::savePalette(const char *file) {
  time_t rawtime;
  struct tm *ptm;
  time(&rawtime);
  ptm = gmtime(&rawtime);
  std::string timestr = asctime(ptm);
  timestr.resize(timestr.size() - 1);

  std::string author = "unknown";

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  TCHAR username[UNLEN + 1];
  DWORD size = UNLEN + 1;
  GetUserName((TCHAR *) username, &size);
  author = username;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  author = getlogin();
#endif

  stringstream ss;
  ss << "{\n";

  INDENT_1
  ss << "\"author\": " << "\"" << author << "\"";
  RT_LINE
  INDENT_1
  ss << "\"time\": " << "\"" << timestr << "\"";
  RT_LINE

  INDENT_1
  ss << "\"palette\": [ \n";

  const auto &palette = Palette::getInstance()->getPalette();
  for (auto it = palette.begin(); it != palette.end(); ++it) {
    if (it != palette.begin()) {
      ss << ", \n";
    }
    INDENT_2
    ss << "{\n";
    INDENT_3
    ss << "\"index\": " << it->first << ", \n";
    INDENT_3
    ss << "\"color\": " << colorStr(it->second) << " \n";
    INDENT_2
    ss << "}";
  }

  INDENT_1
  ss << "] \n";

  ss << "}";

  auto fp = fopen(file, "w+");
  if (!fp) {
    CCLOGWARN("Warning: cannot access the map file : %s", file);
    return;
  }
  fprintf(fp, "%s", ss.str().c_str());
  fclose(fp);
}

void MapSerial::saveMap(const char *file) {
  time_t rawtime;
  struct tm *ptm;
  time(&rawtime);
  ptm = gmtime(&rawtime);
  std::string timestr = asctime(ptm);
  timestr.resize(timestr.size() - 1);

  std::string author = "unknown";

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  TCHAR username[UNLEN + 1];
  DWORD size = UNLEN + 1;
  GetUserName((TCHAR *) username, &size);
  author = username;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  author = getlogin();
#endif

  stringstream ss;
  ss << "{\n";

  auto palette = Palette::getInstance();

  INDENT_1
  ss << "\"author\": " << "\"" << author << "\"";
  RT_LINE
  INDENT_1
  ss << "\"time\": " << "\"" << timestr << "\"";
  RT_LINE
  float ratio = VisibleRect::right().x / VisibleRect::top().y;
  INDENT_1
  ss << "\"ratio\": " << ratio;
  RT_LINE

  INDENT_1
  ss << "\"backgroundColor\": " << colorStr(GameLogic::Game->mBackgroundColor);
  RT_LINE

  INDENT_1
  ss << "\"heroColorIndex\": " << GameLogic::Game->mHero->mPaletteIndex;
  RT_LINE
  INDENT_1
  ss << "\"normalBlockColor\": " << colorStr(palette->getDefaultBlockColors(KIND_BLOCK));
  RT_LINE
  INDENT_1
  ss << "\"deathBlockColor\": " << colorStr(palette->getDefaultBlockColors(KIND_DEATH));
  RT_LINE
  INDENT_1
  ss << "\"deathCircleColor\": " << colorStr(palette->getDefaultBlockColors(KIND_DEATH_CIRCLE));
  RT_LINE
  INDENT_1
  ss << "\"buttonColor\": " << colorStr(palette->getDefaultBlockColors(KIND_BUTTON));
  RT_LINE
  INDENT_1
  ss << "\"pushableBlockColor\": " << colorStr(palette->getDefaultBlockColors(KIND_PUSHABLE));
  RT_LINE
  INDENT_1
  ss << "\"spawnPosition\": " << vec2Str(GameLogic::Game->mSpawnPos);
  RT_LINE

#if USE_SHADOW
  INDENT_1
  ss << "\"lightPosition\": " << vec2Str(GameLogic::Game->mShadows->mOriginLightPos);
  RT_LINE
  INDENT_1
  ss << "\"lightMoving\": " << bool2Str(GameLogic::Game->mShadows->mShadowMovingEnable);
  RT_LINE
  INDENT_1
  ss << "\"shadowDarkness\": " << GameLogic::Game->mShadows->mShadowDarkness;
  RT_LINE
#endif

  INDENT_1
  ss << "\"gradientCenter\": " << vec2Str(GameLogic::Game->mGradientCenter);
  RT_LINE
  INDENT_1
  ss << "\"gradientColorSrc\": " << colorStr(GameLogic::Game->mGradientColorSrc);
  RT_LINE
  INDENT_1
  ss << "\"gradientColorDst\": " << colorStr(GameLogic::Game->mGradientColorDst);
  RT_LINE
  INDENT_1
  ss << "\"paletteFile\": \"" << palette->getPaletteFileName() << "\"";
  RT_LINE

  INDENT_1
  ss << "\"fx\": [ \n";
  for (size_t fi = 0; fi < GameLogic::Game->mFxList.size(); ++fi) {
    INDENT_2
    ss << "\"" << GameLogic::Game->mFxList[fi] << "\"";
    if (fi != GameLogic::Game->mFxList.size() - 1) {
      ss << ",";
    }
    ss << "\n";
  }
  INDENT_1
  ss << "]";
  RT_LINE

  INDENT_1
  ss << "\"stars\": [ \n";
  for (size_t fi = 0; fi < GameLogic::Game->mStarList.size(); ++fi) {
    INDENT_2
    ss << vec2Str(GameLogic::Game->mStarList[fi]);
    if (fi != GameLogic::Game->mStarList.size() - 1) {
      ss << ",";
    }
    ss << "\n";
  }
  INDENT_1
  ss << "]";
  RT_LINE

  INDENT_1
  ss << "\"blocks\": [ \n";

  for (auto it = GameLogic::Game->mBlocks.begin(); it != GameLogic::Game->mBlocks.end(); ++it) {
    auto b = it->second;

    if (it != GameLogic::Game->mBlocks.begin()) {
      INDENT_2
      ss << "},\n";
    }

    INDENT_2
    ss << "{ \n";
    INDENT_3
    ss << "\"id\": " << b->mID;
    RT_LINE
    INDENT_3
    ss << "\"size\": " << size2Str(b->mRestoreSize);
    RT_LINE
    INDENT_3
    ss << "\"position\": " << vec2Str(b->mRestorePosition);
    RT_LINE
    INDENT_3
    ss << "\"pickable\": " << bool2Str(b->mCanPickup);
    RT_LINE
    INDENT_3
    ss << "\"removable\": " << bool2Str(b->mCanDelete);
    RT_LINE
    INDENT_3
    ss << "\"rotatespeed\": " << b->mRotationSpeed;
    RT_LINE
    INDENT_3
    ss << "\"shadowLength\": " << b->mShadowLength;
    RT_LINE
    INDENT_3
    ss << "\"shadowEnable\": " << bool2Str(b->mCastShadow);
    RT_LINE
    INDENT_3
    ss << "\"shadowLayer\": " << b->mShadowLayerID;
    RT_LINE
    INDENT_3
    ss << "\"paletteIndex\": " << b->mPaletteIndex;
    RT_LINE
    INDENT_3
    ss << "\"flipUV\": " << bool2Str(b->mUVFlipped);
    RT_LINE
    INDENT_3
    ss << "\"textureName\": \"" << b->mTextureName << "\"";
    RT_LINE
    if (!b->mUserData.empty()) {
      INDENT_3
      ss << "\"userData\": \"" << b->mUserData << "\"";
      RT_LINE
    }

    if (b->mKind == KIND_DEATH_CIRCLE || b->mKind == KIND_DEATH) {
      INDENT_3
      ss << "\"triggerEvents\": [";
      for (size_t i = 0; i < b->mTriggerEvents.size(); i++) {
        ss << "\"" + b->mTriggerEvents[i] + "\"";
        if (i != b->mTriggerEvents.size() - 1) ss << ", ";
      }
      ss << "],\n";
    }

    INDENT_3
    ss << "\"initEvents\": [";
    for (size_t i = 0; i < b->mInitialEvents.size(); i++) {
      ss << "\"" + b->mInitialEvents[i] + "\"";
      if (i != b->mInitialEvents.size() - 1) {
        ss << ", ";
      }
    }
    ss << "],\n";

    if (b->mKind == KIND_BUTTON) {
      INDENT_3
      ss << "\"direction\": " << direction2Str(b->mButton->mDir);
      RT_LINE
      INDENT_3
      ss << "\"canRestore\": " << bool2Str(b->mButton->mCanRestore);
      RT_LINE

      INDENT_3
      ss << "\"pushedEvent\": " << "\"" << b->mButton->mPushedEvent << "\"";
      RT_LINE
      INDENT_3
      ss << "\"restoredEvent\": " << "\"" << b->mButton->mRestoredEvent << "\"";
      RT_LINE
      INDENT_3
      ss << "\"pushingEvent\": " << "\"" << b->mButton->mPushingEvent << "\"";
      RT_LINE
    }

    // group
    auto ig = GameLogic::Game->mGroups.find(b);
    if (ig != GameLogic::Game->mGroups.end() && !ig->second.empty()) {
      INDENT_3
      ss << "\"groupFollowMode\": " << followMode2Str(b->mFollowMode);
      RT_LINE
      INDENT_3
      ss << "\"groupMembers\": [";
      for (size_t i = 0; i < ig->second.size(); ++i) {
        ss << ig->second[i]->mID;
        if (i != ig->second.size() - 1) ss << ", ";
      }
      ss << "],\n";
    }

    // Path
    if (!b->mPath.empty()) {
      INDENT_3
      ss << "\"pathSpeed\": " << b->mPath.mSpeed;
      RT_LINE
      INDENT_3
      ss << "\"pingpong\": " << bool2Str(b->mPath.mPingPong);
      RT_LINE
      INDENT_3
      ss << "\"pause\": " << bool2Str(b->mPath.mPause);
      RT_LINE
      INDENT_3
      ss << "\"pathWaitTime\": " << b->mPath.mPathWaitTime;
      RT_LINE
      INDENT_3
      ss << "\"pathes\": [ \n";

      for (size_t i = 0; i < b->mPath.getNumPoints(); ++i) {
        INDENT_4
        ss << "{ \n";
        const auto &p = b->mPath.getPoint(i);
        INDENT_5
        ss << "\"position\": " << vec2Str(p.pt);
        RT_LINE
        INDENT_5
        ss << "\"width\": " << p.width;
        RT_LINE
        INDENT_5
        ss << "\"height\": " << p.height;
        RT_LINE
        INDENT_5
        ss << "\"waittime\": " << p.waitTime << "\n";
        INDENT_4
        ss << "}";
        if (i != b->mPath.getNumPoints() - 1) {
          RT_LINE
        } else {
          ss << "\n";
        }
      }

      INDENT_3
      ss << "],\n";
    }
    INDENT_3
    ss << "\"kind\": " << kind2Str(b->mKind) << "\n";
  }
  if (!GameLogic::Game->mBlocks.empty()) {
    INDENT_2
    ss << "}\n";
  }

  INDENT_1
  ss << "],\n";

  INDENT_1
  ss << "\"timeEvents\": [\n";

  for (auto it = GameLogic::Game->mTimeEvents.begin();
       it != GameLogic::Game->mTimeEvents.end(); ++it) {
    if (it != GameLogic::Game->mTimeEvents.begin()) {
      INDENT_2
      ss << "},\n";
    }
    auto event = (*it);
    INDENT_2
    ss << "{\n";

    INDENT_3
    ss << "\"initDelay\": " << event.mInitDelay << ",\n";
    INDENT_3
    ss << "\"loop\": " << event.mLoop << ",\n";

    INDENT_3
    ss << "\"events\": [\n";

    for (auto eit = event.mEventPoints.begin(); eit != event.mEventPoints.end(); ++eit) {
      if (eit != event.mEventPoints.begin()) {
        ss << "},\n";
      }

      INDENT_4
      ss << "{ ";
      ss << "\"event\": [";

      auto command = (*eit);
      for (auto comIt = command.mEvents.begin(); comIt != command.mEvents.end(); ++comIt) {
        if (comIt != command.mEvents.begin()) {
          ss << ", ";
        }
        ss << "\"" << (*comIt) << "\"";
      }
      ss << "], \"delay\": " << (*eit).waitTime;
    }

    if (!event.mEventPoints.empty()) {
      ss << "}\n";
    }
    INDENT_3
    ss << "]\n";
  }

  if (!GameLogic::Game->mTimeEvents.empty()) {
    INDENT_2
    ss << "}\n";
  }

  INDENT_1
  ss << "]\n";

  ss << "}";

  auto fp = fopen(file, "w+");
  if (!fp) {
    CCLOG("Warning: cannot access the map file : %s", file);
    return;
  }
  fprintf(fp, "%s", ss.str().c_str());
  fclose(fp);

  // Update file
#if EDITOR_MODE
  EditorScene::Scene->mCurFileName = file;
#endif
  UILayer::Layer->setFileName(file);
  UILayer::Layer->addMessage("File saved");

  // Save history
  UserDefault::getInstance()->setStringForKey("lastedit", file);
  UserDefault::getInstance()->flush();

  // Save to server
  std::string filename = file;

  const size_t last_slash_idx = filename.find_last_of("\\/");
  if (std::string::npos != last_slash_idx) {
    filename.erase(0, last_slash_idx + 1);
  }
  HttpHelper::updateMap(filename, author, timestr, ss.str());
}

void MapSerial::loadLastEdit() {
  auto file = UserDefault::getInstance()->getStringForKey("lastedit");
  if (file.empty()) {
    return;
  }
  loadMap(file.c_str());
}

#if EDITOR_MODE

void MapSerial::saveMap() {
  std::string fullpath = getMapDir();

  std::vector<std::string> out;
  auto filter = "JSON file(json)|*.json|All files (*.*)|*.*";
  PathLib::saveFileDialog(nullptr, "Save map", fullpath + "/local", "", filter, 0, out);

  if (out.empty()) {
    return;
  }

  auto filename = out[0];
#   if EDITOR_RATIO == EDITOR_IPAD_MODE
  if (!PathLib::endsWith(filename, "_pad.json")) {
    PathLib::replaceString(filename, ".json", "_pad.json");
  }
#   elif EDITOR_RATIO == EDITOR_IP4_MODE
  if (!PathLib::endsWith(filename, "_ip4.json")) {
    PathLib::replaceString(filename, ".json", "_ip4.json");
  }
#   endif

  saveMap(filename.c_str());
}

#endif

void MapSerial::loadMap(const char *filename) {
  if (!FileUtils::getInstance()->isFileExist(filename)) {
    CCLOGWARN("Failed to load map file: %s", filename);
    return;
  }

  CurrentEditingFile = filename;

  int maxID = 0;
  auto buffer = FileUtils::getInstance()->getStringFromFile(filename);

  Document d;
  ParseResult ok = d.Parse<kParseDefaultFlags>(buffer.c_str());
  if (!ok) {
    CCLOGWARN("JSON parse error: %d (%lu)", ok.Code(), ok.Offset());
    CCLOGWARN("%s", buffer.c_str());
  }

  std::map<BlockBase *, std::vector<int>> pregroups;

#if EDITOR_MODE
  EditorScene::Scene->clean(false);
#else
  GameLogic::Game->clean();
#endif

  if (CHECK_STRING(d, "backgroundColor")) {
    GameLogic::Game->setBackgroundColor(str2Color(d["backgroundColor"].GetString()));
  } SHOW_WARNING

  auto palette = Palette::getInstance();

  if (d["normalBlockColor"].IsString()) {
    palette->setDefaultBlockColors(KIND_BLOCK, str2Color(d["normalBlockColor"].GetString()));
  } SHOW_WARNING

  if (d["deathBlockColor"].IsString()) {
    palette->setDefaultBlockColors(KIND_DEATH, str2Color(d["deathBlockColor"].GetString()));
  } SHOW_WARNING

  if (d["deathCircleColor"].IsString()) {
    palette->setDefaultBlockColors(KIND_DEATH_CIRCLE, str2Color(d["deathCircleColor"].GetString()));
  } SHOW_WARNING

  if (d["buttonColor"].IsString()) {
    palette->setDefaultBlockColors(KIND_BUTTON, str2Color(d["buttonColor"].GetString()));
  } SHOW_WARNING

  if (d["pushableBlockColor"].IsString()) {
    palette->setDefaultBlockColors(KIND_PUSHABLE, str2Color(d["pushableBlockColor"].GetString()));
  } SHOW_WARNING

  if (d["spawnPosition"].IsString()) {
    GameLogic::Game->mSpawnPos = str2Vec(d["spawnPosition"].GetString());
  } SHOW_WARNING

#if USE_SHADOW
  if (CHECK_STRING(d, "lightPosition")) {
    GameLogic::Game->mShadows->mLightPos = str2Vec(d["lightPosition"].GetString());
    GameLogic::Game->mShadows->mOriginLightPos = GameLogic::Game->mShadows->mLightPos;
  }

  if (CHECK_BOOL(d, "lightMoving")) {
    GameLogic::Game->mShadows->mShadowMovingEnable = d["lightMoving"].GetBool();
  }

  if (CHECK_NUMBER(d, "shadowDarkness")) {
    GameLogic::Game->mShadows->mShadowDarkness = d["shadowDarkness"].GetDouble();
  }
#endif

  Vec2 gradientCenter(0, 0);
  Color3B colorSrc(50, 201, 219);
  Color3B colorDst(30, 181, 199);

  if (CHECK_STRING(d, "gradientCenter")) {
    gradientCenter = str2Vec(d["gradientCenter"].GetString());
  }
  if (CHECK_STRING(d, "gradientColorSrc")) {
    colorSrc = str2Color(d["gradientColorSrc"].GetString());
  }
  if (CHECK_STRING(d, "gradientColorDst")) {
    colorDst = str2Color(d["gradientColorDst"].GetString());
  }

  GameLogic::Game->setBackGradientCenter(gradientCenter);
  GameLogic::Game->setBackGradientColor(colorSrc, colorDst);

  if (CHECK_ARRAY(d, "fx")) {
    auto fxsize = d["fx"].Size();
    for (auto fi = 0; fi < fxsize; ++fi) {
      auto fxname = d["fx"][fi].GetString();
      GameLogic::Game->mFxList.push_back(fxname);
    }
  }
  GameLogic::Game->loadFxFromList();

  if (CHECK_ARRAY(d, "stars")) {
    auto fxsize = d["stars"].Size();
    for (auto fi = 0; fi < fxsize; ++fi) {
      auto fxname = d["stars"][fi].GetString();
      GameLogic::Game->mStarList.push_back(str2Vec(fxname));
    }
  }
  GameLogic::Game->loadStarFromList();

  std::string paletteFileName = Palette::getInstance()->getPaletteFileName();
  if (CHECK_STRING(d, "paletteFile")) {
    paletteFileName = d["paletteFile"].GetString();
    Palette::getInstance()->setPaletteFileName(paletteFileName);
  }

  paletteFileName = FileUtils::getInstance()->fullPathForFilename(paletteFileName.c_str());
  std::ifstream paletteFileStream(paletteFileName);

  if (!paletteFileStream) {
    CCLOGWARN("Warning: cannot access the palette file : %s\nUsing default value", filename);
  } else {
    std::string paletteBuffer((std::istreambuf_iterator<char>(paletteFileStream)),
                              std::istreambuf_iterator<char>());
    Document dPalette;
    ParseResult ok = dPalette.Parse<kParseDefaultFlags>(paletteBuffer.c_str());
    if (!ok) {
      printf("JSON parse error: %d (%lu)\n", ok.Code(), ok.Offset());
    }

    if (CHECK_ARRAY(dPalette, "palette")) {
      auto size = dPalette["palette"].Size();
      if (size > 0) {
        Palette::getInstance()->clearPalette();
#if EDITOR_MODE
        UIColorEditor::colorEditor->cleanColors();
#endif
      }

      for (auto i = 0; i < size; i++) {
        auto &palette = dPalette["palette"][i];
        if (palette["index"].IsInt() && palette["color"].IsString()) {
          Palette::getInstance()->setColorFromPalette(palette["index"].GetInt(),
                                                      str2Color(palette["color"].GetString()));
#if EDITOR_MODE
          UIColorEditor::colorEditor->addColor(palette["index"].GetInt(),
                                               str2Color(palette["color"].GetString()));
#endif
        }
      }

#if EDITOR_MODE
      UIColorEditor::colorEditor->updateColorButtonDisplay();
#endif
    }
  }

  if (CHECK_INT(d, "heroColorIndex")) {
    GameLogic::Game->mHero->setColor(d["heroColorIndex"].GetInt());
  }

  if (CHECK_ARRAY(d, "timeEvents")) {
    auto size = d["timeEvents"].Size();

    for (auto i = 0; i < size; ++i) {
      auto &var = d["timeEvents"][i];

      bool loop = true;
      float initDelay = -1.0f;
      if (var["loop"].IsBool()) {
        loop = var["loop"].GetBool();
      }

      if (CHECK_NUMBER(var, "initDelay")) {
        initDelay = var["initDelay"].GetDouble();
      }

      TimeEvent tEvent;
      tEvent.mLoop = loop;
      tEvent.mInitDelay = initDelay;

      if (CHECK_ARRAY(var, "events")) {
        auto eventsSize = var["events"].Size();
        for (auto j = 0; j < eventsSize; j++) {
          float delayTime = var["events"][j]["delay"].GetDouble();
          TimeEvent::TimeEventPoint tEventPoint(delayTime);
          if (CHECK_ARRAY(var["events"][j], "event")) {
            auto pointSize = var["events"][j]["event"].Size();
            for (auto k = 0; k < pointSize; k++) {
              auto command = var["events"][j]["event"][k].GetString();
              tEventPoint.push(command);
            }
          }
          tEvent.mEventPoints.push_back(tEventPoint);
        }
      }
      tEvent.reset();
      GameLogic::Game->mTimeEvents.push_back(tEvent);
    }
  }

  if (CHECK_ARRAY(d, "blocks")) {
    auto size = d["blocks"].Size();

    for (auto i = 0; i < size; ++i) {
      auto &var = d["blocks"][i];

      int id = 0;
      Size size;
      Vec2 pos;
      bool pickable = true;
      bool removable = true;
      int rotSpeed = 0;
      BlockKind kind = KIND_BLOCK;
      std::string textureName = "images/saw3.png";
      std::string userData;
      int paletteIndex = -1;
      bool flipuv = false;
      std::string triggerEvent = "";
      float shadowLeng = 100;
      bool shadowEnable = true;
      int shadowLayer = 0;

      if (var["id"].IsInt()) {
        id = var["id"].GetInt();
        maxID = std::max(id, maxID);
      } SHOW_WARNING

      if (var["size"].IsString()) {
        size = str2Size(var["size"].GetString());
        size.width = std::max(size.width, 0.5f);
        size.height = std::max(size.height, 0.5f);
      } SHOW_WARNING

      if (CHECK_STRING(var, "position")) {
        pos = str2Vec(var["position"].GetString());
      } SHOW_WARNING

      if (CHECK_BOOL(var, "removable")) {
        removable = var["removable"].GetBool();
      }

      if (var["pickable"].IsBool()) {
        pickable = var["pickable"].GetBool();
      } SHOW_WARNING

      if (CHECK_NUMBER(var, "rotatespeed")) {
        rotSpeed = var["rotatespeed"].GetInt();
      }

      if (var["kind"].IsString()) {
        kind = str2Kind(var["kind"].GetString());
      } SHOW_WARNING

      if (CHECK_BOOL(var, "flipUV")) {
        flipuv = var["flipUV"].GetBool();
      }

      if (CHECK_STRING(var, "textureName")) {
        textureName = var["textureName"].GetString();
      }

      if (var.HasMember("timeEvents") && var["userData"].IsString()) {
        userData = var["userData"].GetString();
      }

      if (CHECK_NUMBER(var, "paletteIndex")) {
        paletteIndex = var["paletteIndex"].GetInt();
      }

      if (CHECK_NUMBER(var, "shadowLength")) {
        shadowLeng = var["shadowLength"].GetDouble();
      }

      if (CHECK_BOOL(var, "shadowEnable")) {
        shadowEnable = var["shadowEnable"].GetBool();
      }

      if (CHECK_INT(var, "shadowLayer")) {
        shadowLayer = var["shadowLayer"].GetInt();
      }

#if EDITOR_MODE
      if (!pickable) {
        float width = VisibleRect::right().x;
        float height = VisibleRect::top().y;
        float frameSize = 10;

        if (id == 1) {
          pos = Vec2(width / 2, frameSize / 2);
          size = Size(width, frameSize);
        } else if (id == 2) {
          pos = Vec2(width / 2, height - frameSize / 2);
          size = Size(width, frameSize);
        } else if (id == 3) {
          pos = Vec2(frameSize / 2, height / 2);
          size = Size(frameSize, height);
        } else if (id == 4) {
          pos = Vec2(width - frameSize / 2, height / 2);
          size = Size(frameSize, height);
        }
      }
#endif

      BlockBase *block = new BlockBase();
      block->mID = id;
      block->mRestorePosition = pos;
      block->mRestoreSize = size;
      block->mTextureName = textureName;

      if (kind == KIND_DEATH_CIRCLE || kind == KIND_DEATH) {
        if (CHECK_ARRAY(var, "triggerEvents")) {
          auto triggerEventSize = var["triggerEvents"].Size();
          for (auto i = 0; i < triggerEventSize; i++) {
            std::string triggerEvent = var["triggerEvents"][i].GetString();
            block->mTriggerEvents.push_back(triggerEvent);
          }
        }
      }

      if (CHECK_ARRAY(var, "initEvents")) {
        auto initEventSize = var["initEvents"].Size();
        for (auto i = 0; i < initEventSize; i++) {
          std::string initEvent = var["initEvents"][i].GetString();
          block->mInitialEvents.push_back(initEvent);
        }
      }

      block->setKind(kind, true);
      if (paletteIndex != -1) {
        block->setColor(paletteIndex);
      }

      block->mCanPickup = pickable;
      block->mCanDelete = id > 4 ? removable : false;
      block->mRotationSpeed = rotSpeed;
      block->mShadowLength = shadowLeng;
      block->mCastShadow = shadowEnable;
      block->mShadowLayerID = shadowLayer;
      block->mUVFlipped = flipuv;
      block->mUserData = userData;
#if EDITOR_MODE
      block->updateIDLabel();
#endif
      block->reset();

      GameLogic::Game->mBlocks[block->mID] = block;

      if (kind == KIND_BUTTON) {
        if (var["direction"].IsString()) {
          block->mButton->mDir = str2Direction(var["direction"].GetString());
#if EDITOR_MODE
          block->mButton->updateHelper();
#endif
        } SHOW_WARNING

        if (CHECK_BOOL(var, "canRestore")) {
          block->mButton->mCanRestore = var["canRestore"].GetBool();
#if EDITOR_MODE
          block->mButton->updateHelper();
#endif
        }

        if (CHECK_STRING(var, "pushedEvent")) {
          block->mButton->mPushedEvent = var["pushedEvent"].GetString();
        }
        if (CHECK_STRING(var, "restoredEvent")) {
          block->mButton->mRestoredEvent = var["restoredEvent"].GetString();
        }
        if (CHECK_STRING(var, "pushingEvent")) {
          block->mButton->mPushingEvent = var["pushingEvent"].GetString();
        }
      }

      if (CHECK_NUMBER(var, "pathSpeed")) {
        block->mPath.mSpeed = var["pathSpeed"].GetDouble();
      }

      if (CHECK_BOOL(var, "pingpong")) {
        block->mPath.mPingPong = var["pingpong"].GetBool();
      }

      if (CHECK_BOOL(var, "pause")) {
        block->mPath.mPause = var["pause"].GetBool();
        block->mPath.mOriginalPause = block->mPath.mPause;
      }

      if (CHECK_NUMBER(var, "pathWaitTime")) {
        block->mPath.mPathWaitTime = var["pathWaitTime"].GetDouble();
      }

      if (CHECK_ARRAY(var, "pathes")) {
        auto pathsize = var["pathes"].Size();
        for (auto j = 0; j < pathsize; ++j) {
          auto &pa = var["pathes"][j];
          Vec2 pos = Vec2::ZERO;
          float waittime = -1;
          float width = 1, height = 1;

          if (pa["position"].IsString()) {
            pos = str2Vec(pa["position"].GetString());
          } SHOW_WARNING

          if (CHECK_NUMBER(pa, "waittime")) {
            waittime = pa["waittime"].GetDouble();
          } SHOW_WARNING

          if (CHECK_NUMBER(pa, "width")) {
            width = pa["width"].GetDouble();
          }

          if (CHECK_NUMBER(pa, "height")) {
            height = pa["height"].GetDouble();
          }

          block->mPath.push(pos, waittime, width, height);
        }
      }

      if (CHECK_ARRAY(var, "groupMembers")) {
        if (var["groupFollowMode"].IsString()) {
          block->mFollowMode = str2FollowMode(var["groupFollowMode"].GetString());
        } SHOW_WARNING

        auto memberSize = var["groupMembers"].Size();
        for (auto j = 0; j < memberSize; ++j) {
          if (var["groupMembers"][j].IsNumber()) {
            auto id = var["groupMembers"][j].GetInt();
            pregroups[block].push_back(id);
          } SHOW_WARNING
        }
      }
    }
  } SHOW_WARNING

  CC_ASSERT(GameLogic::Game->mGroups.empty());
  // Process groups
  for (auto gi : pregroups) {
    for (auto idi : gi.second) {
      auto m = GameLogic::Game->findBlock(idi);
      CC_ASSERT(m);
      GameLogic::Game->mGroups[gi.first].push_back(m);
    }
  }

  GameLogic::Game->updateBounds();

  BlockBase::mIDCounter = maxID + 1;
#if EDITOR_MODE
  std::string fixedfilename = filename;

#   if EDITOR_RATIO == EDITOR_IPAD_MODE
  if(!PathLib::endsWith(fixedfilename, "_pad.json"))
    PathLib::replaceString(fixedfilename, ".json", "_pad.json");
#   elif EDITOR_RATIO == EDITOR_IP4_MODE
  if(!PathLib::endsWith(fixedfilename, "_ip4.json"))
    PathLib::replaceString(fixedfilename, ".json", "_ip4.json");
#   endif

  EditorScene::Scene->mCurFileName = fixedfilename;
  EditorScene::Scene->mSpawnPoint->setPosition(GameLogic::Game->mSpawnPos);
#if USE_SHADOW
  EditorScene::Scene->mLightPoint->setPosition(GameLogic::Game->mShadows->mOriginLightPos);
#endif
  EditorScene::Scene->mGradientCenterPoint->setPosition(GameLogic::Game->mGradientCenter);

  UILayer::Layer->setFileName(fixedfilename.c_str());
  UILayer::Layer->addMessage("File loaded");
#endif

#if EDITOR_MODE
  EditorScene::Scene->updateGroupDrawNode();
  EditorScene::Scene->enableGame(false, true);
#endif
}

#if EDITOR_MODE

void MapSerial::loadMap(bool local) {
  std::string fullpath = getMapDir();
  std::vector<std::string> out;
  auto filter = "JSON file(json)|*.json|All files (*.*)|*.*";
  PathLib::openFileDialog(nullptr, "Open map", fullpath + (local ? "/local" : "/remote"),
                          "", filter, 0, out);
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

  for (auto &m : HttpHelper::sAllMaps) {
    auto ret = std::regex_match(m.name, base_match, rx);
    if (!ret) {
      m.name += ".json";
    }

    std::string fullpath = getMapDir();

    if (fullpath.size() == 0) {
      CCLOGWARN("Warning: cannot locate the maps folder!");
      continue;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    fullpath = FileUtils::getInstance()->getWritablePath();
    fullpath += m.name;
#else
    fullpath += "/remote/";
    fullpath += m.name;
#endif
    auto fp = fopen(fullpath.c_str(), "w+");

    if (!fp) {
      CCLOGWARN("Warning: cannot save the map file : %s", fullpath.c_str());
      continue;
    }

    fprintf(fp, "%s", m.content.c_str());
    fclose(fp);
  }
}

const char *MapSerial::getMapDir() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  static std::string fullpath;
  auto env = getenv("XCODE_PROJ_DIR"); // manually added
  if (env) {
    fullpath = env;
    fullpath += "/../Resources/maps";
  } else {
    fullpath = FileUtils::getInstance()->fullPathForFilename("maps");
  }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  static std::string fullpath = FileUtils::getInstance()->fullPathForFilename("assets/maps");
#else
  static std::string fullpath = FileUtils::getInstance()->fullPathForFilename("maps");
#endif
  return fullpath.c_str();
}

const char *MapSerial::getConfigDir() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  static std::string fullpath;
  auto env = getenv("XCODE_PROJ_DIR"); // manually added
  if (env) {
    fullpath = env;
    fullpath += "/../Resources/configs";
  } else {
    fullpath = FileUtils::getInstance()->fullPathForFilename("configs");
  }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
  static std::string fullpath = FileUtils::getInstance()->fullPathForFilename("assets/configs");
#else
  static std::string fullpath = FileUtils::getInstance()->fullPathForFilename("configs");
#endif
  return fullpath.c_str();
}

void MapSerial::saveControlConfig(const char *file) {
  if (ControlPad::controlPadConfig->mControlConfig.empty()) {
    auto config1 = new ControlPadConfig();
    ControlPad::controlPadConfig->mControlConfig.push_back(config1);
    auto config2 = new ControlPadConfig();
    config2->mRightButtonPos = Vec2(300, 60);
    config2->mScale = 0.3f;
    ControlPad::controlPadConfig->mControlConfig.push_back(config2);
    auto config3 = new ControlPadConfig();
    config2->mRightButtonPos = Vec2(320, 60);
    config2->mScale = 0.4f;
    ControlPad::controlPadConfig->mControlConfig.push_back(config3);
  }

  stringstream ss;
  ss << "{\n";

  INDENT_1
  ss << "\"ConfigIndex\" : " << ControlPad::controlPadConfig->mSelectedConfig << ",\n ";

  INDENT_1
  ss << "\"ConfigArray\": [ \n";
  for (auto it = ControlPad::controlPadConfig->mControlConfig.begin();
       it != ControlPad::controlPadConfig->mControlConfig.end(); ++it) {
    if (it != ControlPad::controlPadConfig->mControlConfig.begin()) {
      ss << ", \n";
    }
    INDENT_2
    ss << "{\n";
    INDENT_3
    ss << "\"desc\": \"" << (*it)->mDescription << "\", \n";
    INDENT_3
    ss << "\"scale\": " << (*it)->mScale << ", \n";
    INDENT_3
    ss << "\"leftButton\": " << vec2Str((*it)->mLeftButtonPos) << ", \n";
    INDENT_3
    ss << "\"rightButton\": " << vec2Str((*it)->mRightButtonPos) << ", \n";
    INDENT_3
    ss << "\"jumpButton\": " << vec2Str((*it)->mJumpButtonPos) << " \n";
    INDENT_2
    ss << "} ";
  }

  ss << "\n";
  INDENT_1
  ss << "] \n";

  ss << "}";

  auto fp = fopen(file, "w+");
  if (!fp) {
    CCLOGWARN("Warning: cannot access the map file : %s", file);
    return;
  }
  fprintf(fp, "%s", ss.str().c_str());
  fclose(fp);
}

void MapSerial::loadControlConfig(const char *file) {
  std::string fullPath = getConfigDir();
  fullPath += "/";
  fullPath += file;

  auto buffer = FileUtils::getInstance()->getStringFromFile(fullPath);
  Document d;
  d.Parse<kParseDefaultFlags>(buffer.c_str());

  int configIndex = 0;
  if (d["ConfigIndex"].IsInt()) {
    configIndex = d["ConfigIndex"].GetInt();
    ControlPad::controlPadConfig->mSelectedConfig = configIndex;
  }

  std::string configKey = "ConfigArray";
  configKey += getLevelSuffix();

  if (CHECK_ARRAY(d, configKey.c_str())) {
    auto size = d[configKey.c_str()].Size();

    if (size > 0) {
      ControlPad::controlPadConfig->clearConfig();
    }

    for (auto i = 0; i < size; ++i) {
      auto config = new ControlPadConfig();
      auto &var = d[configKey.c_str()][i];

      config->mDescription = var["desc"].GetString();
      config->mScale = var["scale"].GetDouble();
      config->mLeftButtonPos = str2Vec(var["leftButton"].GetString());
      config->mRightButtonPos = str2Vec(var["rightButton"].GetString());
      config->mJumpButtonPos = str2Vec(var["jumpButton"].GetString());

      ControlPad::controlPadConfig->mControlConfig.push_back(config);
    }
  }
}

void MapSerial::loadControlConfig() {
  MapSerial::loadControlConfig("ControlConfig.json");
}
