//
//  GameTypes.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#ifndef GameTypes_h
#define GameTypes_h

enum RendererType {
  RENDERER_RECT,
  RENDERER_DEATH_CIRCLE,
  RENDERER_IMAGE
};

enum ObjectKind {
  OBJ_KIND_HERO = 0,
  OBJ_KIND_BLOCK,
  OBJ_KIND_DEATH,
  OBJ_KIND_DEATH_CIRCLE,
  OBJ_KIND_BUTTON,
  OBJ_KIND_PUSHABLE,

  OBJ_KIND_MAX
};

enum ComponentType {
  COMPONENT_PHYSICS,
  COMPONENT_PATH,
  COMPONENT_BUTTON,
  COMPONENT_ROTATOR
};

enum PhysicsType {
  PHYSICS_NONE,
  PHYSICS_COLLISION_ONLY,
  PHYSICS_KINEMATIC,
  PHYSICS_DYNAMIC
};

enum PhysicsShapeType {
  PHYSICS_SHAPE_CIRCLE,
  PHYSICS_SHAPE_RECT
};

class EnumUtil {
public:
  static std::string toString(RendererType kind);

  static RendererType toRendererType(const std::string& str);

  static std::string toString(ObjectKind kind);
};

#endif /* GameTypes_h */
