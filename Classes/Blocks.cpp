//
//  Blocks.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "Blocks.h"
#include "Hero.h"
#include "GameUtils.h"
#include "Palette.h"
#include "Button.h"
#include "LogicManager.h"
#include "EditorScene.h"
#include "GameScene.h"
#include "VisibleRect.h"
#include "Events.h"
#include "BlockRenderer.h"
#include "RectRenderer.h"
#include "DeathCircleRenderer.h"

USING_NS_CC;

int BlockBase::mIDCounter = 0;

BlockBase::BlockBase() {
  mID = mIDCounter++;
  mColor = Color3B::WHITE;
}

BlockBase::~BlockBase() {
#if EDITOR_MODE
  mIDLabel->removeFromParent();
#endif
  CC_SAFE_DELETE(mRenderer);
  CC_SAFE_DELETE(mButton);
}

void BlockBase::create(const cocos2d::Point &pt, const cocos2d::Size &size) {
  CC_ASSERT(mKind != KIND_MAX);
  CC_SAFE_DELETE(mRenderer);

  Rect r(pt, size);
  BlockRenderer::InitParams param = {
    {BlockRenderer::PARAM_RECT, Any(r)},
    {BlockRenderer::PARAM_COLOR, Any(getColor())}
  };

  if (mKind == KIND_DEATH_CIRCLE) {
    mRenderer = new DeathCircleRenderer(this);
  } else {
    mRenderer = new RectRenderer(this);
  }
  mRenderer->init(param);

  setPosition(pt);

#if EDITOR_MODE
  initIDLabel();
#endif

  mRestoreSize = size;
  mRestorePosition = getPosition();

  initShader();
  initPhysics();

  addToScene(GAME_LAYER->getBlockRoot());
}

void BlockBase::initShader() {
}

void BlockBase::setPosition(const cocos2d::Point &pt) {
  mRenderer->setPosition(pt);
  if (mButton) {
    mButton->updatePosition();
  }
}

void BlockBase::normalizeUV() {
  if (!mRenderer || mRenderer->getType() != BlockRenderer::TYPE_RECT) {
    return;
  }
  CC_ASSERT(dynamic_cast<RectRenderer*>(mRenderer) != nullptr);
  auto rectRenderer = static_cast<RectRenderer*>(mRenderer);
  rectRenderer->normalizeUV();
}

void BlockBase::rotate() {
  auto w = getWidth();
  auto h = getThickness();
  setWidth(h);
  setHeight(w);

  mRestoreSize = Size(h, w);
  mRestorePosition = getPosition();

  normalizeUV();
}

bool BlockBase::canPush() {
  return false;
}

int BlockBase::pushPriority() {
  return mPath.empty() ? 0 : 1;
}

void BlockBase::openDoor(float speed, bool downDirDoor) {
  mDownDirDoor = downDirDoor;
  mOpenCloseSpeed = speed;
  mStatus = OPENING;
}

void BlockBase::closeDoor(float speed, bool downDirDoor) {
  mDownDirDoor = downDirDoor;
  mOpenCloseSpeed = speed;
  mStatus = CLOSING;
}

void BlockBase::reset() {
  mStatus = IDLE;

  mPath.mPause = mPath.mOriginalPause;
  mVelocity.set(0, 0);
  mJumpVelocity.set(0, 0);

  if (mKind != KIND_DEATH_CIRCLE) {
    setSize(mRestoreSize);
  }

  setPosition(mRestorePosition);

  if (mButton) {
    mButton->reset();
  }

  mRenderer->setRotation(0);
  mRenderer->setOpacity(255);
  mRenderer->getNode()->stopAllActions();

  callInitEvent();
}

