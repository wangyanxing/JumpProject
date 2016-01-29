//
//  GameRenderer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#ifndef GameRenderer_h
#define GameRenderer_h

#include "Prerequisites.h"
#include "Parameter.h"
#include "JsonParser.h"

class GameRenderer {
public:
  GameRenderer(GameObject *parent) : mParent(parent) {}

  virtual ~GameRenderer() = default;

  virtual RendererType getType() const = 0;

  virtual GameRenderer *init(Parameter &param) = 0;

  virtual cocos2d::Node *getNode() = 0;

  virtual cocos2d::Node *getNode() const = 0;

  virtual void reset();

  virtual void update(float dt) {}

  virtual void load(JsonValueT &json);

  virtual void addToParent(cocos2d::Node *parent, int zorder);

  virtual void setZOrder(int val);

  virtual void setSize(const cocos2d::Size &size);

  /**
   * This will change the physics part.
   */
  virtual void setPosition(const cocos2d::Vec2 &pos);

  virtual cocos2d::Vec2 getPosition() const;

  virtual cocos2d::Vec2 getOriginalPosition() const;

  virtual void setColor(const cocos2d::Color3B &color);

  virtual cocos2d::Color3B getColor() const;

  virtual void setColorIndex(int id);

  virtual int getColorIndex() const;

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

  virtual cocos2d::Size getOriginalSize() const;

  virtual void setTexture(const std::string& texName) {}

  virtual cocos2d::Rect getBoundingBox() const;

  virtual void setOpacity(GLubyte val);
  
  virtual GLubyte getOpacity() const;

  void getPointsForShadow(const cocos2d::Vec2 &source, std::vector <cocos2d::Vec2> &out);

  bool isShadowEnabled() const {
    return mShadowEnabled;
  }

  void setShadowEnabled(bool val) {
    mShadowEnabled = val;
  }

  int getShadowLayer() const {
    return mShadowLayer;
  }

  void setShadowLayer(int val) {
    mShadowLayer = val;
  }

  int getShadowGroup() const {
    return mShadowGroup;
  }

  void setShadowGroup(int val) {
    mShadowGroup = val;
  }

protected:
  GameObject *mParent{nullptr};

  int mColorIndex{DEFAULT_COLOR_ID};

  bool mShadowEnabled{true};

  int mShadowLayer{0};

  int mShadowGroup{0};

  cocos2d::Size mOriginalSize;

  cocos2d::Vec2 mOriginalPosition;

  float mOriginalRotation{0};
};

#endif /* GameRenderer_h */
