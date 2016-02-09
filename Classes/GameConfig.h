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
};

#endif /* GameConfig_h */
