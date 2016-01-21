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
    {RENDERER_RECT, "", 25, true},
    {RENDERER_RECT, "", 20, true},
    {RENDERER_DEATH, DEATH_IMAGE, 15, false},
    {RENDERER_DEATH_CIRCLE, "", 15, false},
    {RENDERER_RECT, "", 15, false},
    {RENDERER_RECT, "", 20, true}
  };
  return configs[kind];
}

std::vector<ComponentType> BlockKindConfigs::getComponents(BlockKind kind) {
  static std::vector<ComponentType> components[KIND_MAX] = {
    {COMPONENT_INPUT},
    {},
    {},
    {},
    {COMPONENT_BUTTON},
    {},
  };
  return components[kind];
}
