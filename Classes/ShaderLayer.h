#ifndef __SHADERLAYER_H__
#define __SHADERLAYER_H__

#include "cocos2d.h"
#include <string>
USING_NS_CC;

class ShaderLayer : public cocos2d::Layer {
	GLProgram* p;
public:
	ShaderLayer();
	~ShaderLayer();
    
    bool enableShaderLayer{ false };

    virtual bool init(
		std::string pixelShaderFile,
		std::string vertexShaderFile = "shaders/generic.vsh");
    
	static ShaderLayer* create(
		std::string pixelShaderFile,
		std::string vertexShaderFile = "shaders/generic.vsh");

	RenderTexture* renderTexture;
	Sprite* rendTexSprite;
    
    int paramBlending{ 0 };
    Vec2 darkness{1,1};

	virtual void visit(
		Renderer *renderer,
		const Mat4& parentTransform,
        uint32_t parentFlags) override;
};

#endif
