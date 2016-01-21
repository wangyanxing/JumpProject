//
//  RendererActions.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/21/16.
//
//

#include "RendererActions.h"
#include "GameRenderer.h"

USING_NS_CC;

RendererMoveBy* RendererMoveBy::create(GameRenderer *renderer,
                                       float duration,
                                       const Vec2 &deltaPosition) {
  RendererMoveBy *ret = new (std::nothrow) RendererMoveBy();
  if (ret) {
    ret->_renderer = renderer;
    if (ret->initWithDuration(duration, deltaPosition)) {
      ret->autorelease();
    } else {
      CC_SAFE_DELETE(ret);
    }
  }
  return ret;
}

bool RendererMoveBy::initWithDuration(float duration, const Vec2& deltaPosition) {
  bool ret = false;
  if (ActionInterval::initWithDuration(duration)) {
    _positionDelta = deltaPosition;
    ret = true;
  }
  return ret;
}

RendererMoveBy* RendererMoveBy::clone() const {
  auto a = new (std::nothrow) RendererMoveBy();
  a->initWithDuration(_duration, _positionDelta);
  a->autorelease();
  return a;
}

void RendererMoveBy::startWithTarget(Node *target) {
  ActionInterval::startWithTarget(target);
  _previousPosition = _startPosition = _renderer->getPosition();
}

RendererMoveBy* RendererMoveBy::reverse() const {
  return RendererMoveBy::create(_renderer, _duration, -_positionDelta);
}

void RendererMoveBy::update(float t) {
  if (_target) {
#if CC_ENABLE_STACKABLE_ACTIONS
    Vec2 currentPos = _renderer->getPosition();
    Vec2 diff = currentPos - _previousPosition;
    _startPosition = _startPosition + diff;
    Vec2 newPos =  _startPosition + (_positionDelta * t);
    _renderer->setPosition(newPos);
    _previousPosition = newPos;
#else
    _renderer->setPosition(_startPosition + _positionDelta * t);
#endif // CC_ENABLE_STACKABLE_ACTIONS
  }
}

RendererMoveTo* RendererMoveTo::create(GameRenderer *renderer,
                                       float duration,
                                       const Vec2& position) {
  RendererMoveTo *ret = new (std::nothrow) RendererMoveTo();
  if (ret) {
    ret->_renderer = renderer;
    if (ret->initWithDuration(duration, position)) {
      ret->autorelease();
    } else {
      CC_SAFE_DELETE(ret);
    }
  }
  return ret;
}

bool RendererMoveTo::initWithDuration(float duration, const Vec2& position) {
  bool ret = false;
  if (ActionInterval::initWithDuration(duration)) {
    _endPosition = position;
    ret = true;
  }
  return ret;
}

RendererMoveTo* RendererMoveTo::clone() const {
  auto a = new (std::nothrow) RendererMoveTo();
  a->initWithDuration(_duration, _endPosition);
  a->autorelease();
  return a;
}

void RendererMoveTo::startWithTarget(Node *target) {
  RendererMoveBy::startWithTarget(target);
  _positionDelta = _endPosition - _renderer->getPosition();
}

RendererMoveTo* RendererMoveTo::reverse() const {
  CCASSERT(false, "reverse() not supported in MoveTo");
  return nullptr;
}
