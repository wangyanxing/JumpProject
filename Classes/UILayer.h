//
//  UILayer.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/17/14.
//
//

#ifndef __JumpEdt__UILayer__
#define __JumpEdt__UILayer__

#include "cocos2d.h"

class UILayer {
public:
  void init(cocos2d::Node *parent);

  void setFileName(const char *file);

  void addMessage(const char *message);

  static UILayer *Layer;

public:
  cocos2d::Layer *mLayer{nullptr};

  cocos2d::Label *mFileNameLabel{nullptr};
};

#endif /* defined(__JumpEdt__UILayer__) */
