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
  RendererMoveBy() = default;

  virtual ~RendererMoveBy() = default;

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
  RendererMoveTo() = default;

  virtual ~RendererMoveTo() = default;

  bool initWithDuration(float duration, const cocos2d::Vec2& position);

protected:
  cocos2d::Vec2 _endPosition;

private:
  CC_DISALLOW_COPY_AND_ASSIGN(RendererMoveTo);
};

class OpenCloseDoor : public cocos2d::ActionInterval {
public:
  enum Operation {
    OPEN,
    CLOSE
  };
  static OpenCloseDoor* create(GameRenderer *renderer,
                               float duration,
                               Operation op,
                               bool reversedDirection);

  virtual OpenCloseDoor* clone() const override;

  virtual OpenCloseDoor* reverse(void) const override;

  virtual void startWithTarget(cocos2d::Node *target) override;

  virtual void update(float time) override;

protected:
  GameRenderer *_renderer{nullptr};

  bool _reversedDirection{false};

  cocos2d::Size _originalSize;

  Operation _operation;
};

#endif /* RendererActions_h */
