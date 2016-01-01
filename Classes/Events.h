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
  static void callEvents(std::vector<std::string>& events, BlockBase *caller);
  
  static void callSingleEvent(const char *event, BlockBase *caller);
};

#endif /* defined(__JumpEdt__Events__) */
