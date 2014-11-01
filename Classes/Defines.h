//
//  Defines.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/18/14.
//
//

#ifndef __JumpEdt__Defines__
#define __JumpEdt__Defines__

#include "cocos2d.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
#   define EDITOR_MODE 1
#else
#   define EDITOR_MODE 0
#endif

#define EDITOR_IPAD_MODE 1
#define EDITOR_IP5_MODE  2
#define EDITOR_IP4_MODE  3

// change at here
#define EDITOR_RATIO EDITOR_IP5_MODE

#define ORG_RECT_SIZE 8.0

enum BlockKind {
    KIND_HERO = 0,
    KIND_BLOCK,
    KIND_DEATH,
    KIND_DEATH_CIRCLE,
    KIND_BUTTON,
    KIND_PUSHABLE,
    
    KIND_MAX
};

enum FollowMode {
    F_CENTER,
    F_UP, F_DOWN, F_LEFT, F_RIGHT
};


#endif /* defined(__JumpEdt__Defines__) */
