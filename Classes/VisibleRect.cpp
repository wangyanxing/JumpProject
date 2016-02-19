//
//  VisibleRect.cpp
//  JumpEdt
//
//  Created by Yanxing Wang.
//
//

#include "VisibleRect.h"
#include "Defines.h"

USING_NS_CC;

Rect VisibleRect::s_visibleRect;

void VisibleRect::lazyInit() {
#if EDITOR_MODE
  s_visibleRect.origin = Vec2(0, 0);

#   if EDITOR_RATIO == EDITOR_IP5_MODE
  float ratio = 1.7778f;
#   elif EDITOR_RATIO == EDITOR_IP4_MODE
  float ratio = 1.5f;
#   elif EDITOR_RATIO == EDITOR_IPAD_MODE
  float ratio = 1.3333f;
#   endif
  float fixedWidth = 960;
  s_visibleRect.size = Size(fixedWidth, fixedWidth / ratio);
#else
  s_visibleRect = Director::getInstance()->getOpenGLView()->getVisibleRect();
#endif
}

Size VisibleRect::getFrameSize() {
  return Director::getInstance()->getOpenGLView()->getFrameSize();
}

Rect VisibleRect::getVisibleRect() {
  lazyInit();
  return s_visibleRect;
}

Vec2 VisibleRect::left() {
  lazyInit();
  return Vec2(s_visibleRect.origin.x, s_visibleRect.origin.y + s_visibleRect.size.height / 2);
}

Vec2 VisibleRect::right() {
  lazyInit();
  return Vec2(s_visibleRect.origin.x + s_visibleRect.size.width,
              s_visibleRect.origin.y + s_visibleRect.size.height / 2);
}

Vec2 VisibleRect::top() {
  lazyInit();
  return Vec2(s_visibleRect.origin.x + s_visibleRect.size.width / 2,
              s_visibleRect.origin.y + s_visibleRect.size.height);
}

Vec2 VisibleRect::bottom() {
  lazyInit();
  return Vec2(s_visibleRect.origin.x + s_visibleRect.size.width / 2, s_visibleRect.origin.y);
}

Vec2 VisibleRect::center() {
  lazyInit();
  return Vec2(s_visibleRect.origin.x + s_visibleRect.size.width / 2,
              s_visibleRect.origin.y + s_visibleRect.size.height / 2);
}

Vec2 VisibleRect::leftTop() {
  lazyInit();
  return Vec2(s_visibleRect.origin.x, s_visibleRect.origin.y + s_visibleRect.size.height);
}

Vec2 VisibleRect::rightTop() {
  lazyInit();
  return Vec2(s_visibleRect.origin.x + s_visibleRect.size.width,
              s_visibleRect.origin.y + s_visibleRect.size.height);
}

Vec2 VisibleRect::leftBottom() {
  lazyInit();
  return s_visibleRect.origin;
}

Vec2 VisibleRect::rightBottom() {
  lazyInit();
  return Vec2(s_visibleRect.origin.x + s_visibleRect.size.width, s_visibleRect.origin.y);
}
