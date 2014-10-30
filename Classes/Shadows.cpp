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
    
    mOriginLightPos = mLightPos;
}

ShadowManager::~ShadowManager() {
}

void ShadowManager::updateBlock(BlockBase* block, std::vector<cocos2d::V2F_C4B_T2F_Triangle>& triangles) {
    Color4B colorBase = Color4B::BLACK;
    colorBase.a = 255 * mShadowDarkness;
    
    if(!block->mCastShadow || !block->isVisible() || !block->mCanPickup)
        return;
    
    if(block->getSprite()->getBoundingBox().containsPoint(mLightPos))
        return;
    
    std::vector<Vec2> pts;
    block->getPointsForShadow(mLightPos, pts);
    
    Vec2 minPt, maxPt;
    
    CC_ASSERT(pts.size() == 4);
    if(mLightPos.x >= pts[0].x && mLightPos.x <= pts[1].x) {
        if(mLightPos.y >= pts[0].y) {
            minPt = pts[0];
            maxPt = pts[1];
        } else if(mLightPos.y <= pts[2].y) {
            minPt = pts[2];
            maxPt = pts[3];
        }
    } else if(mLightPos.x < pts[0].x) {
        if(mLightPos.y >= pts[0].y) {
            minPt = pts[2];
            maxPt = pts[1];
        } else if(mLightPos.y <= pts[2].y) {
            minPt = pts[0];
            maxPt = pts[3];
        } else {
            minPt = pts[0];
            maxPt = pts[2];
        }
    } else if(mLightPos.x > pts[1].x) {
        if(mLightPos.y >= pts[1].y) {
            minPt = pts[0];
            maxPt = pts[3];
        } else if(mLightPos.y <= pts[3].y) {
            minPt = pts[1];
            maxPt = pts[2];
        } else {
            minPt = pts[1];
            maxPt = pts[3];
        }
    }
    
    Vec2 pa0 = pts[0].getMidpoint(pts[2]);
    Vec2 pa1 = pts[1].getMidpoint(pts[3]);
    
    Vec2 pb0 = pts[0].getMidpoint(pts[1]);
    Vec2 pb1 = pts[2].getMidpoint(pts[3]);
    
    Vec2 OA = minPt - mLightPos;
    Vec2 OB = maxPt - mLightPos;
    
    Vec2 midLine = OA + OB;
    midLine.normalize();
    Vec2 far = mLightPos + midLine * 2000; // make it long enough
    
    bool intersetA = false;
    bool intersetB = false;
    Vec2 interA, interB;
    Vec2 intersection;
    
    if(Vec2::isSegmentIntersect(pa0, pa1, mLightPos, far)) {
        intersetA = true;
        interA = Vec2::getIntersectPoint(pa0, pa1, mLightPos, far);
    }
    if(Vec2::isSegmentIntersect(pb0, pb1, mLightPos, far)) {
        intersetB = true;
        interB = Vec2::getIntersectPoint(pb0, pb1, mLightPos, far);
    }
    if(intersetA && intersetB) {
        intersection = interA.getMidpoint(interB);
    } else if(!intersetA && !intersetB) {
        return;
    } else {
        intersection = intersetA ? interA : interB;
    }
    
    float OX = intersection.distance(mLightPos);

    const float PI = 3.14159265;
    float lenOA = OA.length();
    float lenOB = OB.length();
    
    float ag = midLine.getAngle(OA);
    if(ag < 0) {
        ag += PI;
    }
    if(ag > PI / 2) {
        ag = PI - ag;
    }
    float cosTheta = cos(ag);
    float lengthA = (OX + block->mShadowLength) / cosTheta - lenOA;
    float lengthB = (OX + block->mShadowLength) / cosTheta - lenOB;
    
    auto dir0 = OA/lenOA;
    auto dir1 = OB/lenOB;
    auto curPt0 = minPt;
    auto curPt1 = maxPt;
    auto len0 = lengthA;
    auto len1 = lengthB;
    
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
    
    if(!mShadowMovingEnable || !GameLogic::Game->mGameMode)
        return;
    
    mLightMoveTimer += dt;
    
    bool lightLeft = mLightPos.x <= VisibleRect::center().x;
    bool heroLeft = GameLogic::Game->mHero->getPosition().x <= VisibleRect::center().x;
    
    float dis = abs(mOriginLightPos.x - VisibleRect::center().x);
    
    if(lightLeft == heroLeft && mLightMoveTimer > 10) {
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
    mLightMoveTimer = 10;
    mLightPos = mOriginLightPos;
    mMoving = false;
    mMoveTarget = 0;
    mMovingSpeed = 0;
}
