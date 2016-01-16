//
//  SimpleRenderer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef SimpleRenderer_h
#define SimpleRenderer_h

#include "GameRenderer.h"

class SpriteUV;

/**
 * Simple implementation of the GameRenderer.
 */
class SimpleRenderer : public GameRenderer {
public:
  SimpleRenderer(GameObject *parent);

  virtual ~SimpleRenderer();

  RendererType getType() const override {
    return RENDERER_RECT;
  }

  virtual void init(Parameter &param) override;

  virtual void setTexture(const std::string& texName) override;

  cocos2d::Node* getNode() override;

  cocos2d::Node* getNode() const override;

  SpriteUV* getSprite() {
    return mSprite;
  }

protected:
  SpriteUV* mSprite{nullptr};

  std::string mTextureName;  
};

#endif /* SimpleRenderer_h */