//
//  UILayer.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/17/14.
//
//

#ifndef __JumpEdt__UILayer__
#define __JumpEdt__UILayer__

#include "Prerequisites.h"

class UILayer {
public:
  void init(cocos2d::Node *parent);

  void setFileName(const std::string &file);

  void addMessage(const char *message);

  void setEditModeName(const std::string &name);

  static UILayer *Layer;

public:
  cocos2d::Layer *mLayer{nullptr};

  cocos2d::Label *mFileNameLabel{nullptr};

  cocos2d::Label *mEditModeLebel{nullptr};
};

#endif /* defined(__JumpEdt__UILayer__) */