void BlockBase::updateOpenClose(float dt) {
  float minLength = 1;
  if (mStatus == CLOSING) {
    if (mDownDirDoor) {
      float upperbound = mRestorePosition.y + mRestoreSize.height / 2;
      float curHeight = getThickness() + dt * mOpenCloseSpeed;
      if (curHeight >= mRestoreSize.height) {
        curHeight = mRestoreSize.height;
        mStatus = CLOSED;
      }
      setHeight(curHeight);
      mMovementToRestore.y += upperbound - curHeight / 2 - mRestorePosition.y;
    } else {
      float lowerbound = mRestorePosition.y - mRestoreSize.height / 2;
      float curHeight = getThickness() + dt * mOpenCloseSpeed;
      if (curHeight >= mRestoreSize.height) {
        curHeight = mRestoreSize.height;
        mStatus = CLOSED;
      }
      setHeight(curHeight);
      mMovementToRestore.y += lowerbound + curHeight / 2 - mRestorePosition.y;
    }
  } else if (mStatus == OPENING) {
    if (mDownDirDoor) {
      float upperbound = mRestorePosition.y + mRestoreSize.height / 2;
      float curHeight = getThickness() - dt * mOpenCloseSpeed;
      if (curHeight <= minLength) {
        curHeight = minLength;
        mStatus = OPENED;
      }
      setHeight(curHeight);
      mMovementToRestore.y += upperbound - curHeight / 2 - mRestorePosition.y;
    } else {
      float lowerbound = mRestorePosition.y - mRestoreSize.height / 2;
      float curHeight = getThickness() - dt * mOpenCloseSpeed;
      if (curHeight <= minLength) {
        curHeight = minLength;
        mStatus = OPENED;
      }
      setHeight(curHeight);
      mMovementToRestore.y += lowerbound + curHeight / 2 - mRestorePosition.y;
    }
  } else if (mStatus == OPENED) {
    setHeight(minLength);
    float upperbound = mRestorePosition.y + mRestoreSize.height / 2;
    float lowerbound = mRestorePosition.y - mRestoreSize.height / 2;
    if (mDownDirDoor) {
      mMovementToRestore.y += upperbound - minLength / 2 - mRestorePosition.y;
    } else {
      mMovementToRestore.y += lowerbound + minLength / 2 - mRestorePosition.y;
    }
  }
}

void BlockBase::postUpdate(float dt) {
  if (mKind == KIND_PUSHABLE || mKind == KIND_HERO) {
    getRenderer()->getPhysicsBody()->setVelocityLimit(1000);
  }
}

void BlockBase::updateMovement(float dt) {
  if (mDisableMovement) {
    return;
  }
  auto lastpos = mRenderer->getPosition();
  auto newpos = mKind == KIND_PUSHABLE ? lastpos : mRestorePosition + mMovementToRestore;

  newpos.y += mVelocity.y * dt;
  newpos.x += mVelocity.x * dt;
  
  mRenderer->setPosition(newpos);
  mMovementThisFrame = mRenderer->getPosition() - lastpos;
}

void BlockBase::preUpdate() {
  mMovementToRestore.setZero();
  mUpSideMovement.setZero();
  mDownSideMovement.setZero();
  mRightSideMovement.setZero();
  mLeftSideMovement.setZero();
}

