//
//  ControlPad.h
//  jumpproj
//
//  Created by Yanxing Wang on 12/23/15.
//
//

#ifndef ControlPad_h
#define ControlPad_h

class ControlPadConfig {
public:
  std::string mDescription = "default";
  
  cocos2d::Vec2 mLeftButtonPos{cocos2d::Vec2(74, 60)};
  
  cocos2d::Vec2 mRightButtonPos{cocos2d::Vec2(280, 60)};
  
  cocos2d::Vec2 mJumpButtonPos{cocos2d::Vec2(820, 60)};
  
  float mScale{0.3f};
};

typedef std::vector<ControlPadConfig *> ControlPadConfigs;

class ControlPad {
public:
  static ControlPad *controlPadConfig;
  
  ControlPad();
  
  ~ControlPad();
  
  void clearConfig();
  
  int mSelectedConfig{0};
  
  ControlPadConfig *getControlConfig();
  
  ControlPadConfigs mControlConfig;
};

#endif /* ControlPad_h */
