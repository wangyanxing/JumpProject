//
//  ShadowManager.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/25/16.
//
//

#include "ShadowManager.h"
#include "VisibleRect.h"
#include "GameObject.h"
#include "GameRenderer.h"
#include "JsonFormat.h"

USING_NS_CC;

#if USE_REFACTOR

static void colorMix(const Color4B &src, const Color4B &dst, float r, Color4B &out) {
  out.r = dst.r * r + src.r * (1 - r);
  out.g = dst.g * r + src.g * (1 - r);
  out.b = dst.b * r + src.b * (1 - r);
  out.a = dst.a * r + src.a * (1 - r);
}

ShadowManager::ShadowManager() {
}

ShadowManager::~ShadowManager() {
}

void ShadowManager::load(JsonValueT &group) {
  mPosX = group[SHADOW_GROUP_POSX].GetDouble();
  mWidth = group[SHADOW_GROUP_WIDTH].GetDouble();
  mLightType = group[SHADOW_GROUP_LIGHT_TYPE].getEnum<ShadowLightType>();
  mLightDir = group[SHADOW_GROUP_LIGHT_DIR].GetVec2();
  mLightPos = group[SHADOW_GROUP_LIGHT_POS].GetVec2();
  mShadowDarkness = group[SHADOW_GROUP_LIGHT_DARKNESS].GetDouble();
}

#endif