void BlockBase::update(float dt) {
  // Update velocity
  if (mEnableGravity) {
    float gy = GRAVITY_VAL;
    mVelocity.y += dt * gy;
    mVelocity.y = std::min(mVelocity.y, 1000.0f);
    mVelocity.x = std::min(mVelocity.x, 600.0f);
  }

  if (!mRotator.empty()) {
    auto rot = mRestoreRotation;
    auto size = mRestoreSize;
    float newRot = rot;
    Vec2 outsize(1, 1);
    mRotator.update(dt, newRot, outsize);
    mRenderer->setRotation(newRot);
  }

  if (!mPath.empty()) {
    auto pos = mRestorePosition;
    auto newPos = pos;
    auto size = mRestoreSize;

    Vec2 outsize(1, 1);
    mPath.update(dt, newPos, outsize);

    mRenderer->setScale(outsize.x * mRestoreSize.width / mImageSize,
                      outsize.y * mRestoreSize.height / mImageSize);

    auto newSize = getSize();

    mMovementToRestore += newPos - pos;
    mUpSideMovement = (newPos + Vec2(0, newSize.height / 2)) - (pos + Vec2(0, size.height / 2));
    mDownSideMovement = (newPos + Vec2(0, -newSize.height / 2)) - (pos + Vec2(0, -size.height / 2));
    mRightSideMovement = (newPos + Vec2(newSize.width / 2, 0)) - (pos + Vec2(size.width / 2, 0));
    mLeftSideMovement = (newPos + Vec2(-newSize.width / 2, 0)) - (pos + Vec2(-size.width / 2, 0));

    auto it = GameLogic::Game->mGroups.find(this);
    if (it != GameLogic::Game->mGroups.end()) {
      for (auto &c : it->second) {
        switch (mFollowMode) {
          case F_CENTER:
            c->mMovementToRestore += mMovementToRestore;
            break;
          case F_LEFT:
            c->mMovementToRestore += mLeftSideMovement;
            break;
          case F_RIGHT:
            c->mMovementToRestore += mRightSideMovement;
            break;
          case F_DOWN:
            c->mMovementToRestore += mDownSideMovement;
            break;
          case F_UP:
            c->mMovementToRestore += mUpSideMovement;
            break;
        }
      }
    }
  }

  if (mButton) {
    mButton->update(dt);
  } else {
    updateOpenClose(dt);
  }

  if (mRotationSpeed > 0) {
    auto rotation = mRenderer->getRotation();
    rotation += mRotationSpeed * dt;
    if (rotation > 360) {
      rotation -= 360;
    }
    mRenderer->setRotation(rotation);
  }

  // Event continue time
  if (mTriggerEventsCalled) {
    mTriggerEventsCalled = false;
  }

#if EDITOR_MODE
  if (mIDLabel) {
    if (mShowIDLabel) {
      mIDLabel->setVisible(true);
      mIDLabel->setPosition(mRenderer->getPosition() -
                            Vec2(mIDLabel->getBoundingBox().size.width / 2,
                                 mIDLabel->getBoundingBox().size.height / 2));
    } else {
      mIDLabel->setVisible(false);
    }
  }
#endif
}

void BlockBase::setVisible(bool val) {
  mRenderer->setVisible(val);
}

#if EDITOR_MODE
void BlockBase::initIDLabel() {
  char buffer[10];
  sprintf(buffer, "%d", mID);
  mIDLabel = LabelAtlas::create(buffer, "images/numbers.png", 37, 60, '0');
  mIDLabel->setScale(0.3);
  auto size = mIDLabel->getBoundingBox().size;
  GAME_LAYER->getBlockRoot()->addChild(mIDLabel, ZORDER_EDT_ID_LABEL);
  mIDLabel->setPosition(mRenderer->getPosition() - Vec2(size.width / 2, size.height / 2));
  mShowIDLabel = EditorScene::Scene->mShowGrid;
  mIDLabel->setVisible(mShowIDLabel);
  mIDLabel->setCameraMask((unsigned short) CameraFlag::USER2);
}

void BlockBase::updateIDLabel() {
  char buffer[10];
  sprintf(buffer, "%d", mID);
  mIDLabel->setString(buffer);
}

#endif

void BlockBase::initPhysics() {
  mRenderer->removePhysicsBody();

  PhysicsBody *pbody = nullptr;
  auto size = mRenderer->getContentSize();
  if (mKind == KIND_DEATH_CIRCLE) {
    float diagonal = std::max(size.height / 2, size.width / 2);
    diagonal = sqrt(diagonal * diagonal + diagonal * diagonal);
    pbody = PhysicsBody::createCircle(diagonal);
  } else {
    pbody = PhysicsBody::createBox(size);
  }
  pbody->setContactTestBitmask(1);
  pbody->setGroup(1);
  pbody->setGravityEnable(false);

  if (mKind == KIND_PUSHABLE) {
    mEnableGravity = true;
    pbody->setDynamic(true);
    pbody->setRotationEnable(false);
    pbody->setMass(40);
  } else {
    pbody->setDynamic(false);
  }
  mRenderer->setPhysicsBody(pbody);
}

