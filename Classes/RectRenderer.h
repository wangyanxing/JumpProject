//
//  RectRenderer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/7/16.
//
//

#ifndef RectRenderer_h
#define RectRenderer_h

#include "BlockRenderer.h"

class RectRenderer : public BlockRenderer {
public:
  RectRenderer(BlockBase *parent);

  virtual ~RectRenderer();

  RendererType getType() const override {
    return BlockRenderer::TYPE_RECT;
  }

  virtual void init(InitParams& param) override;

  void setTexture(const std::string& texName) override;

  cocos2d::Node* getNode() override;

  cocos2d::Node* getNode() const override;

  SpriteUV* getSprite() {
    return mSprite;
  }

  void normalizeUV();

protected:
  SpriteUV* mSprite{nullptr};
};

#endif /* RectRenderer_h */
