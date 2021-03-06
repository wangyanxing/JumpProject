//
//  CameraShake.h
//  JumpEdt
//
//  Created by Yanxing Wang.
//
//

#ifndef __SHAKE_H__
#define __SHAKE_H__

class CameraShake : public cocos2d::ActionInterval {
public:
  CameraShake();

  // Create the action with a time and a strength (same in x and y)
  static CameraShake *create(float d, float strength);

  // Create the action with a time and strengths (different in x and y)
  static CameraShake *createWithStrength(float d, float strength_x, float strength_y);

  bool initWithDuration(float d, float strength_x, float strength_y);

  virtual ActionInterval *reverse() const override;

  virtual ActionInterval *clone() const override;

protected:
  void startWithTarget(cocos2d::Node *pTarget) override;

  void update(float time) override;

  void stop(void) override;

  cocos2d::Point m_StartPosition;

  // Strength of the action
  float m_strength_x{0};

  float m_strength_y{0};
};

#endif
