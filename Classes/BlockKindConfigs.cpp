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
    {RENDERER_RECT, ""},
    {RENDERER_RECT, ""},
    {RENDERER_RECT, DEATH_IMAGE},
    {RENDERER_DEATH_CIRCLE, DEATH_CIRCLE_IMAGE},
    {RENDERER_RECT, ""},
    {RENDERER_RECT, ""}
  };
  return configs[kind];
}
