//
//  SaveMapSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 1/1/16.
//
//

void MapSerial::saveMap(const char *file) {
  stringstream ss;
  auto Game = GameLogic::Game;
  auto palette = Palette::getInstance();
  auto timestr = getTimeStr();
  auto author = getComputerUser();

  BEGIN_OBJECT(0)

  WRITE_STR_E(1, "author", author);
  WRITE_STR_E(1, "time", timestr);
  WRITE_NUM_E(1, "ratio", VisibleRect::right().x / VisibleRect::top().y);
  WRITE_COL_E(1, "backgroundColor", Game->mBackgroundColor);
  WRITE_NUM_E(1, "heroColorIndex", Game->mHero->mPaletteIndex);
  WRITE_COL_E(1, "normalBlockColor", palette->getDefaultBlockColors(KIND_BLOCK));
  WRITE_COL_E(1, "deathBlockColor", palette->getDefaultBlockColors(KIND_DEATH));
  WRITE_COL_E(1, "deathCircleColor", palette->getDefaultBlockColors(KIND_DEATH_CIRCLE));
  WRITE_COL_E(1, "buttonColor", palette->getDefaultBlockColors(KIND_BUTTON));
  WRITE_COL_E(1, "pushableBlockColor", palette->getDefaultBlockColors(KIND_PUSHABLE));
  WRITE_VEC_E(1, "spawnPosition", Game->mSpawnPos);
  WRITE_STR_E(1, "paletteFile", palette->getPaletteFileName());

#if USE_SHADOW
  BEGIN_ARRAY(1, "shadowGroup");
  for (auto it = Game->mShadows.begin(); it != Game->mShadows.end(); ++it) {
    BEGIN_OBJECT(2)

    auto shadow = *it;
    WRITE_NUM_E(3, "posx", shadow->mPosX);
    WRITE_NUM_E(3, "width", shadow->mWidth);
    WRITE_GEN_E(3, "lightType", lightType2Str(shadow->mLightType));
    WRITE_NUM_E(3, "lightDir", shadow->mLightDirDegree);
    WRITE_VEC_E(3, "lightPosition", shadow->mOriginLightPos);
    WRITE_BOL_E(3, "lightMoving", shadow->mShadowMovingEnable);
    WRITE_NUM_R(3, "shadowDarkness", shadow->mShadowDarkness);

    END_OBJECT(2, it + 1 != Game->mShadows.end());
  }
  END_ARRAY(1, WITH_COMMA);
#endif

  BEGIN_ARRAY(1, "fx");
  for (auto it = Game->mFxList.begin(); it != Game->mFxList.end(); ++it) {
    WRITE_ARR_STR(2, *it, it + 1 != Game->mFxList.end());
  }
  END_ARRAY(1, WITH_COMMA);

  BEGIN_ARRAY(1, "stars");
  for (auto it = Game->mStarList.begin(); it != Game->mStarList.end(); ++it) {
    WRITE_ARR_VEC(2, *it, it + 1 != Game->mStarList.end());
  }
  END_ARRAY(1, WITH_COMMA);

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
