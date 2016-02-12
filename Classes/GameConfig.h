//
//  GameConfig.hpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/19/16.
//
//

#ifndef GameConfig_h
#define GameConfig_h

#include "Prerequisites.h"
#include "Singleton.h"

class GameConfig {
public:
  DECL_SIMPLE_SINGLETON(GameConfig);
  
  void load();
  
  float HeroSize{30};
  
  float MoveAcceleration{1500};
  
  float JumpAcceleration{25000};

  float AccelerationResistance{0.3f};
  
  std::string SawImage;
  
  std::string SawImageRotation;

  float CurtainMoveTime{0.8f};

  float ControlPadScale{0.5f};

  cocos2d::Vec2 ControlPadLeftButton{100, 70};

  cocos2d::Vec2 ControlPadRightButton{290, 70};

  cocos2d::Vec2 ControlPadJumpButton{840, 70};
};

#endif /* GameConfig_h */