void BlockBase::setKind(BlockKind kind, bool forceSet) {
  if (!forceSet && kind == mKind) {
    return;
  }

  static int kindZOrder[KIND_MAX] = {
      25,
      20,
      15,
      15,
      15,
      20
  };

  static bool castShadow[KIND_MAX] = {
      true,
      true,
      false,
      false,
      false,
      true
  };

  if (mPaletteIndex == -1) {
    mColor = Palette::getInstance()->getDefaultBlockColors(kind);
  } else {
    setColor(mPaletteIndex);
  }

  mKind = kind;
  mCastShadow = castShadow[kind];

  create(mRestorePosition, mRestoreSize);

  mRenderer->setZOrder(kindZOrder[kind]);
  mRenderer->setColor(mColor);

  if (kind == KIND_BUTTON) {
    mButton = new Button(this);
  } else {
    CC_SAFE_DELETE(mButton);
  }

  initPhysics();

  mRenderer->setRotation(0);

  if ((kind == KIND_DEATH || kind == KIND_DEATH_CIRCLE) && mTriggerEvents.empty()) {
    mTriggerEvents.push_back("die");
  }

  if (kind == KIND_DEATH_CIRCLE) {
    auto s = Size(mRenderer->getScaleX() * mImageSize, mRenderer->getScaleY() * mImageSize);
    auto size = std::max(s.width, s.height);
    mRotationSpeed = ROTATION_SPEED;

    // Update image
    mImageSize = ORG_RECT_SIZE;
    mRenderer->setTexture(BLOCK_IMAGE);

    setWidth(size);
    setHeight(size);
    mRestoreSize = Size(size, size);
  } else if (kind == KIND_DEATH) {
    if (mTriggerEvents.size() == 1 && mTriggerEvents.at(0) == "die") {
      mRenderer->TextureName = DEATH_IMAGE;
    }
    mRenderer->setTexture(mRenderer->TextureName);
    normalizeUV();
  } else {
    mImageSize = ORG_RECT_SIZE;
    mRotationSpeed = 0;

    mRenderer->TextureName = BLOCK_IMAGE;
    mRenderer->setTexture(mRenderer->TextureName);

    setSize(Size(mRenderer->getScaleX() * mImageSize, mRenderer->getScaleY() * mImageSize));
  }
}

void BlockBase::setColor(int index) {
  if (index == -1) {
    mColor = Palette::getInstance()->getDefaultBlockColors(mKind);
  } else {
    mPaletteIndex = index;
    Color3B color = Palette::getInstance()->getColorFromPalette(index);
    mColor = color;
  }
  getRenderer()->setColor(mColor);
}

void BlockBase::moveX(float val) {
  setPositionX(val + mRenderer->getPosition().x);
  if (!mPath.empty()) {
    mPath.translatePoints(Vec2(val, 0));
#if EDITOR_MODE
    mPath.updateHelper();
#endif
  }
  mRestorePosition.x += val;
}

void BlockBase::moveY(float val) {
  setPositionY(val + mRenderer->getPosition().y);
  if (!mPath.empty()) {
    mPath.translatePoints(Vec2(0, val));
#if EDITOR_MODE
    mPath.updateHelper();
#endif
  }
  mRestorePosition.y += val;
}

void BlockBase::addThickness(int val) {
  auto t = getThickness() + val;
  t = std::min<int>(t, VisibleRect::top().y * 1.3);

  if (mKind == KIND_DEATH_CIRCLE) {
    mRenderer->setScale(t / mImageSize);
  } else {
    mRenderer->setScale(mRenderer->getScaleX(), t / mImageSize);
  }

  mRestoreSize = Size(mRenderer->getScaleX() * mImageSize,
                      mRenderer->getScaleY() * mImageSize);
  normalizeUV();
}

void BlockBase::subThickness(int val) {
  auto t = getThickness() - val;
  t = std::max<int>(t, 5);

  if (mKind == KIND_DEATH_CIRCLE) {
    mRenderer->setScale(t / mImageSize);
  } else {
    mRenderer->setScale(mRenderer->getScaleX(), t / mImageSize);
  }

  mRestoreSize = Size(mRenderer->getScaleX() * mImageSize,
                      mRenderer->getScaleY() * mImageSize);
  normalizeUV();
}

void BlockBase::addWidth(int val) {
  auto w = getWidth() + val;
  w = std::min<int>(w, VisibleRect::right().x * 1.3);

  if (mKind == KIND_DEATH_CIRCLE) {
    mRenderer->setScale(w / mImageSize);
  } else {
    mRenderer->setScale(w / mImageSize, getRenderer()->getScaleY());
  }

  mRestoreSize = Size(mRenderer->getScaleX() * mImageSize,
                      mRenderer->getScaleY() * mImageSize);

  normalizeUV();
}

