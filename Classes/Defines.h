//
//  Defines.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/18/14.
//
//

#ifndef __JumpEdt__Defines__
#define __JumpEdt__Defines__

#define EDITOR_MODE 1

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
