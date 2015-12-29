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

class BlockRenderer {
public:
  enum RendererType {
    TYPE_RECT,
    TYPE_IMAGE
  };

  enum InitParamType{
    PARAM_RECT,   // cocos2d::Rect
    PARAM_COLOR,  // cocos2d::Color3B
    PARAM_IMAGE,  // std::string
  };

  typedef std::map<InitParamType, Any> InitParams;

  BlockRenderer() = default;

  virtual ~BlockRenderer() = default;

  virtual RendererType getType() const = 0;

  virtual void init(InitParams& param) = 0;

  virtual cocos2d::Node* getNode() = 0;

  virtual void addToParent(cocos2d::Node* parent, int zorder) = 0;

  virtual void setZOrder(int val) = 0;

  virtual void setPosition(const cocos2d::Vec2& pos) = 0;

  virtual cocos2d::Vec2 getPosition() const = 0;

  virtual void setColor(const cocos2d::Color3B& color) = 0;

  virtual cocos2d::Color3B getColor() const = 0;

  virtual void setVisible(bool val) = 0;

  virtual bool isVisible() const = 0;

  virtual void setRotation(float val) = 0;

  virtual float getRotation() const = 0;

  virtual void setScale(float x, float y) = 0;

  virtual void setScale(float scale) {
    setScale(scale, scale);
  }

  virtual float getScaleX() const = 0;

  virtual float getScaleY() const = 0;

  virtual cocos2d::Size getContentSize() const = 0;

  virtual void removePhysicsBody() = 0;

  virtual void setPhysicsBody(cocos2d::PhysicsBody* body) = 0;

  virtual cocos2d::PhysicsBody* getPhysicsBody() = 0;

  virtual void setTexture(cocos2d::Texture2D* tex) = 0;

  virtual void setTexture(const std::string& texName) = 0;

  virtual cocos2d::Rect getBoundingBox() const = 0;

  virtual void setOpacity(GLubyte val) = 0;

  virtual GLubyte getOpacity() const = 0;
};

/////////////////////

class RectRenderer : public BlockRenderer {
public:
  RectRenderer();

  ~RectRenderer();

  RendererType getType() const {
    return BlockRenderer::TYPE_RECT;
  }

  void init(InitParams& param);

  void setZOrder(int val);

  void setPosition(const cocos2d::Vec2& pos);

  cocos2d::Vec2 getPosition() const;

  SpriteUV* getSprite() {
    return mSprite;
  }

  void setRotation(float val);

  float getRotation() const;

  void setScale(float x, float y);

  float getScaleX() const;

  float getScaleY() const;

  void setVisible(bool val);

  bool isVisible() const;

  cocos2d::Size getContentSize() const;

  void setColor(const cocos2d::Color3B& color);

  cocos2d::Color3B getColor() const;

  void removePhysicsBody();

  void setPhysicsBody(cocos2d::PhysicsBody* body);

  cocos2d::PhysicsBody* getPhysicsBody();

  void setTexture(cocos2d::Texture2D* tex);

  void setTexture(const std::string& texName);

  void setOpacity(GLubyte val);

  GLubyte getOpacity() const;

  void addToParent(cocos2d::Node* parent, int zorder);

  cocos2d::Rect getBoundingBox() const;

  cocos2d::Node* getNode();

private:
  SpriteUV* mSprite{nullptr};
};

/////////////////////

class ImageRenderer : public BlockRenderer {
public:
  ImageRenderer();

  ~ImageRenderer();

  RendererType getType() const {
    return BlockRenderer::TYPE_IMAGE;
  }
};

#endif /* BlockRenderer_h */
