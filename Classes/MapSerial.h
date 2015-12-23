//
//  MapSerial.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/16/14.
//
//

#ifndef __JumpEdt__MapSerial__
#define __JumpEdt__MapSerial__

#include "Defines.h"

class ControlPadConfig{
public:
  std::string mDescription = "default";
  
  cocos2d::Vec2 mLeftButtonPos {cocos2d::Vec2(74,60)};
  
  cocos2d::Vec2 mRightButtonPos{cocos2d::Vec2(280,60)};
  
  cocos2d::Vec2 mJumpButtonPos{cocos2d::Vec2(820,60)};
  
  float mScale {0.3f};
};

typedef std::vector<ControlPadConfig *> ControlPadConfigs;

class ControlPad{
public:
  static ControlPad* controlPadConfig;
  
  ControlPad();
  
  ~ControlPad();
  
  void clearConfig();
  
  int mSelectedConfig {0};
  
  ControlPadConfig* getControlConfig();
  
  ControlPadConfigs mControlConfig;
};

class MapSerial {
public:
#if EDITOR_MODE
  static void saveMap();
#endif

  static void saveMap(const char* file);
  
  static void savePalette(const char* file);

#if EDITOR_MODE
  static void loadMap(bool local = true);
#endif

  static void loadMap(const char* file);

  static void loadLastEdit();

  static void saveRemoteMaps();

  static void afterLoadRemoteMaps();

  static const char* getMapDir();
  
  static const char* getConfigDir();

  static void saveControlConfig(const char* file);
  
  static void loadControlConfig(const char* file);

  static void loadControlConfig();
};

#endif /* defined(__JumpEdt__MapSerial__) */
