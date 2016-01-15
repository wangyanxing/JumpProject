//
//  Blocks.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__Blocks__
#define __JumpEdt__Blocks__

#include "Prerequisites.h"
#include "Path.h"
#include "Rotator.h"
#include "SpriteUV.h"

class Button;
class BlockRenderer;

class BlockBase {
public:
  enum Status {
    IDLE,
    OPENING,
    CLOSING,
    OPENED,
    CLOSED
  };

  BlockBase();

  virtual ~BlockBase();

  virtual void setPosition(const cocos2d::Point &pt);

  void setPosition(float x, float y) {
    setPosition(cocos2d::Point(x, y));
  }

  void setPositionX(float x) {
    setPosition(cocos2d::Point(x, getPosition().y));
  }

  void setPositionY(float y) {
    setPosition(cocos2d::Point(getPosition().x, y));
  }

  void forceUpdatePhysicsPosition();

  void rotate();

  void normalizeUV();

  cocos2d::Point getPosition();

  virtual void addThickness(int val);

  virtual void subThickness(int val);

  virtual void addWidth(int val);

  virtual void subWidth(int val);

  void setWidth(float val);

  void setHeight(float val);

  virtual void setSize(cocos2d::Size size);

  virtual cocos2d::Size getSize();

  virtual void moveX(float val);

  virtual void moveY(float val);

  virtual void switchToSelectionImage();

  virtual void switchToNormalImage();

  virtual int getZOrder() {
    return mZOrder;
  }

  virtual float getWidth();

  virtual float getThickness();

  virtual void initPhysics();

  virtual void initShader();

  virtual void setVisible(bool val);

  virtual bool canPush();

  virtual bool pushable() {
    return mKind == KIND_PUSHABLE;
  }

  virtual int pushPriority();

  bool isVisible();

  virtual void setKind(BlockKind kind, bool forceSet = false);

  virtual cocos2d::Color3B getColor() {
    return mColor;
  }

  virtual void getPointsForShadow(const cocos2d::Vec2 &source, std::vector <cocos2d::Vec2> &out);

  BlockRenderer *getRenderer() {
    return mRenderer;
  }

  void setColor(int index);

  virtual void update(float dt);

  virtual void preUpdate();

  virtual void updateMovement(float dt);

  virtual void postUpdate(float dt);

  virtual void openDoor(float speed = 250, bool downDirDoor = true);

  virtual void closeDoor(float speed = 250, bool downDirDoor = true);

  virtual void reset();

  void callTriggerEvent();

  void callInitEvent();

  void updateOpenClose(float dt);

  void setRestoreRect(const cocos2d::Rect &rect);

  bool mCastShadow{true};

  int mShadowLayerID{0};

  float mShadowLength{DEFAULT_SHADOW_LENGTH};

  float mShadowFadeRatio{0.4};

  int mShadowFadeSegments{20};

  BlockKind mKind{KIND_MAX};

  int mZOrder{ZORDER_BLOCK};

  int mPaletteIndex{DEFAULT_PALETTE_INDEX};

  cocos2d::Color3B mColor{cocos2d::Color3B::WHITE};

  Path mPath;

  Rotator mRotator;

  float mImageSize{8};

  Button *mButton{nullptr};

  cocos2d::Vec2 mMovementThisFrame{0, 0};
  
  cocos2d::Vec2 mMovementToRestore{0, 0};

  cocos2d::Vec2 mUpSideMovement{0, 0};

  cocos2d::Vec2 mDownSideMovement{0, 0};

  cocos2d::Vec2 mLeftSideMovement{0, 0};

  cocos2d::Vec2 mRightSideMovement{0, 0};

  FollowMode mFollowMode{F_CENTER};

  bool mCanPush{true};

  bool mCanPickup{true};

  bool mCanDelete{true};
  
  bool mDisableMovement{false};
  
  bool mPreciseTrigger{false};

  int mID{0};

  cocos2d::Size mRestoreSize;

  cocos2d::Vec2 mRestorePosition;

  float mRestoreRotation{0};

  float mOpenCloseSpeed{100};

  int mRotationSpeed{DEFAULT_ROTATE_SPEED};

  bool mDownDirDoor{true};

  Status mStatus{IDLE};

  bool mUVFlipped{false};

  static int mIDCounter;

  std::string mUserData;

  std::vector <std::string> mTriggerEvents;

  std::vector <std::string> mInitialEvents;

  bool mTriggerEventsCalled{false};

  cocos2d::Vec2 mVelocity{0, 0};

  cocos2d::Vec2 mJumpVelocity{0, 0};

  bool mEnableGravity{false};

  int mShadowGroup{0};

#if EDITOR_MODE
  bool mShowIDLabel{false};

  void initIDLabel();

  void updateIDLabel();

  cocos2d::LabelAtlas *mIDLabel{nullptr};
#endif
  
  bool mOnPlatform{false};

protected:
  virtual void create(const cocos2d::Point &pt, const cocos2d::Size &size);

  virtual void addToScene(cocos2d::Node *parent);  

  BlockRenderer* mRenderer{nullptr};
};

#endif /* defined(__JumpEdt__Blocks__) */
