#include "SpriteUV.h"

using namespace cocos2d;

SpriteUV* SpriteUV::create(const std::string& filename)
{
    SpriteUV *sprite = new (std::nothrow) SpriteUV();
    if (sprite && sprite->initWithFile(filename.c_str()))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

SpriteUV::SpriteUV()
{
}


SpriteUV::~SpriteUV()
{
}

void SpriteUV::setupTexParameters(){

    Texture2D::TexParams params = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
    this->getTexture()->setTexParameters(params);

}

bool SpriteUV::initWithFile(const char *pszFilename)
{
    if( CCSprite::initWithFile(pszFilename) ) 
    {
        setupTexParameters();
      
        return true;
    }
    
    return false;
}


bool SpriteUV::initWithSpriteFrameName(const char *pszFilename)
{
    if( CCSprite::initWithSpriteFrameName(pszFilename) ) 
    {
        setupTexParameters();
       
        return true;
    }
    
    return false;
}

void SpriteUV::setUVLength(float length) {
    _quad.tr.texCoords.u = length;
    _quad.br.texCoords.u = length;
}

void SpriteUV::setUVOffset(cocos2d::Point offset){

    V3F_C4B_T2F_Quad quad = this->getQuad();
   
    //change UV coords
    quad.tl.texCoords.u+=offset.x;
    quad.bl.texCoords.u+=offset.x;
    quad.tr.texCoords.u+=offset.x;
    quad.br.texCoords.u+=offset.x;
    
    quad.tl.texCoords.v+=offset.y;
    quad.bl.texCoords.v+=offset.y;
    quad.tr.texCoords.v+=offset.y;
    quad.br.texCoords.v+=offset.y;

    //write quad back
    this->_quad = quad;

}
