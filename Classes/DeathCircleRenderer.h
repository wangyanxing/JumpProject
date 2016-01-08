//
//  DeathCircleRenderer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/7/16.
//
//

#ifndef DeathCircleRenderer_h
#define DeathCircleRenderer_h

#include "RectRenderer.h"

class DeathCircleRenderer : public RectRenderer {
public:
  DeathCircleRenderer(BlockBase *parent);

  ~DeathCircleRenderer();

  RendererType getType() const override {
    return BlockRenderer::TYPE_DEATH_CIRCLE;
  }

  void init(InitParams& param) override;

  void addToParent(cocos2d::Node* parent, int zorder) override;

  void setPosition(const cocos2d::Vec2& pos) override;

  void setRotation(float val) override;

  void setScale(float x, float y) override;

  void setColor(const cocos2d::Color3B& color) override;

private:
  cocos2d::Sprite* mChild{nullptr};
};

#endif /* DeathCircleRenderer_h */
