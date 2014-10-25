//
//  Button.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "Button.h"
#include "GameScene.h"
#include "Events.h"

USING_NS_CC;

void Button::reset() {
    mPushing = false;
    mPushedEventCalled = false ;
    mRestoredEventCalled = false ;
    
    callRestoreEvent();
    
#if EDITOR_MODE
    updateHelper();
#endif
}

void Button::doPush() {
    mPushing = false;
    float newLength = 0;
    
    Vec2 newPos(0,0);
    if(mDir == DIR_LEFT) {
        newLength = std::min(mParent->getWidth() - mPushLength, mParent->mRestoreSize.width);
        setParentWidth(newLength);
        float leftBound = mParent->mRestorePosition.x - mParent->mRestoreSize.width/2;
        newPos.x = leftBound + newLength / 2 - mParent->mRestorePosition.x;
        
    } else if(mDir == DIR_RIGHT) {
        newLength = std::min(mParent->getWidth() - mPushLength, mParent->mRestoreSize.width);
        setParentWidth(newLength);
        float rightBound = mParent->mRestorePosition.x + mParent->mRestoreSize.width/2;
        newPos.x = rightBound - newLength / 2 - mParent->mRestorePosition.x;
        
    } else if(mDir == DIR_UP) {
        newLength = std::min(mParent->getThickness() - mPushLength, mParent->mRestoreSize.height);
        setParentHeight(newLength);
        float upBound = mParent->mRestorePosition.y + mParent->mRestoreSize.height/2;
        newPos.y = upBound - newLength / 2 - mParent->mRestorePosition.y;
        
    } else if(mDir == DIR_DOWN) {
        newLength = std::min(mParent->getThickness() - mPushLength, mParent->mRestoreSize.height);
        setParentHeight(newLength);
        float downBound = mParent->mRestorePosition.y - mParent->mRestoreSize.height/2;
        newPos.y = downBound + newLength / 2 - mParent->mRestorePosition.y;
    }
    
    mParent->mMovementToRestore += newPos;
    
    bool callpushing = false;
    if( mDir == DIR_LEFT || mDir == DIR_RIGHT ){
        callpushing = newLength < mParent->mRestoreSize.width / 2;
    } else if( mDir == DIR_UP || mDir == DIR_DOWN ) {
        callpushing = newLength < mParent->mRestoreSize.height / 2;
    }
    
    if( callpushing ) {
        callPushingEvent();
        
        mRestoredEventCalled = false;
        if(!mPushedEventCalled) {
            callPushEvent();
        }
    }
    
    mPushLength = 0;
    
    if( newLength < 1 ) {
        mEnable = false;
    }
}

bool Button::push(const cocos2d::Vec2& normal, BlockBase* hero) {
    if(!mEnable) return true;
    
    if(normal.y > 0.9 && mDir == DIR_DOWN) {
        mPushing = true;
    } else if(normal.y < -0.9 && mDir == DIR_UP) {
        mPushing = true;
    } else if(normal.x > 0.9  && mDir == DIR_LEFT) {
        mPushing = true;
    } else if(normal.x < -0.9 && mDir == DIR_RIGHT) {
        mPushing = true;
    }
    
    if(mPushing) {
        auto pos = mParent->getPosition();
        auto size = mParent->getSize();
        
        auto posHero = hero->getPosition();
        auto sizeHero = hero->getSize();
        
        float length = 0;
        
        if(mDir == DIR_UP) {
            length = (posHero.y + sizeHero.height/2) - (pos.y - size.height/2);
        }else if(mDir == DIR_DOWN){
            length = (pos.y + size.height/2) - (posHero.y - sizeHero.height/2);
        }else if(mDir == DIR_LEFT){
            length = (pos.x + size.width/2) - (posHero.x - sizeHero.width/2);
        }else if(mDir == DIR_RIGHT){
            length = (posHero.x + sizeHero.width/2) - (pos.x - size.width/2);
        }
        
        if(length < 0) {
            mPushing = false;
            return mDir == DIR_UP ? true : false;
        }
        
        // will do push later
        mPushLength = length;
        
        return true;
    } else {
        return mDir == DIR_UP ? true : false;
    }
}

void Button::rotateDir() {
    static PushDir dirs[] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    auto id = (int)mDir;
    mDir = dirs[++id % 4];
    updateHelper();
}

void Button::callPushEvent() {
    mPushedEventCalled = true;
    if(mPushedEvent.empty()) return;
    
    Events::callEvent(mPushedEvent.c_str(), (BlockBase*)this);
}

void Button::callRestoreEvent() {
    mRestoredEventCalled = true;
    if(mRestoredEvent.empty()) return;
    
    Events::callEvent(mRestoredEvent.c_str(), (BlockBase*)this);
}

void Button::callPushingEvent() {
    if(mPushingEvent.empty()) return;
    
    Events::callEvent(mPushingEvent.c_str(), (BlockBase*)this);
}

