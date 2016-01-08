//
//  LoadMapSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 1/1/16.
//
//

void MapSerial::loadLastEdit() {
  auto file = UserDefault::getInstance()->getStringForKey("lastedit");
  if (file.empty()) {
    loadMap(TEMPLATE_MAP);
  } else {
    loadMap(file.c_str());
  }
}

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

  GAME_LAYER->clean();

  GameLogic::Game->initBackground();
  
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
    GameLogic::Game->updateHeroSpawnPos();
  } SHOW_WARNING
  
#if USE_SHADOW
  if (CHECK_ARRAY(d, "shadowGroup")) {
    auto size = d["shadowGroup"].Size();
    GameLogic::Game->mNumShadowGroup = size;
    
    for (auto si = 0; si < size; ++si) {
      Document::ValueType &var = d["shadowGroup"][si];
      GameLogic::Game->addShadowGroup();
      GameLogic::Game->mShadows[si]->mShadowGroup = si;
      
      if (CHECK_STRING(var, "lightType")) {
        GameLogic::Game->mShadows[si]->mLightType = str2lightType(var["lightType"].GetString());
        GameLogic::Game->mShadows[si]->mLightDirDegree = var["lightDir"].GetDouble();
        GameLogic::Game->mShadows[si]->updateLightDir();
      } else {
        GameLogic::Game->mShadows[si]->mLightType = ShadowManager::LIGHT_POINT;
      }
      
      if (CHECK_STRING(var, "lightPosition")) {
        GameLogic::Game->mShadows[si]->mLightPos = str2Vec(var["lightPosition"].GetString());
        GameLogic::Game->mShadows[si]->mOriginLightPos = GameLogic::Game->mShadows[si]->mLightPos;
      }
      
      if (CHECK_BOOL(var, "lightMoving")) {
        GameLogic::Game->mShadows[si]->mShadowMovingEnable = var["lightMoving"].GetBool();
      }
      
      if (CHECK_NUMBER(var, "shadowDarkness")) {
        GameLogic::Game->mShadows[si]->mShadowDarkness =var["shadowDarkness"].GetDouble();
      }
      
      if (CHECK_NUMBER(var, "posx")) {
        GameLogic::Game->mShadows[si]->mPosX = var["posx"].GetDouble();
      }
      
      if (CHECK_NUMBER(var, "width")) {
        GameLogic::Game->mShadows[si]->mWidth =var["width"].GetDouble();
      }
      
      GameLogic::Game->initShadowGroup(si);
    }
  } else {
    // Double write now, should be deprecated soon.
    GameLogic::Game->mNumShadowGroup = 1;
    GameLogic::Game->addShadowGroup();

    if (CHECK_STRING(d, "lightType")) {
      GameLogic::Game->mShadows[0]->mLightType = str2lightType(d["lightType"].GetString());
      GameLogic::Game->mShadows[0]->mLightDirDegree = d["lightDir"].GetDouble();
      GameLogic::Game->mShadows[0]->updateLightDir();
    } else {
      GameLogic::Game->mShadows[0]->mLightType = ShadowManager::LIGHT_POINT;
    }
    
    if (CHECK_STRING(d, "lightPosition")) {
      GameLogic::Game->mShadows[0]->mLightPos = str2Vec(d["lightPosition"].GetString());
      GameLogic::Game->mShadows[0]->mOriginLightPos = GameLogic::Game->mShadows[0]->mLightPos;
    }
    
    if (CHECK_BOOL(d, "lightMoving")) {
      GameLogic::Game->mShadows[0]->mShadowMovingEnable = d["lightMoving"].GetBool();
    }
    
    if (CHECK_NUMBER(d, "shadowDarkness")) {
      GameLogic::Game->mShadows[0]->mShadowDarkness = d["shadowDarkness"].GetDouble();
    }

    GameLogic::Game->initShadowGroup(0);
  }
