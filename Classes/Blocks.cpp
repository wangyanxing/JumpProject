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
#include "VisibleRect.h"
#include "Events.h"

USING_NS_CC;

int BlockBase::mIDCounter = 0;

BlockBase::BlockBase() {
  mID = mIDCounter++;
  mColor = Color3B::WHITE;
}

BlockBase::~BlockBase() {
#if EDITOR_MODE
  mSprite->removeFromParent();
  mIDLabel->removeFromParent();
#endif
  if (mButton) {
    delete mButton;
    mButton = nullptr;
  }
}

void BlockBase::create(const cocos2d::Point &pt) {
  auto thick = 30;
  auto width = 200;
  Rect r;
  r.origin = Point::ZERO;
  r.size.width = width / 2;
  r.size.height = thick / 2;
  mSprite = GameUtils::createRect(r, getColor());
  mSprite->setPosition(pt);

#if EDITOR_MODE
  initIDLabel();
#endif

  mRestoreSize = r.size;
  mRestorePosition = getPosition();

  initShader();
  initPhysics();
}

void BlockBase::create(const cocos2d::Rect &rect) {
  mSprite = GameUtils::createRect(rect, getColor());

#if EDITOR_MODE
  initIDLabel();
#endif
  mRestoreSize = rect.size;
  mRestorePosition = getPosition();

  initShader();
  initPhysics();
}

void BlockBase::create(const cocos2d::Point &pt, const cocos2d::Size &size) {
  Rect r(pt, size);
  mSprite = GameUtils::createRect(r, getColor());
  setPosition(pt);

#if EDITOR_MODE
  initIDLabel();
#endif

  mRestoreSize = size;
  mRestorePosition = getPosition();

  initShader();
  initPhysics();
}

void BlockBase::initShader() {
#if EDITOR_MODE == 0
  // Load shader
  auto shaderfile = FileUtils::getInstance()->fullPathForFilename("shaders/normal_vig.fsh");
  auto shaderContent = FileUtils::getInstance()->getStringFromFile(shaderfile);
  auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert,
                                                 shaderContent.c_str());
  auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);

  float screenWidth = VisibleRect::getFrameSize().width;
  float screenHeight = VisibleRect::getFrameSize().height;

  mSprite->setGLProgramState(glProgramState);
  glProgramState->setUniformVec3("resolution", Vec3(screenWidth, screenHeight, 1.3));
#endif
}

void BlockBase::setPosition(const cocos2d::Point &pt) {
  mSprite->setPosition(pt);
  if (mButton) {
    mButton->updatePosition();
  }
}

void BlockBase::normalizeUV() {
  if (mTextureName != "images/saw.png" && mTextureName != "images/saw_r.png") {
    mSprite->resetUV();
    return;
  }

  //Change uv
  auto w = getWidth();
  auto h = getThickness();
  mSprite->resetUV();
  if (w >= h) {
    if (mTextureName != "images/saw.png") {
      mTextureName = "images/saw.png";
      Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(mTextureName);
      mSprite->setTexture(texture);
    }

    Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_CLAMP_TO_EDGE};
    mSprite->getTexture()->setTexParameters(params);

    float l = w / h;
    mSprite->setUVWidth(l);
    if (mUVFlipped) {
      mSprite->flipUVY();
    }

  } else {
    if (mTextureName != "images/saw_r.png") {
      mTextureName = "images/saw_r.png";
      Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(mTextureName);
      mSprite->setTexture(texture);
    }

    Texture2D::TexParams params = {GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_REPEAT};
    mSprite->getTexture()->setTexParameters(params);

    float l = h / w;
    mSprite->setUVHeight(l);

    if (mUVFlipped) {
      mSprite->flipUVX();
    }
  }
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

  mSprite->setRotation(0);

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
    getSprite()->getPhysicsBody()->setVelocityLimit(1000);
  }
}

void BlockBase::updateMovement(float dt) {
  auto lastpos = mSprite->getPosition();
  auto newpos = mKind == KIND_PUSHABLE ? lastpos : mRestorePosition + mMovementToRestore;

  newpos.y += mVelocity.y * dt;
  newpos.x += mVelocity.x * dt;

  mSprite->setPosition(newpos);
  mMovementThisFrame = mSprite->getPosition() - lastpos;
}

void BlockBase::preUpdate() {
  mMovementToRestore = Vec2::ZERO;
  mUpSideMovement = Vec2::ZERO;
  mDownSideMovement = Vec2::ZERO;
  mRightSideMovement = Vec2::ZERO;
  mLeftSideMovement = Vec2::ZERO;
}

