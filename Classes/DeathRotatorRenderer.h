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

#define DEFAULT_DEATH_ROTATOR_SPEED 80

class DeathRotatorRenderer : public SimpleRenderer {
public:
  DeathRotatorRenderer(GameObject *parent);

  ~DeathRotatorRenderer();

  RendererType getType() const override {
    return RENDERER_DEATH_CIRCLE;
  }

  void init(Parameter& param) override;

  void update(float dt) override;

  void addToParent(cocos2d::Node* parent, int zorder) override;

  void setPosition(const cocos2d::Vec2& pos) override;

  void setRotation(float val) override;

  void setScale(float x, float y) override;

  void setColor(const cocos2d::Color3B& color) override;

  void setRotationSpeed(int speed);

  int getRotationSpeed();
  
private:
  cocos2d::Sprite* mChild{nullptr};

  int mRotationSpeed{DEFAULT_DEATH_ROTATOR_SPEED};
};

#endif /* DeathRotatorRenderer_h */
