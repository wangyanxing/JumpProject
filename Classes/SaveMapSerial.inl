//
//  SaveMapSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 1/1/16.
//
//

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
  ss << "\"shadowGroup\": [ \n";
  for (size_t fi = 0; fi < GameLogic::Game->mShadows.size(); ++fi) {
    INDENT_2
    ss << "{\n";

    INDENT_3
    ss << "\"posx\": " << GameLogic::Game->mShadows[fi]->mPosX;
    RT_LINE
    INDENT_3
    ss << "\"width\": " << GameLogic::Game->mShadows[fi]->mWidth;
    RT_LINE
    INDENT_3
    ss << "\"lightType\": " << lightType2Str(GameLogic::Game->mShadows[fi]->mLightType);
    RT_LINE
    INDENT_3
    ss << "\"lightDir\": " << GameLogic::Game->mShadows[fi]->mLightDirDegree;
    RT_LINE
    INDENT_3
    ss << "\"lightPosition\": " << vec2Str(GameLogic::Game->mShadows[fi]->mOriginLightPos);
    RT_LINE
    INDENT_3
    ss << "\"lightMoving\": " << bool2Str(GameLogic::Game->mShadows[fi]->mShadowMovingEnable);
    RT_LINE
    INDENT_3
    ss << "\"shadowDarkness\": " << GameLogic::Game->mShadows[fi]->mShadowDarkness;
    ss << std::endl;

    INDENT_2
    ss << "}";
    if (fi != GameLogic::Game->mShadows.size() - 1) {
      ss << ",";
    }
    ss << "\n";
  }
  INDENT_1
  ss << "]";
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
  ss << "\"sprites\": [ \n";
  for (size_t fi = 0; fi < GameLogic::Game->mSpriteList.size(); ++fi) {
    INDENT_2
    ss << "{\n";
    saveSceneSprite(ss, GameLogic::Game->mSpriteList[fi]);
    INDENT_2
    ss << "}";
    if (fi != GameLogic::Game->mSpriteList.size() - 1) {
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
    
    if (!b->mCanPickup) {
      INDENT_3
      ss << "\"pickable\": " << bool2Str(b->mCanPickup);
      RT_LINE
    }
    
    if (!b->mCanDelete) {
      INDENT_3
      ss << "\"removable\": " << bool2Str(b->mCanDelete);
      RT_LINE
    }
    
    if (b->mDisableMovement) {
      INDENT_3
      ss << "\"noMovement\": " << bool2Str(b->mDisableMovement);
      RT_LINE
    }
    
    if (b->mPreciseTrigger) {
      INDENT_3
      ss << "\"preciseTrigger\": " << bool2Str(b->mPreciseTrigger);
      RT_LINE
    }
    
    if (b->mRotationSpeed != DEFAULT_ROTATE_SPEED) {
      INDENT_3
      ss << "\"rotatespeed\": " << b->mRotationSpeed;
      RT_LINE
    }
    
    if (!floatEqual(b->mShadowLength, DEFAULT_SHADOW_LENGTH)) {
      INDENT_3
      ss << "\"shadowLength\": " << b->mShadowLength;
      RT_LINE
    }
    
    if (!b->mCastShadow) {
      INDENT_3
      ss << "\"shadowEnable\": " << bool2Str(b->mCastShadow);
      RT_LINE
    }

    if (b->mShadowGroup != 0) {
      INDENT_3
      ss << "\"shadowGroup\": " << b->mShadowGroup;
      RT_LINE
    }
    
    if (b->mShadowLayerID != 0) {
      INDENT_3
      ss << "\"shadowLayer\": " << b->mShadowLayerID;
      RT_LINE
    }
    
    if (b->mPaletteIndex != DEFAULT_PALETTE_INDEX) {
      INDENT_3
      ss << "\"paletteIndex\": " << b->mPaletteIndex;
      RT_LINE
    }
    
    if (b->mUVFlipped) {
      INDENT_3
      ss << "\"flipUV\": " << bool2Str(b->mUVFlipped);
      RT_LINE
    }
    
    if (b->getRenderer()->TextureName != DEFAULT_BLOCK_TEXTURE) {
      INDENT_3
      ss << "\"textureName\": \"" << b->getRenderer()->TextureName << "\"";
      RT_LINE
    }
    
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
    
    if (!b->mInitialEvents.empty()) {
      INDENT_3
      ss << "\"initEvents\": [";
      for (size_t i = 0; i < b->mInitialEvents.size(); i++) {
        ss << "\"" + b->mInitialEvents[i] + "\"";
        if (i != b->mInitialEvents.size() - 1) {
          ss << ", ";
        }
      }
      ss << "],\n";
    }
    
    if (b->mKind == KIND_BUTTON) {
      INDENT_3
      ss << "\"direction\": " << direction2Str(b->mButton->mDir);
      RT_LINE
      INDENT_3
      ss << "\"canRestore\": " << bool2Str(b->mButton->mCanRestore);
      RT_LINE
      
      INDENT_3
      ss << "\"pushedEvent\": " << toJsonArray(b->mButton->mPushedEvents);
      RT_LINE
      INDENT_3
      ss << "\"restoredEvent\": " << toJsonArray(b->mButton->mRestoredEvents);
      RT_LINE
      INDENT_3
      ss << "\"pushingEvent\": " << toJsonArray(b->mButton->mPushingEvents);
      RT_LINE
    }
    
    // Group
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

void MapSerial::saveRemoteMaps() {
  HttpHelper::getAllMaps();
}