void Button::update(float dt) {
    if (mPushing) {
        doPush();
        return;
    }
    if(!mPushing && mCanRestore) {
        // restore
        auto size = mParent->getSize();
        if(mDir == DIR_LEFT || mDir == DIR_RIGHT) {
            if(size.width >= mParent->mRestoreSize.width) {
                return;
            }
        } if(mDir == DIR_UP || mDir == DIR_DOWN) {
            if(size.height >= mParent->mRestoreSize.height) {
                return;
            }
        }
        
        mParent->getSprite()->getPhysicsBody()->setEnable(true);
        
        Vec2 newPos(0,0);
        float length = dt * 100;
        float newLength = 0;
        if(mDir == DIR_LEFT) {
            newLength = std::min(mParent->getWidth() + length, mParent->mRestoreSize.width);
            setParentWidth(newLength);
            float leftBound = mParent->mRestorePosition.x - mParent->mRestoreSize.width/2;
            newPos.x = leftBound + newLength / 2 - mParent->mRestorePosition.x;

        } else if(mDir == DIR_RIGHT) {
            newLength = std::min(mParent->getWidth() + length, mParent->mRestoreSize.width);
            setParentWidth(newLength);
            float rightBound = mParent->mRestorePosition.x + mParent->mRestoreSize.width/2;
            newPos.x = rightBound - newLength / 2 - mParent->mRestorePosition.x;

        } else if(mDir == DIR_UP) {
            newLength = std::min(mParent->getThickness() + length, mParent->mRestoreSize.height);
            setParentHeight(newLength);
            float upBound = mParent->mRestorePosition.y + mParent->mRestoreSize.height/2;
            newPos.y = upBound - newLength / 2 - mParent->mRestorePosition.y;

        } else if(mDir == DIR_DOWN) {
            newLength = std::min(mParent->getThickness() + length, mParent->mRestoreSize.height);
            setParentHeight(newLength);
            float downBound = mParent->mRestorePosition.y - mParent->mRestoreSize.height/2;
            newPos.y = downBound + newLength / 2 - mParent->mRestorePosition.y;
        }
        
        mParent->mMovementToRestore += newPos;
        
        bool callrestore = false;
        if( mDir == DIR_LEFT || mDir == DIR_RIGHT ){
            callrestore = newLength >= mParent->mRestoreSize.width / 2;
        } else if( mDir == DIR_UP || mDir == DIR_DOWN ) {
            callrestore = newLength >= mParent->mRestoreSize.height / 2;
        }
        
        if( callrestore ) {
            mPushedEventCalled = false;
            if(!mRestoredEventCalled) {
                callRestoreEvent();
            }
        }
        
        if(newLength > 0.05) {
            mEnable = true;
        }
    } else {
        mPushing = false;
    }
}

void Button::setParentWidth(float v) {
    if(mParent->getSprite()->getRotation() > 1)
        mParent->setHeight(v);
    else
        mParent->setWidth(v);
}

void Button::setParentHeight(float v) {
    if(mParent->getSprite()->getRotation() > 1)
        mParent->setWidth(v);
    else
        mParent->setHeight(v);
}

void Button::updatePosition() {
#if EDITOR_MODE
    auto p = mParent->getPosition();
    mHelperNode->setPosition(p);
#endif
}

Button::Button(BlockBase* parent) : mParent(parent) {
    //mRestoreSize = mParent->getBoundingBox().size;
    
#if EDITOR_MODE
    mHelperNode = DrawNode::create();
    mParent->getSprite()->getParent()->addChild(mHelperNode,1000);
    updateHelper();
#endif
}

Button::~Button() {
#if EDITOR_MODE
    mHelperNode->removeFromParentAndCleanup(true);
#endif
}

#if EDITOR_MODE
void Button::updateHelper() {
    Color4F color(123/255.,221/255.,45/255.,1.0);
    auto p = mParent->getPosition();
    auto size = mParent->getSize();
    float arrawH = 20;
    float arrawW = 5;
    mHelperNode->clear();
    if(mDir == DIR_UP) {
        mHelperNode->drawTriangle(Vec2(0, -size.height/2),
                                  Vec2(-arrawW, -size.height/2 - arrawH),
                                  Vec2( arrawW, -size.height/2 - arrawH),
                                  color);
    } else if(mDir == DIR_DOWN) {
        mHelperNode->drawTriangle(Vec2(0, size.height/2),
                                  Vec2(-arrawW, size.height/2 + arrawH),
                                  Vec2( arrawW, size.height/2 + arrawH),
                                  color);
    } else if(mDir == DIR_RIGHT) {
        mHelperNode->drawTriangle(Vec2(-size.width/2, 0),
                                  Vec2(-size.width/2 - arrawH, -arrawW),
                                  Vec2(-size.width/2 - arrawH,  arrawW),
                                  color);
    } else if(mDir == DIR_LEFT) {
        mHelperNode->drawTriangle(Vec2(size.width/2, 0),
                                  Vec2(size.width/2 + arrawH, -arrawW),
                                  Vec2(size.width/2 + arrawH,  arrawW),
                                  color);
    }
    mHelperNode->setPosition(p);
}
#endif
