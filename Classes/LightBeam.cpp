//
//  LightBeam.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 11/3/14.
//
//

#include "LightBeam.h"
#include "LogicManager.h"
#include "VisibleRect.h"

USING_NS_CC;

LightBeamDesc LightBeamDesc::DEFAULT;

void LightBeam::init(const LightBeamDesc& desc) {
    mNode = DrawNodeEx::create(desc.texture);
    mNode->setBlendFunc({GL_ONE, GL_ONE});
    mNode->setTag(GLOW_NODE_TAG);
    
    GameLogic::Game->mParentLayer->addChild(mNode,desc.zorder);
    
    auto shaderfile = FileUtils::getInstance()->fullPathForFilename("shaders/light_beam.fsh");
    GLchar * fragSource = (GLchar*)String::createWithContentsOfFile(shaderfile.c_str())->getCString();
    auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, fragSource);
    auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);
    mNode->setGLProgramState(glProgramState);
    
    mNode->setPosition(VisibleRect::center());
    
    mUVAnimTimer = desc.uvStart;
    mDesc = desc;
}

void LightBeam::update(float dt) {
    mNode->clear();
    
    std::vector<cocos2d::V2F_C4B_T2F_Triangle> triangles;
    triangles.reserve(50);
    
    float upperLength = mDesc.segSizeUpper * mDesc.segNums;
    float lowerLength = mDesc.segSizeLower * mDesc.segNums;
    
    Texture2D::TexParams params = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE };
    mNode->getTexture()->setTexParameters(params);
    //mNode->getTexture()->setAntiAliasTexParameters();
    
    mUVAnimTimer += dt * mDesc.uvSpeed;
    
    if(mUVAnimTimer > 1) mUVAnimTimer -= 1;
    if(mUVAnimTimer < -1) mUVAnimTimer += 1;
    
    for (int i = 0; i < mDesc.segNums; ++i) {
        auto col0 = mDesc.color;
        auto col1 = mDesc.color;
        
        if(i == 0) col0 = Color4B::BLACK;
        if(i == mDesc.segNums-1) col1 = Color4B::BLACK;
        
        float x0_u = i * mDesc.segSizeUpper - upperLength / 2;
        float x1_u = (i+1) * mDesc.segSizeUpper - upperLength / 2;
        
        float x0_l = i * mDesc.segSizeLower - lowerLength / 2;
        float x1_l = (i+1) * mDesc.segSizeLower - lowerLength / 2;
        
        float y0_u = mDesc.height/2;
        float y1_u = mDesc.height/2;
        float y0_l = -mDesc.height/2;
        float y1_l = -mDesc.height/2;
        
        float ratio0 = i / (float)mDesc.segNums * mDesc.uvScale + mUVAnimTimer;
        float ratio1 = (i+1) / (float)mDesc.segNums * mDesc.uvScale + mUVAnimTimer;
        
        V2F_C4B_T2F_Triangle t;
        t.a.vertices.set(x0_u, y0_u);
        t.a.colors = col0;
        t.a.texCoords = Tex2F(ratio0, mDesc.uvUpper);
        
        t.b.vertices.set(x0_l, y0_l);
        t.b.colors = col0;
        t.b.texCoords = Tex2F(ratio0, mDesc.uvLower);
        
        t.c.vertices.set(x1_u, y1_u);
        t.c.colors = col1;
        t.c.texCoords = Tex2F(ratio1, mDesc.uvUpper);
        
        triangles.push_back(t);
        
        t.a.vertices.set(x0_l, y0_l);
        t.a.colors = col0;
        t.a.texCoords = Tex2F(ratio0, mDesc.uvLower);
        
        t.b.vertices.set(x1_l, y1_l);
        t.b.colors = col1;
        t.b.texCoords = Tex2F(ratio1, mDesc.uvLower);
        
        t.c.vertices.set(x1_u, y1_u);
        t.c.colors = col1;
        t.c.texCoords = Tex2F(ratio1, mDesc.uvUpper);
        
        triangles.push_back(t);
    }
    
    mNode->drawTriangles(triangles);
}

void LightBeam::destroy() {
    mNode->clear();
}