//
//  WaveEffect.h
//  JumpEdt
//
//  Created by Yanxing Wang on 11/2/14.
//
//

#ifndef __JumpEdt__WaveEffect__
#define __JumpEdt__WaveEffect__

#include "DrawNodeEx.h"

class WaveEffect {
public:
  struct WaveLine {
    cocos2d::Color4B color{255,255,255,255};
    float length{ 20 };
    float height{ 25 };
    float yBias{ -25 };
    float seg{ 5 };
    float angleBias{ 0 };
    float speed{ 1 };
    float phase{ 1 };

    cocos2d::DrawNodeEx* node{ nullptr };
  };

  WaveEffect(cocos2d::Node* parent);

  virtual ~WaveEffect();

  void update(float dt);

  WaveLine& newLine();

private:

  void updateLine(WaveLine& line, float dt);

  void init(cocos2d::Node* parent);

  void destory();

  std::vector<WaveLine> mLines;

  cocos2d::Node* mParentNode{ nullptr };
};

#endif /* defined(__JumpEdt__WaveEffect__) */