void BlockBase::subWidth(int val) {
  auto w = getWidth() - val;
  w = std::max<int>(w, 5);

  if (mKind == KIND_DEATH_CIRCLE) {
    mRenderer->setScale(w / mImageSize);
  } else {
    mRenderer->setScale(w / mImageSize, mRenderer->getScaleY());
  }
  mRestoreSize = Size(mRenderer->getScaleX() * mImageSize,
                      mRenderer->getScaleY() * mImageSize);
  normalizeUV();
}

void BlockBase::setWidth(float val) {
  if (mKind == KIND_DEATH_CIRCLE) {
    mRenderer->setScale(val / mImageSize);
  } else {
    mRenderer->setScale(val / mImageSize, mRenderer->getScaleY());
  }
  normalizeUV();
}

void BlockBase::setHeight(float val) {
  if (mKind == KIND_DEATH_CIRCLE) {
    mRenderer->setScale(val / mImageSize);
  } else {
    mRenderer->setScale(mRenderer->getScaleX(), val / mImageSize);
  }
  normalizeUV();
}

float BlockBase::getWidth() {
  auto scaleX = mRenderer->getScaleX();
  return scaleX * mImageSize;
}

float BlockBase::getThickness() {
  auto scaleY = mRenderer->getScaleY();
  return scaleY * mImageSize;
}

void BlockBase::setSize(Size size) {
  mRenderer->setScale(size.width / mImageSize, size.height / mImageSize);
  mRestoreSize = size;
  normalizeUV();
}

cocos2d::Size BlockBase::getSize() {
  return Size(mRenderer->getScaleX() * mImageSize, mRenderer->getScaleY() * mImageSize);
}

void BlockBase::switchToNormalImage() {
  mRenderer->setColor(getColor());
}

void BlockBase::switchToSelectionImage() {
  mRenderer->setColor(Color3B(0, 0, 255));
}

void BlockBase::addToScene(cocos2d::Node *parent) {
  mRenderer->addToParent(parent, mZOrder);
}

void BlockBase::getPointsForShadow(const cocos2d::Vec2 &source,
                                   std::vector<cocos2d::Vec2> &out) {
  auto size = mRenderer->getBoundingBox().size;
  auto p = mRenderer->getPosition();
  out.resize(4);
  out[0] = p + Vec2(-size.width / 2, size.height / 2);
  out[2] = p + Vec2(-size.width / 2, -size.height / 2);
  out[1] = p + Vec2(size.width / 2, size.height / 2);
  out[3] = p + Vec2(size.width / 2, -size.height / 2);
}

void BlockBase::callTriggerEvent() {
  if (mTriggerEvents.empty()) {
    return;
  }
  mTriggerEventsCalled = true;
  for (size_t i = 0; i < mTriggerEvents.size(); ++i) {
    Events::callSingleEvent(mTriggerEvents[i].c_str(), this);
  }
}

void BlockBase::callInitEvent() {
  if (mInitialEvents.empty()) {
    return;
  }
  for (size_t i = 0; i < mInitialEvents.size(); ++i) {
    Events::callSingleEvent(mInitialEvents[i].c_str(), this);
  }
}

void BlockBase::forceUpdatePhysicsPosition() {
  auto body = mRenderer->getPhysicsBody();
  if (!body) {
    return;
  }

  Vec2 pos = mRenderer->getPosition();
  auto node = mRenderer->getNode();
  auto scene = node->getScene();
  if (scene && scene->getPhysicsWorld()) {
    pos = node->getParent() == scene ? mRenderer->getPosition() :
          scene->convertToNodeSpace(node->getParent()->convertToWorldSpace(getPosition()));
  }

  body->setPosition(pos.x, pos.y);
}

cocos2d::Point BlockBase::getPosition() {
  return mRenderer->getPosition();
}

bool BlockBase::isVisible() {
  return mRenderer->isVisible();
}

void BlockBase::setRestoreRect(const cocos2d::Rect &rect) {
  mRestorePosition = rect.origin;
  mRestoreSize = rect.size;
}
