//
//  SaveMapSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 1/1/16.
//
//

#if EDITOR_MODE

void MapSerial::saveMap(const char *file) {
  stringstream ss;
  auto Game = GameLogic::Game;
  auto palette = Palette::getInstance();
  auto timestr = getTimeStr();
  auto author = getComputerUser();

  BEGIN_OBJECT(0)

  /**
   * Level information
   */
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
  /**
   * Shadow groups, usually only have one group.
   */
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

    END_OBJECT(2, next(it) != Game->mShadows.end());
  }
  END_ARRAY(1, WITH_COMMA);
#endif

  /**
   * Particle effects.
   */
  BEGIN_ARRAY(1, "fx");
  for (auto it = Game->mFxList.begin(); it != Game->mFxList.end(); ++it) {
    WRITE_ARR_STR(2, *it, next(it) != Game->mFxList.end());
  }
  END_ARRAY(1, WITH_COMMA);

  /**
   * Star effects (deprecated).
   */
  BEGIN_ARRAY(1, "stars");
  for (auto it = Game->mStarList.begin(); it != Game->mStarList.end(); ++it) {
    WRITE_ARR_VEC(2, *it, next(it) != Game->mStarList.end());
  }
  END_ARRAY(1, WITH_COMMA);

  /**
   * Simple sprite images.
   */
  BEGIN_ARRAY(1, "sprites");
  for (auto it = Game->mSpriteList.begin(); it != Game->mSpriteList.end(); ++it) {
    BEGIN_OBJECT(2)
    saveSceneSprite(ss, *it);
    END_OBJECT(2, next(it) != Game->mSpriteList.end());
  }
  END_ARRAY(1, WITH_COMMA);

  /**
   * Blocks.
   */
  BEGIN_ARRAY(1, "blocks");
  
  for (auto bit = Game->mBlocks.begin(); bit != Game->mBlocks.end(); ++bit) {
    auto b = bit->second;

    BEGIN_OBJECT(2)

    WRITE_NUM_E(3, "id", b->mID);
    WRITE_SIZ_E(3, "size", b->mRestoreSize);
    WRITE_VEC_E(3, "position", b->mRestorePosition);

    CHECK_DEFAULT(b->mCanPickup, true) {
      WRITE_BOL_E(3, "pickable", b->mCanPickup);
    }

    CHECK_DEFAULT(b->mCanDelete, true) {
      WRITE_BOL_E(3, "removable", b->mCanDelete);
    }

    CHECK_DEFAULT(b->mDisableMovement, false) {
      WRITE_BOL_E(3, "noMovement", b->mDisableMovement);
    }

    CHECK_DEFAULT(b->mPreciseTrigger, false) {
      WRITE_BOL_E(3, "preciseTrigger", b->mPreciseTrigger);
    }

    CHECK_DEFAULT(b->mRotationSpeed, DEFAULT_ROTATE_SPEED) {
      WRITE_NUM_E(3, "rotatespeed", b->mRotationSpeed);
    }

    CHECK_DEFAULT(b->mShadowLength, DEFAULT_SHADOW_LENGTH) {
      WRITE_NUM_E(3, "shadowLength", b->mShadowLength);
    }

    CHECK_DEFAULT(b->mCastShadow, true) {
      WRITE_BOL_E(3, "shadowEnable", b->mCastShadow);
    }

    CHECK_DEFAULT(b->mShadowGroup, 0) {
      WRITE_NUM_E(3, "shadowGroup", b->mShadowGroup);
    }

    CHECK_DEFAULT(b->mShadowLayerID, 0) {
      WRITE_NUM_E(3, "shadowLayer", b->mShadowLayerID);
    }

    CHECK_DEFAULT(b->mPaletteIndex, DEFAULT_PALETTE_INDEX) {
      WRITE_NUM_E(3, "paletteIndex", b->mPaletteIndex);
    }

    CHECK_DEFAULT(b->mUVFlipped, false) {
      WRITE_BOL_E(3, "flipUV", b->mUVFlipped);
    }

    CHECK_DEFAULT(b->getRenderer()->TextureName, std::string(DEFAULT_BLOCK_TEXTURE)) {
      WRITE_STR_E(3, "textureName", b->getRenderer()->TextureName);
    }

    CHECK_DEFAULT(b->mUserData, std::string()) {
      WRITE_STR_E(3, "userData", b->mUserData);
    }

    if (b->mKind == KIND_DEATH_CIRCLE || b->mKind == KIND_DEATH) {
      WRITE_GEN_E(3, "triggerEvents", toJsonArray(b->mTriggerEvents));
    }
    
    if (!b->mInitialEvents.empty()) {
      WRITE_GEN_E(3, "initEvents", toJsonArray(b->mInitialEvents));
    }
    
    if (b->mKind == KIND_BUTTON) {
      WRITE_GEN_E(3, "direction", direction2Str(b->mButton->mDir));
      WRITE_BOL_E(3, "canRestore", b->mButton->mCanRestore);
      WRITE_GEN_E(3, "pushedEvent", toJsonArray(b->mButton->mPushedEvents));
      WRITE_GEN_E(3, "restoredEvent", toJsonArray(b->mButton->mRestoredEvents));
      WRITE_GEN_E(3, "pushingEvent", toJsonArray(b->mButton->mPushingEvents));
    }
    
    /*
     * Groups.
     */
    auto ig = GameLogic::Game->mGroups.find(b);
    if (ig != GameLogic::Game->mGroups.end() && !ig->second.empty()) {
      vector<int> ids;
      for (size_t i = 0; i < ig->second.size(); ++i) {
        ids.push_back(ig->second[i]->mID);
      }
      WRITE_GEN_E(3, "groupFollowMode", followMode2Str(b->mFollowMode));
      WRITE_GEN_E(3, "groupMembers", toJsonArray(ids));
    }
    
    /*
     * Path.
     */
    if (!b->mPath.empty()) {
      WRITE_NUM_E(3, "pathSpeed", b->mPath.mSpeed);
      WRITE_BOL_E(3, "pingpong", b->mPath.mPingPong);
      WRITE_BOL_E(3, "pause", b->mPath.mPause);
      WRITE_NUM_E(3, "pathWaitTime", b->mPath.mPathWaitTime);

      BEGIN_ARRAY(3, "pathes");
      for (size_t i = 0; i < b->mPath.getNumPoints(); ++i) {
        const auto &p = b->mPath.getPoint(i);
        BEGIN_OBJECT(4);
        WRITE_VEC_E(5, "position", p.pt);
        WRITE_NUM_E(5, "width", p.width);
        WRITE_NUM_E(5, "height", p.height);
        WRITE_NUM_R(5, "waittime", p.waitTime);
        END_OBJECT(4, i != b->mPath.getNumPoints() - 1);
      }
      END_ARRAY(3, WITH_COMMA);
    }
    WRITE_GEN_R(3, "kind", kind2Str(b->mKind));
    END_OBJECT(2, next(bit) != Game->mBlocks.end());
  }

  // Blocks array.
  END_ARRAY(1, true);

  BEGIN_ARRAY(1, "timeEvents")

  for (auto it = Game->mTimeEvents.begin(); it != Game->mTimeEvents.end(); ++it) {
    auto event = *it;

    BEGIN_OBJECT(2)

    WRITE_NUM_E(3, "initDelay", event.mInitDelay);
    WRITE_NUM_E(3, "loop", event.mLoop);

    BEGIN_ARRAY(3, "events")
    
    for (auto eit = event.mEventPoints.begin(); eit != event.mEventPoints.end(); ++eit) {
      auto command = *eit;
      BEGIN_OBJECT(4)
      WRITE_GEN_E(5, "event", toJsonArray(command.mEvents));
      WRITE_GEN_R(5, "delay", command.waitTime);
      END_OBJECT(4, next(eit) != event.mEventPoints.end())
    }

    // Events array.
    END_ARRAY(3, NO_COMMA)

    // Time events object.
    END_OBJECT(2, next(it) != Game->mTimeEvents.end())
  }

  // Time event array.
  END_ARRAY(1, NO_COMMA)

  // Root object.
  END_OBJECT(0, NO_COMMA)

  saveToFile(file, ss);
  
  // Update file.
  EditorScene::Scene->mCurFileName = file;
  UILayer::Layer->setFileName(file);
  UILayer::Layer->addMessage("File saved");

  // Save history.
  UserDefault::getInstance()->setStringForKey("lastedit", file);
  UserDefault::getInstance()->flush();
  
  // Upload to server.
  std::string filename = file;
  const size_t last_slash_idx = filename.find_last_of("\\/");
  if (std::string::npos != last_slash_idx) {
    filename.erase(0, last_slash_idx + 1);
  }
  HttpHelper::updateMap(filename, author, timestr, ss.str());
}

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
