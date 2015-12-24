//
//  Hero.hpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/14/15.
//
//

#ifndef Hero_hpp
#define Hero_hpp

#include "Blocks.h"

class Hero : public BlockBase {
public:
  Hero() {
#if EDITOR_MODE
    mShowIDLabel = false;
#endif
  }

  virtual void initPhysics() override;

  virtual void initShader() override;

  virtual void updateMovement(float dt) override;

  virtual cocos2d::Color3B getColor() override { return cocos2d::Color3B::BLACK; }

  virtual int getZOrder() override { return 30; }

  virtual bool canPush() override { return true; }

  virtual void openDoor(float speed, bool downDirDoor) override { }

  virtual void closeDoor(float speed, bool downDirDoor) override { }

  virtual cocos2d::Size getSize() override { return mRestoreSize; }

  bool mCanJump{false};

  bool mPushing{false};

  int mLinkingID{-1};

  bool mPushLeftFlag{false};

  bool mPushRightFlag{false};

  float mCurrentMovingSpeed{0};
};

#include <stdio.h>

#endif /* Hero_hpp */
