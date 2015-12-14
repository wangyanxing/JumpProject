#ifndef __SHAKE_H__
#define __SHAKE_H__

#include "cocos2d.h"

class CCShake : public cocos2d::ActionInterval
{
public:
  CCShake();

  // Create the action with a time and a strength (same in x and y)
  static CCShake *create(float d, float strength );
  // Create the action with a time and strengths (different in x and y)
  static CCShake *createWithStrength(float d, float strength_x, float strength_y );
  bool initWithDuration(float d, float strength_x, float strength_y );

  virtual ActionInterval* reverse() const override;
  virtual ActionInterval *clone() const override;

protected:

  void startWithTarget(cocos2d::Node *pTarget);
  void update(float time);
  void stop(void);

  cocos2d::Point m_StartPosition;


  // Strength of the action
  float m_strength_x, m_strength_y;
};

#endif