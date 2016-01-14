//
//  GameRenderer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#ifndef GameRenderer_h
#define GameRenderer_h

#include "Defines.h"
#include "Parameter.h"
#include "GameTypes.h"

class GameObject;

class GameRenderer {
public:
  GameRenderer(GameObject *parent) : mParent(parent) {}

  virtual ~GameRenderer() = default;

  virtual RendererType getType() const = 0;

  virtual void init(Parameter &param) = 0;

  virtual cocos2d::Node *getNode() = 0;

  virtual cocos2d::Node *getNode() const = 0;

  virtual void update(float dt) {}

  virtual void addToParent(cocos2d::Node *parent, int zorder);

  virtual void setZOrder(int val);

  virtual void setPosition(const cocos2d::Vec2 &pos);

  virtual cocos2d::Vec2 getPosition() const;

  virtual void setColor(const cocos2d::Color3B &color);

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

  virtual cocos2d::Size getSize() const;

  virtual cocos2d::Size getContentSize() const;

  virtual void setTexture(const std::string& texName) {}

  virtual cocos2d::Rect getBoundingBox() const;

  virtual void setOpacity(GLubyte val);
  
  virtual GLubyte getOpacity() const;

protected:
  GameObject *mParent{nullptr};
};

#endif /* GameRenderer_h */
