//
//  BlockRenderer.h
//  jumpproj
//
//  Created by Yanxing Wang on 12/29/15.
//
//

#ifndef BlockRenderer_h
#define BlockRenderer_h

#include "Any.h"

class SpriteUV;
class BlockBase;

class BlockRenderer {
public:
  enum RendererType {
    TYPE_RECT,
    TYPE_DEATH_CIRCLE,
    TYPE_IMAGE
  };

  enum InitParamType{
    PARAM_RECT,   // cocos2d::Rect
    PARAM_COLOR,  // cocos2d::Color3B
    PARAM_IMAGE,  // std::string
  };

  typedef std::map<InitParamType, Any> InitParams;

  BlockRenderer(BlockBase *parent) : mParentBlock(parent) {}

  virtual ~BlockRenderer() = default;

  virtual RendererType getType() const = 0;

  virtual void init(InitParams& param) = 0;

  virtual cocos2d::Node* getNode() = 0;

  virtual cocos2d::Node* getNode() const = 0;

  virtual void addToParent(cocos2d::Node* parent, int zorder);

  virtual void setZOrder(int val);

  virtual void setPosition(const cocos2d::Vec2& pos);

  virtual cocos2d::Vec2 getPosition() const;

  virtual void setColor(const cocos2d::Color3B& color);

  virtual cocos2d::Color3B getColor() const;

  virtual void setVisible(bool val);

  virtual bool isVisible() const;

  virtual void setRotation(float val);

  virtual float getRotation() const;

  virtual void setScale(float x, float y);

  virtual void setScale(float scale) {
    setScale(scale, scale);
  }

  virtual float getScaleX() const;

  virtual float getScaleY() const;

  virtual cocos2d::Size getContentSize() const;

  virtual void removePhysicsBody();

  virtual void setPhysicsBody(cocos2d::PhysicsBody* body);

  virtual cocos2d::PhysicsBody* getPhysicsBody();

  virtual void setTexture(const std::string& texName) = 0;

  virtual cocos2d::Rect getBoundingBox() const;

  virtual void setOpacity(GLubyte val);

  virtual GLubyte getOpacity() const;

public:
  std::string TextureName = "images/saw3.png";

protected:
  BlockBase *mParentBlock{nullptr};
};

///////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////

class DirthCircleRenderer : public RectRenderer {
public:
  DirthCircleRenderer(BlockBase *parent);

  ~DirthCircleRenderer();

  RendererType getType() const override {
    return BlockRenderer::TYPE_DEATH_CIRCLE;
  }

  void init(InitParams& param) override;

  void addToParent(cocos2d::Node* parent, int zorder) override;

  void setPosition(const cocos2d::Vec2& pos) override;

  void setRotation(float val) override;

  void setScale(float x, float y) override;

private:
  cocos2d::Sprite* mChild{nullptr};
};

///////////////////////////////////////////////////////////////

class ImageRenderer : public BlockRenderer {
public:
  ImageRenderer(BlockBase *parent);

  ~ImageRenderer();

  RendererType getType() const {
    return BlockRenderer::TYPE_IMAGE;
  }
};

#endif /* BlockRenderer_h */