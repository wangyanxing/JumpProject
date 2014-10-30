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

    auto b = getBoundingBox();
    if(b.size.width < b.size.height) {
        Texture2D::TexParams params = { GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_REPEAT };
        this->getTexture()->setTexParameters(params);
    } else {
        Texture2D::TexParams params = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE };
        this->getTexture()->setTexParameters(params);
    }
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

void SpriteUV::setUVHeight(float length) {
    _quad.bl.texCoords.v = length;
    _quad.br.texCoords.v = length;
}

void SpriteUV::setUVWidth(float length) {
    _quad.tr.texCoords.u = length;
    _quad.br.texCoords.u = length;
}

void SpriteUV::setUVSize(float val) {
    
}

void SpriteUV::rotateUV() {
    auto quad = _quad;
    _quad.tl.texCoords = quad.tr.texCoords;
    _quad.tr.texCoords = quad.br.texCoords;
    _quad.bl.texCoords = quad.tl.texCoords;
    _quad.br.texCoords = quad.bl.texCoords;
}

void SpriteUV::flipUVX() {
    std::swap(_quad.tl.texCoords, _quad.tr.texCoords);
    std::swap(_quad.bl.texCoords, _quad.br.texCoords);
}

void SpriteUV::flipUVY() {
    std::swap(_quad.tl.texCoords, _quad.bl.texCoords);
    std::swap(_quad.tr.texCoords, _quad.br.texCoords);
}

void SpriteUV::resetUV() {
    _quad.tl.texCoords.u = 0;
    _quad.tl.texCoords.v = 0;
    
    _quad.bl.texCoords.u = 0;
    _quad.bl.texCoords.v = 1;
    
    _quad.tr.texCoords.u = 1;
    _quad.tr.texCoords.v = 0;
    
    _quad.br.texCoords.u = 1;
    _quad.br.texCoords.v = 1;
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
