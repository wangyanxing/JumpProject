#include "ShaderLayer.h"
#include "VisibleRect.h"

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
    
    auto texRect = Rect(0, 0, texSzie.width, texSzie.height);
    auto texRect2 = Rect(0, 0, visibleSize.width/4, visibleSize.height/4);
    
    renderTexture->retain();
    renderTextureBlur->retain();
	
    rendTexSprite = Sprite::create();
	rendTexSprite->setTexture(renderTexture->getSprite()->getTexture());
	rendTexSprite->setTextureRect(texRect);
	rendTexSprite->setPosition(Point::ZERO);
	rendTexSprite->setAnchorPoint(Point::ZERO);
	rendTexSprite->setFlippedY(true);
    rendTexSprite->retain();
    
    rendTexSpriteBlur = Sprite::create();
    rendTexSpriteBlur->setTexture(renderTextureBlur->getSprite()->getTexture());
    rendTexSpriteBlur->setTextureRect(texRect2);
    rendTexSpriteBlur->setPosition(Point::ZERO);
    rendTexSpriteBlur->setAnchorPoint(Point::ZERO);
    rendTexSpriteBlur->setFlippedY(true);
    rendTexSpriteBlur->retain();

	rendTexSprite->setGLProgram(p);
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
        renderTextureBlur->beginWithClear(0, 0, 0, 0);
        rendTexSprite->visit(renderer, parentTransform, parentFlags);
        renderTextureBlur->end();
        
        rendTexSpriteBlur->visit(renderer, parentTransform, parentFlags);
#else
        rendTexSprite->visit(renderer, parentTransform, parentFlags);
#endif

    } else {
        Layer::visit(renderer, parentTransform, parentFlags);
#if 0
        renderTexture->beginWithClear(0, 0, 0, 0);
        for (auto child : getChildren()) {
            if (child != renderTexture && child != rendTexSprite && child->getTag() != 1024)
                child->visit(renderer, parentTransform, parentFlags);
        }
        renderTexture->end();
#endif
    }
}
