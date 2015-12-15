#ifndef __SHADERLAYER_H__
#define __SHADERLAYER_H__

#include "cocos2d.h"
#include "Defines.h"
#include <string>
USING_NS_CC;

#if USE_SHADER_LAYER

#define VIG 1

class ShaderLayer : public cocos2d::Layer {
public:
  ShaderLayer();

  ~ShaderLayer();

  bool enableShaderLayer{ false };

  virtual bool init(std::string pixelShaderFile,
                    std::string vertexShaderFile = "shaders/generic.vsh");

  static ShaderLayer* create(std::string pixelShaderFile,
                             std::string vertexShaderFile = "shaders/generic.vsh");

  RenderTexture* renderTexture{ nullptr };

  RenderTexture* renderTextureDownscale{ nullptr };

  RenderTexture* renderTextureBlur{ nullptr };

  Sprite* rendTexSprite{ nullptr };

  Sprite* rendTexSpriteBlur{ nullptr };

  Sprite* rendTexSpriteFinal{ nullptr };

  int paramBlending{ 0 };

  Vec2 darkness{1, 1};

  virtual void visit(Renderer *renderer,
                     const Mat4& parentTransform,
                     uint32_t parentFlags) override;
private:
  GLProgram* p;
};

#endif
#endif
