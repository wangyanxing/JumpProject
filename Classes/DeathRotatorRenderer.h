//
//  DeathRotatorRenderer.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#ifndef DeathRotatorRenderer_h
#define DeathRotatorRenderer_h

#include "SimpleRenderer.h"

class DeathRotatorRenderer : public SimpleRenderer {
public:
  DeathRotatorRenderer(GameObject *parent);

  ~DeathRotatorRenderer();

  RendererType getType() const override {
    return RENDERER_DEATH_CIRCLE;
  }

  GameRenderer *init(Parameter& param) override;

  void clone(GameRenderer *renderer) override;

  void setSize(const cocos2d::Size &size) override;

  cocos2d::Size getSize() const override;

  void load(JsonValueT &json) override;

  void save(JsWriter &writer) override;

  void update(float dt) override;

  void addToParent(cocos2d::Node* parent, int zorder) override;

  void setPosition(const cocos2d::Vec2& pos) override;

  void setRotation(float val) override;

  void setScale(float x, float y) override;

  void setColor(const cocos2d::Color3B& color) override;

  void reSize(const cocos2d::Vec2 &delta) override;

  void setRotationSpeed(int speed);

  int getRotationSpeed();
  
private:
  cocos2d::Sprite* mChild{nullptr};

  int mRotationSpeed{DEFAULT_DEATH_ROTATOR_SPEED};
};

#endif /* DeathRotatorRenderer_h */
