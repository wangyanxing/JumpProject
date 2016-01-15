//
//  GameTypes.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#ifndef GameTypes_h
#define GameTypes_h

enum BlockKind {
  KIND_HERO = 0,
  KIND_BLOCK,
  KIND_DEATH,
  KIND_DEATH_CIRCLE,
  KIND_BUTTON,
  KIND_PUSHABLE,

  KIND_MAX
};

enum RendererType {
  RENDERER_RECT,
  RENDERER_DEATH_CIRCLE,
  RENDERER_IMAGE
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

template <typename T>
class EnumSerial {
public:
  static std::string toString(T val);

  static T parse(const std::string& str);
};

template <>
class EnumSerial <BlockKind> {
public:
  static std::string toString(BlockKind val) {
    static std::string names[] = {
      "HERO",
      "BLOCK",
      "DEATH",
      "DEATH_CIRCLE",
      "BUTTON",
      "PUSHABLE"
    };
    return names[val];
  }

  static BlockKind parse(const std::string& str) {
    static std::map<std::string, BlockKind> kinds = {
      {"HERO",         KIND_HERO},
      {"BLOCK",        KIND_BLOCK},
      {"DEATH",        KIND_DEATH},
      {"DEATH_CIRCLE", KIND_DEATH_CIRCLE},
      {"BUTTON",       KIND_BUTTON},
      {"PUSHABLE",     KIND_PUSHABLE}
    };

    if (!kinds.count(str)) {
      printf("[Parse Error] Invalid block kind: %s\n", str.c_str());
      return KIND_BLOCK;
    }
    return kinds[str];
  }
};

#endif /* GameTypes_h */
