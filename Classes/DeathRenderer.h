//
//  DeathRenderer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#ifndef DeathRenderer_h
#define DeathRenderer_h

#include "Prerequisites.h"
#include "SimpleRenderer.h"

class DeathRenderer : public SimpleRenderer {
public:
  DeathRenderer(GameObject *parent);

  virtual ~DeathRenderer();

  RendererType getType() const override {
    return RENDERER_DEATH;
  }

  virtual void init(Parameter &param) override;

  void normalizeUV();

  void setFlipUV(bool val);

  bool isFlipUV();

private:
  float mUVFlipped{false};
};

#endif /* DeathRenderer_h */
