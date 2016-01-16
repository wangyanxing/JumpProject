//
//  BlockKindConfigs.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#include "BlockKindConfigs.h"

PhysicsConfig BlockKindConfigs::getPhysicsConfig(BlockKind kind) {
  static PhysicsConfig configs[KIND_MAX] = {
    {PHYSICS_DYNAMIC, PHYSICS_SHAPE_RECT},
    {PHYSICS_KINEMATIC, PHYSICS_SHAPE_RECT},
    {PHYSICS_COLLISION_ONLY, PHYSICS_SHAPE_RECT},
    {PHYSICS_COLLISION_ONLY, PHYSICS_SHAPE_CIRCLE},
    {PHYSICS_COLLISION_ONLY, PHYSICS_SHAPE_RECT},
    {PHYSICS_DYNAMIC, PHYSICS_SHAPE_RECT}
  };
  return configs[kind];
}

RendererConfig BlockKindConfigs::getRendererConfig(BlockKind kind) {
  static RendererConfig configs[KIND_MAX] = {
    {RENDERER_RECT, "", 25},
    {RENDERER_RECT, "", 20},
    {RENDERER_DEATH, DEATH_IMAGE, 15},
    {RENDERER_DEATH_CIRCLE, DEATH_CIRCLE_IMAGE, 15},
    {RENDERER_RECT, "", 15},
    {RENDERER_RECT, "", 20}
  };
  return configs[kind];
}
