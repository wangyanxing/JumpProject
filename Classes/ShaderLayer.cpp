#include "ShaderLayer.h"
#include "VisibleRect.h"
#include "Defines.h"

using namespace std;

ShaderLayer::ShaderLayer() {
  p = nullptr;
}

ShaderLayer::~ShaderLayer() {
  renderTexture->release();
  renderTextureBlur->release();
  rendTexSprite->release();
  rendTexSpriteBlur->release();
}

ShaderLayer* ShaderLayer::create(string pixelShaderFile, string vertexShaderFile) {
  ShaderLayer *pRet = new ShaderLayer();
  if (pRet && pRet->init(pixelShaderFile, vertexShaderFile)) {
    pRet->autorelease();
    return pRet;
  } else {
    delete pRet;
    pRet = nullptr;
    return nullptr;
  }
}

bool ShaderLayer::init(string pixelShaderFile, string vertexShaderFile) {
  if ( !Layer::init() ) {
    return false;
  }

  p = GLProgram::createWithFilenames(vertexShaderFile, pixelShaderFile);

  Size visibleSize = VisibleRect::getVisibleRect().size;

  Size texSzie = visibleSize;

  renderTexture = RenderTexture::create(texSzie.width, texSzie.height);
  renderTextureBlur = RenderTexture::create(visibleSize.width/4, visibleSize.height/4);
  renderTextureDownscale = RenderTexture::create(visibleSize.width/4, visibleSize.height/4);

  auto texRect = Rect(0, 0, texSzie.width, texSzie.height);
  auto texRect2 = Rect(0, 0, visibleSize.width/4, visibleSize.height/4);

  renderTexture->retain();
  renderTextureBlur->retain();
  renderTextureDownscale->retain();

  rendTexSprite = Sprite::create();
  rendTexSprite->setTexture(renderTexture->getSprite()->getTexture());
  rendTexSprite->setTextureRect(texRect);
  rendTexSprite->setPosition(Point::ZERO);
  rendTexSprite->setAnchorPoint(Point::ZERO);
  rendTexSprite->setFlippedY(true);
  rendTexSprite->setGLProgram(p);
  rendTexSprite->retain();

  rendTexSpriteBlur = Sprite::create();
  rendTexSpriteBlur->setTexture(renderTextureDownscale->getSprite()->getTexture());
  rendTexSpriteBlur->setTextureRect(texRect2);
  rendTexSpriteBlur->setPosition(Point::ZERO);
  rendTexSpriteBlur->setAnchorPoint(Point::ZERO);
  rendTexSpriteBlur->setFlippedY(true);
#if 1
  rendTexSpriteBlur->setGLProgram(GLProgram::createWithFilenames("shaders/generic.vsh", "shaders/blur.fsh"));

  auto _glprogramstate = rendTexSpriteBlur->getGLProgramState();
  _glprogramstate->setUniformVec2("resolution", Vec2(visibleSize.width, visibleSize.height));
  _glprogramstate->setUniformFloat("blurRadius", 10);
  _glprogramstate->setUniformFloat("sampleNum", 5);
#endif
  rendTexSpriteBlur->retain();

  rendTexSpriteFinal = Sprite::create();
  rendTexSpriteFinal->setTexture(renderTextureBlur->getSprite()->getTexture());
  rendTexSpriteFinal->setTextureRect(texRect);
  rendTexSpriteFinal->setPosition(Point::ZERO);
  rendTexSpriteFinal->setAnchorPoint(Point::ZERO);
  rendTexSpriteFinal->setFlippedY(true);
  rendTexSpriteFinal->retain();

  return true;
}

void ShaderLayer::visit( Renderer *renderer,
                        const Mat4& parentTransform,
                        uint32_t parentFlags) {

  if(enableShaderLayer) {
    float dt = 1.0 / 60;
    if (paramBlending > 0) {
      darkness.y -= (float)paramBlending * dt;
      if(darkness.y <= 0){
        darkness.y = 0;
        paramBlending *= -1;
      }
#if VIG
      rendTexSprite->getGLProgramState()->setUniformVec2("darkness", darkness);
#endif
    } else if(paramBlending < 0) {
      darkness.y -= (float)paramBlending * dt;
      if(darkness.y >= 1) {
        darkness.y = 1;
        paramBlending = 0;
        enableShaderLayer = false;
      }
#if VIG
      rendTexSprite->getGLProgramState()->setUniformVec2("darkness", darkness);
#endif
    }

    renderTexture->beginWithClear(0, 0, 0, 0);
    sortAllChildren();

    for (auto child : getChildren()) {
      if (child != renderTexture && child != rendTexSprite)
        child->visit(renderer, parentTransform, parentFlags);
    }

    renderTexture->end();

#if !VIG

    // original scene -> down scale
    // down scale -> blur
    // blur -> get blured texture!

    renderTextureDownscale->beginWithClear(0, 0, 0, 0);
    rendTexSprite->visit(renderer, parentTransform, parentFlags);
    renderTextureDownscale->end();

    renderTextureBlur->beginWithClear(0, 0, 0, 0);
    rendTexSpriteBlur->visit(renderer, parentTransform, parentFlags);
    renderTextureBlur->end();

    //rendTexSpriteBlur->visit(renderer, parentTransform, parentFlags);
#else
    rendTexSprite->visit(renderer, parentTransform, parentFlags);
#endif

  } else {
    Layer::visit(renderer, parentTransform, parentFlags);

#if !VIG
    renderTexture->beginWithClear(0, 0, 0, 0);
    for (auto child : getChildren()) {
      if (child->getTag() == GLOW_NODE_TAG)
        child->visit(renderer, parentTransform, parentFlags);
    }
    renderTexture->end();

    renderTextureDownscale->beginWithClear(0, 0, 0, 0);
    rendTexSprite->visit(renderer, parentTransform, parentFlags);
    renderTextureDownscale->end();

    renderTextureBlur->beginWithClear(0, 0, 0, 0);
    rendTexSpriteBlur->visit(renderer, parentTransform, parentFlags);
    renderTextureBlur->end();
#endif
  }
}
