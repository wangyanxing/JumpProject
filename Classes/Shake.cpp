#include "Shake.h"

USING_NS_CC;

CameraShake::CameraShake() {
}

CameraShake *CameraShake::create(float d, float strength) {
  return createWithStrength(d, strength, strength);
}

CameraShake *CameraShake::createWithStrength(float duration, float strength_x, float strength_y) {
  CameraShake *pRet = new CameraShake();

  if (pRet && pRet->initWithDuration(duration, strength_x, strength_y)) {
    pRet->autorelease();
  } else {
    CC_SAFE_DELETE(pRet);
  }
  return pRet;
}

ActionInterval *CameraShake::reverse() const {
  return nullptr;
}

ActionInterval *CameraShake::clone() const {
  return nullptr;
}

bool CameraShake::initWithDuration(float duration, float strength_x, float strength_y) {
  if (ActionInterval::initWithDuration(duration)) {
    m_strength_x = strength_x;
    m_strength_y = strength_y;
    return true;
  }
  return false;
}

// Helper function. I included it here so that you can compile the whole file
// it returns a random value between min and max included
static float fgRangeRand(float min, float max) {
  float rnd = ((float) rand() / (float) RAND_MAX);
  return rnd * (max - min) + min;
}

void CameraShake::update(float dt) {
  float randx = fgRangeRand(-m_strength_x, m_strength_x) * dt;
  float randy = fgRangeRand(-m_strength_y, m_strength_y) * dt;

  // move the target to a shaked position
  _target->setPosition(m_StartPosition + Vec2(randx, randy));
}

void CameraShake::startWithTarget(Node *pTarget) {
  ActionInterval::startWithTarget(pTarget);

  // save the initial position
  m_StartPosition = pTarget->getPosition();
}

void CameraShake::stop(void) {
  // Action is done, reset clip position
  this->getTarget()->setPosition(m_StartPosition);

  ActionInterval::stop();
}
