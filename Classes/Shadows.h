//
//  Shadows.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__Shadows__
#define __JumpEdt__Shadows__

#include "cocos2d.h"
#include "Defines.h"

#if USE_SHADOW

class BlockBase;
namespace cocos2d {
  class DrawNodeEx;
}

class ShadowManager {
public:
  enum {
    SHADOW_LAYER_0,
    SHADOW_LAYER_1,
    NUM_SHADOW_LAYERS,

    SHADOW_LAYER_HERO = SHADOW_LAYER_1,
  };

  enum LightType {
    LIGHT_POINT,
    LIGHT_DIR,
  };

  struct ShadowEntry {
    bool noShadow{false};
    cocos2d::Vec2 pt1;
    cocos2d::Vec2 pt2;
    bool needMakeUp{false};
    cocos2d::Vec2 makeUpPt;
  };

  ShadowManager(cocos2d::Node *parentNode);

  ~ShadowManager();

  void reset();

  void update(float dt);

  void updateLightDir();

  void updateBlock(BlockBase *block,
                   std::vector <cocos2d::V2F_C4B_T2F_Triangle> &triangles,
                   bool clipX = false);

  ShadowEntry getShadowEntry(const std::vector <cocos2d::Vec2> &pts, const cocos2d::Vec2 &lightPos);

  LightType mLightType{LIGHT_POINT};

  cocos2d::Vec2 mLightPos;

  cocos2d::Vec2 mOriginLightPos;

  float mLightDirDegree{ 45 };

  cocos2d::Vec2 mLightDir;

  cocos2d::DrawNodeEx *mShadowDrawers[NUM_SHADOW_LAYERS];

  cocos2d::RenderTexture *mRenderTextures[NUM_SHADOW_LAYERS];

  float mLightMoveTimer{0};

  bool mMoving{true};

  float mMoveTarget{0};

  float mMovingSpeed{0};

  bool mShadowMovingEnable{true};

  float mShadowDarkness{0.098f};
  
  int mShadowGroup{0};
};

#endif


#endif /* defined(__JumpEdt__Shadows__) */
