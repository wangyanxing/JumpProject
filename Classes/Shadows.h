//
//  Shadows.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__Shadows__
#define __JumpEdt__Shadows__

#include "cocos2d.h"

class BlockBase;
namespace cocos2d {
    class DrawNodeEx;
}

class ShadowManager {
public:
    
    ShadowManager(cocos2d::Node* parentNode);
    ~ShadowManager();
    
    void reset();
    
    void update(float dt);
    
    void updateNodes(float dt, std::vector<cocos2d::Node*>& nodes, bool clipX);
    
    void updateShaderParam();
    
    void updateShaderParam(const cocos2d::Vec2& center,
                           const cocos2d::Color3B& colSrc,
                           const cocos2d::Color3B& colDst);
    
    void updateBlockSoft(BlockBase* block, std::vector<cocos2d::V2F_C4B_T2F_Triangle>& triangles, bool clipX = false);
    void updateBlockNormal(BlockBase* block, std::vector<cocos2d::V2F_C4B_T2F_Triangle>& triangles, bool clipX = false);
    
    std::pair<cocos2d::Vec2, cocos2d::Vec2> getShadowEntry(const std::vector<cocos2d::Vec2>& pts);
    
    cocos2d::Vec2 mLightPos;
    cocos2d::Vec2 mOriginLightPos;
    cocos2d::DrawNodeEx* mRendererSoft{ nullptr };
    cocos2d::DrawNodeEx* mRendererNormal{ nullptr };
    
    float mLightMoveTimer{ 0 };
    bool mMoving{ false };
    float mMoveTarget{ 0 };
    float mMovingSpeed{ 0 };
    
    bool mShadowMovingEnable{ true };
    
    float mShadowDarkness{ 0.098f };
    bool mUseSoftShadow{ false }; // using fake soft shadow or not
};


#endif /* defined(__JumpEdt__Shadows__) */
