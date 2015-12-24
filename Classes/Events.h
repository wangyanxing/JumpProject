//
//  Events.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/18/14.
//
//

#ifndef __JumpEdt__Events__
#define __JumpEdt__Events__

class BlockBase;

class Events {
public:
  static void callEvent(const char *event, BlockBase *block);
};

#endif /* defined(__JumpEdt__Events__) */
