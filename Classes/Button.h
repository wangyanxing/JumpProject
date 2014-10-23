//
//  Button.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__Button__
#define __JumpEdt__Button__

#include "cocos2d.h"
#include "Defines.h"

class BlockBase;

class Button {
public:
    
    enum PushDir {
        DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT
    };
    
    Button(BlockBase* parent);
    
    ~Button();
    
    void setParentHeight(float v);
    
    void setParentWidth(float v);
    
    // call in case collsion detected
    bool push(const cocos2d::Vec2& normal, BlockBase* hero);
    
    void update(float dt);
    
    void rotateDir();
    
    void reset();
    
    bool mEnable{ true };
    
    bool mPushing{ false };
    
    bool mCanRestore{ true };
    
    void updatePosition();
    
    void callPushEvent();
    
    void callPushingEvent();
    
    void callRestoreEvent();
    
    PushDir mDir{ DIR_DOWN };
    
    BlockBase* mParent{ nullptr };
    
    std::string mPushedEvent;
    
    std::string mRestoredEvent;
    
    std::string mPushingEvent;
    
    bool mPushedEventCalled{ false };
    
    bool mRestoredEventCalled{ false };
    
#if EDITOR_MODE
    void updateHelper();
    void showHelper(bool val) {mHelperNode->setVisible(val);}
    cocos2d::DrawNode* mHelperNode{ nullptr };
#endif
};


#endif /* defined(__JumpEdt__Button__) */
