//
//  EffectSprite.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 11/4/14.
//
//

#include "EffectSprite.h"

static int tuple_sort( const std::tuple<ssize_t,Effect*,QuadCommand> &tuple1, const std::tuple<ssize_t,Effect*,QuadCommand> &tuple2 )
{
    return std::get<0>(tuple1) < std::get<0>(tuple2);
}

EffectSprite *EffectSprite::create(const std::string& filename) {
    auto ret = new (std::nothrow) EffectSprite;
    if(ret && ret->initWithFile(filename)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE(ret);
    return nullptr;
}

EffectSprite *EffectSprite::create() {
    auto ret = new (std::nothrow) EffectSprite;
    if(ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE(ret);
    return nullptr;
}

void EffectSprite::setEffect(Effect* effect) {
    if(_defaultEffect != effect) {
        effect->setTarget(this);
        
        CC_SAFE_RELEASE(_defaultEffect);
        _defaultEffect = effect;
        CC_SAFE_RETAIN(_defaultEffect);
        
        setGLProgramState(_defaultEffect->getGLProgramState());
    }
}
void EffectSprite::addEffect(Effect *effect, ssize_t order) {
    effect->retain();
    effect->setTarget(this);
    
    _effects.push_back(std::make_tuple(order,effect,QuadCommand()));
    
    std::sort(std::begin(_effects), std::end(_effects), tuple_sort);
}

void EffectSprite::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    // Don't do calculate the culling if the transform was not updated
    _insideBounds = (flags & FLAGS_TRANSFORM_DIRTY) ? renderer->checkVisibility(transform, _contentSize) : _insideBounds;
    
    if(_insideBounds)
    {
        // negative effects: order < 0
        int idx=0;
        for(auto &effect : _effects) {
            
            if(std::get<0>(effect) >=0)
                break;
            QuadCommand &q = std::get<2>(effect);
            q.init(_globalZOrder, _texture->getName(), std::get<1>(effect)->getGLProgramState(), _blendFunc, &_quad, 1, transform, flags);
            renderer->addCommand(&q);
            idx++;
            
        }
        
        // normal effect: order == 0
        _trianglesCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, _polyInfo.triangles, transform, flags);
        renderer->addCommand(&_trianglesCommand);
        
        // postive effects: oder >= 0
        for(auto it = std::begin(_effects)+idx; it != std::end(_effects); ++it) {
            QuadCommand &q = std::get<2>(*it);
            q.init(_globalZOrder, _texture->getName(), std::get<1>(*it)->getGLProgramState(), _blendFunc, &_quad, 1, transform, flags);
            renderer->addCommand(&q);
            idx++;
        }
    }
}

EffectSprite::EffectSprite() : _defaultEffect(nullptr)
{
    _effects.reserve(2);
}

EffectSprite::~EffectSprite() {
    for(auto &tuple : _effects) {
        std::get<1>(tuple)->release();
    }
    CC_SAFE_RELEASE(_defaultEffect);
}


bool Effect::initGLProgramState(const std::string &fragmentFilename)
{
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename(fragmentFilename);
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    auto glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _fragSource = fragSource;
#endif
    
    _glprogramstate = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _glprogramstate->retain();
    
    return _glprogramstate != nullptr;
}

Effect::Effect()
: _glprogramstate(nullptr)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _backgroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED,
                                                      [this](EventCustom*)
                                                      {
                                                          auto glProgram = _glprogramstate->getGLProgram();
                                                          glProgram->reset();
                                                          glProgram->initWithByteArrays(ccPositionTextureColor_noMVP_vert, _fragSource.c_str());
                                                          glProgram->link();
                                                          glProgram->updateUniforms();
                                                      }
                                                      );
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backgroundListener, -1);
#endif
}

Effect::~Effect()
{
    CC_SAFE_RELEASE_NULL(_glprogramstate);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backgroundListener);
#endif
}

void EffectBlur::setTarget(EffectSprite *sprite)
{
    Size size = sprite->getTexture()->getContentSizeInPixels();
    _glprogramstate->setUniformVec2("resolution", size);
    _glprogramstate->setUniformFloat("blurRadius", _blurRadius);
    _glprogramstate->setUniformFloat("sampleNum", _blurSampleNum);
}

bool EffectBlur::init(float blurRadius, float sampleNum)
{
    initGLProgramState("shaders/blur.fsh");
    _blurRadius = blurRadius;
    _blurSampleNum = sampleNum;
    
    return true;
}

void EffectBlur::setBlurRadius(float radius)
{
    _blurRadius = radius;
}

void EffectBlur::setBlurSampleNum(float num)
{
    _blurSampleNum = num;
}
