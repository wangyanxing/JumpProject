//
//  Path.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "Path.h"

void Path::update(float dt, cocos2d::Vec2& out, cocos2d::Vec2& outsize) {
	if (mPoints.empty() || mPoints.size()==1) return;
    if(mPause) dt = 0;
    
    if(mDisable) {
        out = mPoints[0].pt;
        
    } else {
        
        // process path timer
        if(mPathWaitingTimer > mPathWaitTime) {
            int nextPt = nextPoint();
            
            if(mWaitingTimer > mPoints[mCurPt].waitTime) {
                auto pt = mPoints[mCurPt];
                auto ptNext = mPoints[nextPt];
                float dist = pt.pt.distance(ptNext.pt);
                
                mCurDist += mSpeed * dt;
                if(mCurDist >= dist) {
                    // move to next point
                    mCurDist -= dist;
                    mWaitingTimer = 0;
                    
                    if(mDirection) {
                        if(nextPt == mPoints.size() - 1 && mPingPong) {
                            mDirection = !mDirection;
                        }
                    } else {
                        if(nextPt == 0 && mPingPong) {
                            mDirection = !mDirection;
                        }
                    }
                    mCurPt = nextPt;
                    nextPt = nextPoint();
                    
                    pt = mPoints[mCurPt];
                    ptNext = mPoints[nextPt];
                }
                auto dir = ptNext.pt - pt.pt;
                dir.x /= dist;
                dir.y /= dist;
                out = pt.pt + dir * mCurDist;
                
                float ratio = mCurDist / dist;
                outsize.x = pt.width * (1-ratio) + ptNext.width * ratio;
                outsize.y = pt.height * (1-ratio) + ptNext.height * ratio;
                
            } else {
                out = mPoints[mCurPt].pt;
                mWaitingTimer += dt;
            }
            
        } else {
            mPathWaitingTimer += dt;
        }
    }
    
#if EDITOR_MODE
    mSegmentNode->clear();
    for(size_t i = 0; i < mPoints.size() - 1; ++i) {
        mSegmentNode->drawSegment(mPoints[i].pt, mPoints[i+1].pt, 1, cocos2d::Color4F(0,1,0,1));
    }
#endif
}

void Path::pop() {
    if(empty()) return;
    
#if EDITOR_MODE
    auto back = mPoints.back();
    auto node = mHelperNode->getChildByTag(mPoints.size() - 1);
    node->removeFromParent();
#else
    mPoints.back();
#endif
    
    mPoints.pop_back();
}

void Path::push(const cocos2d::Vec2& pos, float waitTime, float width, float height) {
    PathPoint pt(pos, waitTime, width ,height);
    mPoints.push_back(pt);
#if EDITOR_MODE
    auto sprite = cocos2d::Sprite::create("images/circle.png");
    sprite->setTag(mPoints.size() - 1);
    sprite->setPosition(pos);
    sprite->setScale(0.7);
    mHelperNode->addChild(sprite, 100);
#endif
}

void Path::cloneFrom(const Path& rsh, const cocos2d::Vec2& posBias) {
    reset();
    for(size_t i = 0; i < rsh.mPoints.size(); ++i) {
        const auto& p = rsh.mPoints[i];
        push(p.pt + posBias, p.waitTime, p.width, p.height);
    }
    mSpeed = rsh.mSpeed;
    mPingPong = rsh.mPingPong;
    mPause = rsh.mPause;
    mPathWaitTime = rsh.mPathWaitTime;
}

int Path::nextPoint() {
    int nextPt;
    if(mPingPong) {
        nextPt = mDirection ? mCurPt + 1 : mCurPt - 1;
        nextPt %= mPoints.size();
    } else {
        nextPt = (mCurPt + 1) % mPoints.size();
    }
    return nextPt;
}