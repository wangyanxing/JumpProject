//
//  BlockKindConfigs.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#ifndef BlockKindConfigs_h
#define BlockKindConfigs_h

#include "Prerequisites.h"

struct PhysicsConfig {
  PhysicsType type;
  PhysicsShapeType shapeType;
};

struct RendererConfig {
  RendererType type;
  std::string defaultTexture;
  int zorder;
  bool shadowEnabled;
};

class BlockKindConfigs {
public:
  static PhysicsConfig getPhysicsConfig(BlockKind kind);

  static RendererConfig getRendererConfig(BlockKind kind);
  
  /**
   * Get components for a kind (besides physics component).
   */
  static std::vector<ComponentType> getComponents(BlockKind kind);
};

#endif /* BlockKindConfigs_h */
