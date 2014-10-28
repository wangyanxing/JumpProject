//
//  Shadows.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "Shadows.h"
#include "DrawNodeEx.h"
#include "VisibleRect.h"
#include "EditorScene.h"
#include "LogicManager.h"

USING_NS_CC;

void colorMix(const Color4B& src, const Color4B& dst, float r, Color4B& out) {
    out.r = dst.r * r + src.r * (1 - r);
    out.g = dst.g * r + src.g * (1 - r);
    out.b = dst.b * r + src.b * (1 - r);
    out.a = dst.a * r + src.a * (1 - r);
}

ShadowManager::ShadowManager(cocos2d::Node* parentNode) {
    
    mRenderer = DrawNodeEx::create("images/rect.png");
    parentNode->addChild(mRenderer, 2);
    
    mLightPos = VisibleRect::center();
    mLightPos.x = 300;
    mLightPos.y = VisibleRect::top().y - 10;
}

ShadowManager::~ShadowManager() {
}

void ShadowManager::updateBlock(BlockBase* block, std::vector<cocos2d::V2F_C4B_T2F_Triangle>& triangles) {
    Color4B colorBase = Color4B::BLACK;
    colorBase.a = 25;
    
    if(!block->mCastShadow || !block->isVisible())
        return;
    
    std::vector<Vec2> pts;
    block->getPointsForShadow(mLightPos, pts);
    
    float minRet = 1000;
    float maxRet = -1000;
    Vec2 minPt, maxPt;
    
    for(const auto& p : pts) {
        float x = abs(p.x - mLightPos.x);
        float y = abs(p.y - mLightPos.y);
        float ret = atan2(y, x);
        if(p.y - mLightPos.y < 0) {
            ret = 180-ret;
        }
        if(p.x - mLightPos.x < 0) {
            ret = -ret;
        }
        if(ret < minRet) {
            minRet = ret;
            minPt = p;
        }
        if(ret > maxRet) {
            maxRet = ret;
            maxPt = p;
        }
    }
    
    Vec2 minPtFar = minPt - mLightPos;
    float lengthMin = minPtFar.length();
    
    Vec2 maxPtFar = maxPt - mLightPos;
    float lengthMax = maxPtFar.length();
    
    float maxLen = std::max(lengthMax, lengthMin) + block->mShadowLength;
    
    minPtFar.x /= lengthMin;
    minPtFar.y /= lengthMin;
    
    maxPtFar.x /= lengthMax;
    maxPtFar.y /= lengthMax;
    
    auto dir0 = minPtFar;
    auto dir1 = minPtFar;
    auto curPt0 = minPt;
    auto curPt1 = maxPt;
    auto len0 = maxLen - lengthMin;
    auto len1 = maxLen - lengthMax;
    
    for (int i = 0; i < block->mShadowFadeSegments + 1; i++) {
        
        float curlen0 = (i==0) ? block->mShadowFadeRatio * len0 :
        (1.0-block->mShadowFadeRatio) * len0 / block->mShadowFadeSegments;
        
        float curlen1 = (i==0) ? block->mShadowFadeRatio * len1 :
        (1.0-block->mShadowFadeRatio) * len0 / block->mShadowFadeSegments;
        
        Color4B color0 = colorBase;
        Color4B color1 = colorBase;
        
        if(i > 0) {
            colorMix(colorBase, Color4B(0,0,0,0), (i-1.0) / block->mShadowFadeSegments, color0);
            colorMix(colorBase, Color4B(0,0,0,0), (i-1.0) / block->mShadowFadeSegments, color1);
        }
        
        auto pt0 = curPt0;
        auto pt1 = curPt1;
        
        curPt0 = curPt0 + dir0 * curlen0;
        curPt1 = curPt1 + dir1 * curlen1;
        
        V2F_C4B_T2F_Triangle t;
        t.a.vertices = pt0;
        t.a.colors = color0;
        
        t.b.vertices = curPt0;
        t.b.colors = color0;
        
        t.c.vertices = pt1;
        t.c.colors = color0;
        
        triangles.push_back(t);
        
        t.a.vertices = pt1;
        t.a.colors = color1;
        
        t.b.vertices = curPt0;
        t.b.colors = color1;
        
        t.c.vertices = curPt1;
        t.c.colors = color1;
        
        triangles.push_back(t);
    }
}

void ShadowManager::update(float dt) {
    mRenderer->clear();
    std::vector<V2F_C4B_T2F_Triangle> triangles;
    
    for(auto b : GameLogic::Game->mBlocks) {
        auto block = b.second;
        updateBlock(block, triangles);
    }
    updateBlock(GameLogic::Game->mHero, triangles);
    
    if(!triangles.empty())
        mRenderer->drawTriangles(triangles);
    
    if(!mShadowMovingEnable)
        return;
    
    mLightMoveTimer += dt;
    
    bool lightLeft = mLightPos.x <= VisibleRect::center().x;
    bool heroLeft = GameLogic::Game->mHero->getPosition().x <= VisibleRect::center().x;
    
    float dis = abs(mOriginLightPos.x - VisibleRect::center().x);
    
    if(lightLeft == heroLeft && mLightMoveTimer > 3.5) {
        mLightMoveTimer = 0;
        
        mMoveTarget = heroLeft ? VisibleRect::center().x + dis : VisibleRect::center().x - dis;
        mMovingSpeed = (dis * 2) / 3.0f;
        if(!heroLeft) mMovingSpeed*=-1;
        mMoving = true;
    }
    
    if(mMoving) {
        mLightPos.x += mMovingSpeed * dt;
        if(mMovingSpeed > 0) {
            if(mLightPos.x >= VisibleRect::center().x + dis) {
                mLightPos.x = VisibleRect::center().x + dis;
                mMoving = false;
            }
        } else {
            if(mLightPos.x <= VisibleRect::center().x - dis) {
                mLightPos.x = VisibleRect::center().x - dis;
                mMoving = false;
            }
        }
    }
}

void ShadowManager::reset() {
    mLightMoveTimer = 3.5;
    mLightPos = mOriginLightPos;
    mMoving = false;
    mMoveTarget = 0;
    mMovingSpeed = 0;
}
