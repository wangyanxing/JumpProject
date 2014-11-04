//
//  LevelSelector.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/30/14.
//
//

#ifndef __JumpEdt__LevelSelector__
#define __JumpEdt__LevelSelector__

#include "cocos2d.h"

class BlockBase;
class ShadowManager;
class WaveEffect;

enum WorldID {
    TITLE,
    WORLD_1,
    WORLD_2,
    WORLD_3,
    WORLD_4,
};

class LevelSelLayer : public cocos2d::Layer {
public:
    
    CREATE_FUNC(LevelSelLayer);
    
    LevelSelLayer() = default;
    
    virtual ~LevelSelLayer();
    
public:
    
    virtual void onEnter();
    
    virtual bool init();
    
    void initWorld(WorldID world);
    
    void cleanBlocks();
    
    void update(float dt);
    
    void setBackGradientColor(const cocos2d::Color3B& colorSrc,
                              const cocos2d::Color3B& colorDst);
    
    void setBackGradientCenter(const cocos2d::Vec2& pos);
    
    bool isTapToMove() { return mTapToMove; }
    
private:
    
    void onTouch(const cocos2d::Vec2& pos);
    void onEndTouch(const cocos2d::Vec2& pos);
    
    void initAsTitle();
    void initAsWorld1();
    void initAsWorld2();
    
    void createBlock(int id, const cocos2d::Vec2& pt);
    
    std::vector<cocos2d::Node*> mBlocks;
    
    ShadowManager* mShadows{ nullptr };
    
    cocos2d::Sprite* mBack{ nullptr };
    
    WaveEffect* mWaveFx{ nullptr };
    
    WorldID mWorld{ TITLE };
    
    bool mTapToMove{ false };
};

#endif /* defined(__JumpEdt__LevelSelector__) */
