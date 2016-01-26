//
//  ShadowManager.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/25/16.
//
//

#ifndef ShadowManager_h
#define ShadowManager_h

#include "Prerequisites.h"
#include "JsonParser.h"

#if USE_REFACTOR

class ShadowManager {
public:
  enum {
    SHADOW_LAYER_0,
    SHADOW_LAYER_1,
    NUM_SHADOW_LAYERS,

    SHADOW_LAYER_HERO = SHADOW_LAYER_1,
  };

  struct ShadowEntry {
    bool noShadow{false};
    cocos2d::Vec2 pt1;
    cocos2d::Vec2 pt2;
    bool needMakeUp{false};
    cocos2d::Vec2 makeUpPt;
  };

  ShadowManager();

  ~ShadowManager();

  void init(cocos2d::Node *parentNode);

  void load(JsonValueT &json);

  void reset();

  void update(float dt);

  void updateLightDir();

private:
  cocos2d::Vec2 mLightPos;

  cocos2d::Vec2 mOriginLightPos;

  float mLightDirDegree{45};

  cocos2d::Vec2 mLightDir;

  ShadowLightType mLightType{LIGHT_POINT};

  cocos2d::DrawNodeEx *mShadowDrawers[NUM_SHADOW_LAYERS];

  cocos2d::RenderTexture *mRenderTextures[NUM_SHADOW_LAYERS];

  float mShadowDarkness{0.098f};

  int mShadowGroup{0};

  /// Relative position (0 ~ 1).
  float mPosX{0};

  /// Relative size (0 ~ 1).
  float mWidth{1};
};

#endif

#endif /* ShadowManager_h */
