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
  RENDERER_DEATH,
  RENDERER_DEATH_CIRCLE,
  RENDERER_IMAGE
};

enum ComponentType {
  COMPONENT_PHYSICS,
  COMPONENT_PATH,
  COMPONENT_BUTTON,
  COMPONENT_ROTATOR,
  COMPONENT_INPUT,
  COMPONENT_EDITOR,
};

enum ComponentCommand {
  COMMAND_COLLISION,
  COMMAND_INPUT,
  COMMAND_PATH,
  COMMAND_EDITOR
};

enum InputType {
  INPUT_LEFT,
  INPUT_RIGHT,
  INPUT_JUMP
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

enum ButtonDirection {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT
};

enum ShadowLightType {
  LIGHT_POINT,
  LIGHT_DIR,
};

enum EditorCommand {
  CMD_SELECT,
  CMD_UNSELECT,
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

template <>
class EnumSerial <ButtonDirection> {
public:
  static std::string toString(ButtonDirection val) {
    static std::string names[] = {
      "UP",
      "DOWN",
      "LEFT",
      "RIGHT"
    };
    return names[val];
  }
  
  static ButtonDirection parse(const std::string& str) {
    static std::map<std::string, ButtonDirection> dirs = {
      {"UP",    BUTTON_UP},
      {"DOWN",  BUTTON_DOWN},
      {"LEFT",  BUTTON_LEFT},
      {"RIGHT", BUTTON_RIGHT}
    };
    
    if (!dirs.count(str)) {
      printf("[Parse Error] Invalid button direction: %s\n", str.c_str());
      return BUTTON_DOWN;
    }
    return dirs[str];
  }
};

template <>
class EnumSerial <ShadowLightType> {
public:
  static std::string toString(ShadowLightType val) {
    static std::string names[] = {
      "POINT",
      "DIR"
    };
    return names[val];
  }

  static ShadowLightType parse(const std::string& str) {
    static std::map<std::string, ShadowLightType> types = {
      {"POINT", LIGHT_POINT},
      {"DIR",   LIGHT_DIR}
    };

    if (!types.count(str)) {
      printf("[Parse Error] Invalid light type: %s\n", str.c_str());
      return LIGHT_POINT;
    }
    return types[str];
  }
};

// For unoredered_map.
struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

#endif /* GameTypes_h */
