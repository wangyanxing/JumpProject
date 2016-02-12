//
//  RectDrawNode.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 2/12/16.
//
//

#include "RectDrawNode.h"

USING_NS_CC;

RectDrawNode* RectDrawNode::create(const cocos2d::Size &size, const cocos2d::Color3B& color) {
  RectDrawNode* ret = new (std::nothrow) RectDrawNode();
  if (ret && ret->init()) {
    ret->autorelease();
    ret->mSize = size;
    ret->setContentSize(size);
    ret->setColor(color);
  } else {
    CC_SAFE_DELETE(ret);
  }
  return ret;
}

void RectDrawNode::setSize(const cocos2d::Size& size) {
  mSize = size;
  setContentSize(size);
  redraw();
}

void RectDrawNode::setColor(const Color3B& color) {
  DrawNode::setColor(color);
  redraw();
}

void RectDrawNode::redraw() {
  drawSolidRect(Vec2(-mSize.width / 2, -mSize.height / 2),
                Vec2(mSize.width / 2, mSize.height / 2),
                Color4F(getColor()));
}