void BlockBase::update(float dt) {
  // Update velocity
  if (mEnableGravity) {
    float gy = GRAVITY_VAL;
    mVelocity.y += dt * gy;
    mVelocity.y = MIN(mVelocity.y, 1000);
    mVelocity.x = MIN(mVelocity.x, 600);
  }

  if (!mRotator.empty()) {
    auto rot = mRestoreRotation;
    auto size = mRestoreSize;
    float newRot = rot;
    Vec2 outsize(1, 1);
    mRotator.update(dt, newRot, outsize);
    mSprite->setRotation(newRot);
  }

  if (!mPath.empty()) {
    auto pos = mRestorePosition;
    auto newPos = pos;
    auto size = mRestoreSize;

    Vec2 outsize(1, 1);
    mPath.update(dt, newPos, outsize);

    mSprite->setScale(outsize.x * mRestoreSize.width / mImageSize,
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
    auto r = mSprite->getRotation();
    r += mRotationSpeed * dt;
    if (r > 360) r -= 360;
    mSprite->setRotation(r);
  }

  /**
   * event continue time
   */
  if (mTriggerEventsCalled) {
    mTriggerEventContinueTime += dt;
    mTriggerEventsCalled = false;
  } else {
    mTriggerEventContinueTime = 0.0f;
    if (mHeroOpacityChanged) {
      GameLogic::Game->mHero->getSprite()->setOpacity(255);
      mHeroOpacityChanged = false;
    }
  }

#if EDITOR_MODE
  if (mIDLabel) {
    if (mShowIDLabel) {
      mIDLabel->setVisible(true);
      mIDLabel->setPosition(mSprite->getPosition() -
                            Vec2(mIDLabel->getBoundingBox().size.width / 2,
                                 mIDLabel->getBoundingBox().size.height / 2));
    } else {
      mIDLabel->setVisible(false);
    }
  }
#endif
}

void BlockBase::setVisible(bool val) {
  mSprite->setVisible(val);
}

#if EDITOR_MODE

void BlockBase::initIDLabel() {
  char buffer[10];
  sprintf(buffer, "%d", mID);
  mIDLabel = LabelAtlas::create(buffer, "images/numbers.png", 37, 60, '0');
  mIDLabel->setScale(0.3);
  auto size = mIDLabel->getBoundingBox().size;
  EditorScene::Scene->addChild(mIDLabel, 800);
  mIDLabel->setPosition(mSprite->getPosition() - Vec2(size.width / 2, size.height / 2));
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
  if (mSprite->getPhysicsBody()) {
    mSprite->removeComponent(mSprite->getPhysicsBody());
  }

#if 1
  auto size = mSprite->getContentSize();
#else
  auto size = Size(mSprite->getScaleX() * mImageSize,
                   mSprite->getScaleY() * mImageSize);
#endif
  PhysicsBody *pbody = nullptr;
  if (mKind != KIND_DEATH_CIRCLE && mKind != KIND_FORCEFIELD) {
    pbody = PhysicsBody::createBox(size);
  } else {
    pbody = PhysicsBody::createCircle(std::max(size.height / 2, size.width / 2));
  }
  pbody->setContactTestBitmask(1);
  pbody->setGroup(1);
  pbody->setGravityEnable(false);
#if 0
  pbody->setScaleEnabled(false);
#endif

  if (mKind == KIND_PUSHABLE) {
    mEnableGravity = true;
    pbody->setDynamic(true);
    pbody->setRotationEnable(false);
    pbody->setMass(40);
  } else {
    pbody->setDynamic(false);
  }
  mSprite->setPhysicsBody(pbody);
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
      20,
      12
  };

  static bool castShadow[KIND_MAX] = {
      true,
      true,
      false,
      false,
      false,
      true,
      false
  };

  if (mPaletteIndex == -1) {
    mColor = Palette::getInstance()->getDefaultBlockColors(kind);
  } else {
    setColor(mPaletteIndex);
  }

  mKind = kind;
  mCastShadow = castShadow[kind];

  mSprite->setZOrder(kindZOrder[kind]);
  mSprite->setColor(mColor);

  if (kind == KIND_BUTTON) {
    mButton = new Button(this);
  } else {
    delete mButton;
    mButton = nullptr;
  }

  initPhysics();

  mSprite->setRotation(0);

  if (kind == KIND_DEATH || kind == KIND_DEATH_CIRCLE) {
    if (mTriggerEvents.empty()) {
      mTriggerEvents.push_back("die");
    }
  }

  if (kind == KIND_DEATH_CIRCLE || kind == KIND_FORCEFIELD) {
    auto s = Size(mSprite->getScaleX() * mImageSize,
                  mSprite->getScaleY() * mImageSize);
    auto size = std::max(s.width, s.height);

    if (kind == KIND_DEATH_CIRCLE) {
      mRotationSpeed = 50;
    }

    // Update image
    mImageSize = 8;

    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(mTextureName);
    mSprite->setTexture(texture);

    setWidth(size);
    setHeight(size);
    mRestoreSize = Size(size, size);
  } else if (kind == KIND_DEATH) {
    if (mTriggerEvents.size() == 1 && mTriggerEvents.at(0) == "die") {
      mTextureName = "images/saw.png";
    }

    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(mTextureName);
    mSprite->setTexture(texture);

    normalizeUV();
  } else {
    mImageSize = 8;
    mRotationSpeed = 0;

    mTextureName = "images/rect.png";
    mSprite->setTexture("images/rect.png");

    setSize(Size(mSprite->getScaleX() * mImageSize, mSprite->getScaleY() * mImageSize));
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
  getSprite()->setColor(mColor);
}

void BlockBase::moveX(float val) {
  setPositionX(val + mSprite->getPositionX());
  if (!mPath.empty()) {
    mPath.translatePoints(Vec2(val, 0));
#if EDITOR_MODE
    mPath.updateHelper();
#endif
  }
  mRestorePosition.x += val;
}

void BlockBase::moveY(float val) {
  setPositionY(val + mSprite->getPositionY());
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

  if (mKind == KIND_DEATH_CIRCLE || mKind == KIND_FORCEFIELD) {
    mSprite->setScale(t / mImageSize);
  } else {
    mSprite->setScale(mSprite->getScaleX(), t / mImageSize);
  }

  mRestoreSize = Size(mSprite->getScaleX() * mImageSize,
                      mSprite->getScaleY() * mImageSize);
  normalizeUV();
}

void BlockBase::subThickness(int val) {
  auto t = getThickness() - val;
  t = std::max<int>(t, 5);

  if (mKind == KIND_DEATH_CIRCLE || mKind == KIND_FORCEFIELD) {
    mSprite->setScale(t / mImageSize);
  } else {
    mSprite->setScale(mSprite->getScaleX(), t / mImageSize);
  }

  mRestoreSize = Size(mSprite->getScaleX() * mImageSize,
                      mSprite->getScaleY() * mImageSize);
  normalizeUV();
}

void BlockBase::addWidth(int val) {
  auto w = getWidth() + val;
  w = std::min<int>(w, VisibleRect::right().x * 1.3);

  if (mKind == KIND_DEATH_CIRCLE || mKind == KIND_FORCEFIELD)
    mSprite->setScale(w / mImageSize);
  else
    mSprite->setScale(w / mImageSize, mSprite->getScaleY());

  mRestoreSize = Size(mSprite->getScaleX() * mImageSize,
                      mSprite->getScaleY() * mImageSize);

  normalizeUV();
}

void BlockBase::subWidth(int val) {
  auto w = getWidth() - val;
  w = std::max<int>(w, 5);

  if (mKind == KIND_DEATH_CIRCLE || mKind == KIND_FORCEFIELD) {
    mSprite->setScale(w / mImageSize);
  } else {
    mSprite->setScale(w / mImageSize, mSprite->getScaleY());
  }
  mRestoreSize = Size(mSprite->getScaleX() * mImageSize,
                      mSprite->getScaleY() * mImageSize);
  normalizeUV();
}

void BlockBase::setWidth(float val) {
  if (mKind == KIND_DEATH_CIRCLE || mKind == KIND_FORCEFIELD) {
    mSprite->setScale(val / mImageSize);
  } else {
    mSprite->setScale(val / mImageSize, mSprite->getScaleY());
  }
  normalizeUV();
}

void BlockBase::setHeight(float val) {
  if (mKind == KIND_DEATH_CIRCLE || mKind == KIND_FORCEFIELD) {
    mSprite->setScale(val / mImageSize);
  } else {
    mSprite->setScale(mSprite->getScaleX(), val / mImageSize);
  }
  normalizeUV();
}

float BlockBase::getWidth() {
  auto scaleX = mSprite->getScaleX();
  return scaleX * mImageSize;
}

float BlockBase::getThickness() {
  auto scaleY = mSprite->getScaleY();
  return scaleY * mImageSize;
}

void BlockBase::setSize(Size size) {
  mSprite->setScale(size.width / mImageSize, size.height / mImageSize);
  mRestoreSize = size;
  normalizeUV();
}

cocos2d::Size BlockBase::getSize() {
  return Size(mSprite->getScaleX() * mImageSize, mSprite->getScaleY() * mImageSize);
}

void BlockBase::switchToNormalImage() {
  mSprite->setColor(getColor());
}

void BlockBase::switchToSelectionImage() {
  mSprite->setColor(Color3B(0, 0, 255));
}

void BlockBase::addToScene(cocos2d::Node *parent) {
  parent->addChild(mSprite, mZOrder);
#if EDITOR_MODE
  parent->addChild(mPath.mHelperNode, mZOrder + 1);
#endif
}

void BlockBase::getPointsForShadow(const cocos2d::Vec2 &source,
                                   std::vector<cocos2d::Vec2> &out) {
  auto size = getSize();
  auto p = mSprite->getPosition();
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
    Events::callEvent(mTriggerEvents[i].c_str(), this);
  }
}

void BlockBase::callInitEvent() {
  if (mInitialEvents.empty()) {
    return;
  }
  for (size_t i = 0; i < mInitialEvents.size(); ++i) {
    Events::callEvent(mInitialEvents[i].c_str(), this);
  }
}

void BlockBase::forceUpdatePhysicsPosition() {
  auto body = mSprite->getPhysicsBody();
  if (!body) {
    return;
  }

  Vec2 pos = mSprite->getPosition();
  auto scene = mSprite->getScene();
  if (scene && scene->getPhysicsWorld()) {
    pos = mSprite->getParent() == scene ? mSprite->getPosition() :
          scene->convertToNodeSpace(mSprite->getParent()->convertToWorldSpace(getPosition()));
  }

  body->setPosition(pos.x, pos.y);
}
