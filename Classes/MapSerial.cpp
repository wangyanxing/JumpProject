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
#include "SceneSprite.h"

std::string MapSerial::CurrentEditingFile;

#include "MapSerialUtils.inl"
#include "SceneSpriteSerial.inl"
#include "PaletteSerial.inl"
#include "ControlConfigSerial.inl"
#include "SaveMapSerial.inl"
#include "LoadMapSerial.inl"

const char *MapSerial::getMapDir() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  static std::string fullpath;
  auto env = getenv("XCODE_PROJ_DIR");
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
  auto env = getenv("XCODE_PROJ_DIR");
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
