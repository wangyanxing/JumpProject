//
//  GameConfig.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/19/16.
//
//

#include "GameConfig.h"
#include "JsonFormat.h"
#include "JsonParser.h"

void GameConfig::load() {
  JsonParser jp("configs/GameConfig.json");
  auto &json = jp.getCurrentDocument();
  
  HeroSize = json["heroSize"].GetDouble();
  MoveAcceleration = json["moveAcceleration"].GetDouble();
  JumpAcceleration = json["jumpAcceleration"].GetDouble();
  AccelerationResistance = json["accelerationResistance"].GetDouble();
  SawImage = json["sawImage"].GetString();
  SawImageRotation = json["sawImageRotation"].GetString();
}