#endif

  GameLogic::Game->mFxList = parseJsonStrArray("fx", d);
  GameLogic::Game->loadFxFromList();

  if (CHECK_ARRAY(d, "sprites")) {
    auto size = d["sprites"].Size();
    GameLogic::Game->mSpriteList.clear();
    for (auto fi = 0; fi < size; ++fi) {
      Document::ValueType &var = d["sprites"][fi];
      GameLogic::Game->mSpriteList.push_back(loadSceneSprite(var));
    }
  }
  GameLogic::Game->loadSpritesFromList();
  
  std::string paletteFileName = Palette::getInstance()->getPaletteFileName();
  if (CHECK_STRING(d, "paletteFile")) {
    paletteFileName = d["paletteFile"].GetString();
    Palette::getInstance()->setPaletteFileName(paletteFileName);
  }
  paletteFileName = FileUtils::getInstance()->fullPathForFilename(paletteFileName.c_str());
  loadPalette(paletteFileName.c_str());
  
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

      READ_INT(id, "id", var, 0);
      READ_SIZE(size, "size", var);
      READ_VEC(pos, "position", var);
      READ_BOOL(removable, "removable", var, true);
      READ_BOOL(pickable, "pickable", var, true)
      READ_BOOL(noMovement, "noMovement", var, false)
      READ_BOOL(preciseTrigger, "preciseTrigger", var, false)
      READ_INT(rotSpeed, "rotatespeed", var, DEFAULT_ROTATE_SPEED);
      READ_KIND(kind, "kind", var);
      READ_BOOL(flipuv, "flipUV", var, false);
      READ_STR(textureName, "textureName", var, DEFAULT_BLOCK_TEXTURE);
      READ_STR(userData, "userData", var, "");
      READ_INT(paletteIndex, "paletteIndex", var, DEFAULT_PALETTE_INDEX);
      READ_FLT(shadowLeng, "shadowLength", var, DEFAULT_SHADOW_LENGTH);
      READ_BOOL(shadowEnable, "shadowEnable", var, true);
      READ_INT(shadowLayer, "shadowLayer", var, 0);
      READ_INT(shadowGroup, "shadowGroup", var, 0);

      maxID = std::max(id, maxID);
      size.width = std::max(size.width, 0.5f);
      size.height = std::max(size.height, 0.5f);

      BlockBase *block = new BlockBase();
      block->mID = id;
      block->mRestorePosition = pos;
      block->mRestoreSize = size;
      block->mTriggerEvents = parseJsonStrArray("triggerEvents", var);
      block->mInitialEvents = parseJsonStrArray("initEvents", var);
      block->setKind(kind, true);
      block->setColor(paletteIndex);
      block->getRenderer()->setTexture(textureName);
      block->mCanPickup = pickable;
      block->mCanDelete = id > 4 ? removable : false;
      block->mDisableMovement = noMovement;
      block->mPreciseTrigger = preciseTrigger;
      block->mRotationSpeed = rotSpeed;
      block->mShadowLength = shadowLeng;
      block->mShadowGroup = shadowGroup;
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
        if (CHECK_STRING(var, "direction")) {
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
        block->mButton->mPushedEvents = parseJsonStrArray("pushedEvent", var);
        block->mButton->mRestoredEvents = parseJsonStrArray("restoredEvent", var);
        block->mButton->mPushingEvents = parseJsonStrArray("pushingEvent", var);
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
        pregroups[block] = parseJsonIntArray("groupMembers", var);
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
  EditorScene::Scene->updateLightHelper();
#endif
  
  UILayer::Layer->setFileName(fixedfilename.c_str());
  UILayer::Layer->addMessage("File loaded");

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
    fullpath = FileUtils::getInstance()->getWritablePath() + m.name;
#else
    fullpath += "/remote/" + m.name;
#endif
    saveToFile(fullpath.c_str(), m.content.c_str());
  }
}
