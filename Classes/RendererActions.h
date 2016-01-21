//
//  RendererActions.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/21/16.
//
//

#ifndef RendererActions_h
#define RendererActions_h

#include "Prerequisites.h"

class RendererMoveBy : public cocos2d::ActionInterval {
public:
  static RendererMoveBy* create(GameRenderer *renderer,
                                float duration,
                                const cocos2d::Vec2& deltaPosition);

  virtual RendererMoveBy* clone() const override;

  virtual RendererMoveBy* reverse(void) const override;

  virtual void startWithTarget(cocos2d::Node *target) override;

  virtual void update(float time) override;

CC_CONSTRUCTOR_ACCESS:
  RendererMoveBy() {}

  virtual ~RendererMoveBy() {}

  bool initWithDuration(float duration, const cocos2d::Vec2& deltaPosition);

protected:
  cocos2d::Vec2 _positionDelta;

  cocos2d::Vec2 _startPosition;

  cocos2d::Vec2 _previousPosition;

  GameRenderer *_renderer{nullptr};

private:
  CC_DISALLOW_COPY_AND_ASSIGN(RendererMoveBy);
};

class RendererMoveTo : public RendererMoveBy {
public:
  static RendererMoveTo* create(GameRenderer *renderer,
                                float duration,
                                const cocos2d::Vec2& position);

  virtual RendererMoveTo* clone() const override;

  virtual RendererMoveTo* reverse() const  override;

  virtual void startWithTarget(cocos2d::Node *target) override;

CC_CONSTRUCTOR_ACCESS:
  RendererMoveTo() {}

  virtual ~RendererMoveTo() {}

  bool initWithDuration(float duration, const cocos2d::Vec2& position);

protected:
  cocos2d::Vec2 _endPosition;

private:
  CC_DISALLOW_COPY_AND_ASSIGN(RendererMoveTo);
};

#endif /* RendererActions_h */
