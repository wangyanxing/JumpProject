//
//  Blocks.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "Blocks.h"
#include "GameUtils.h"
#include "Button.h"
#include "LogicManager.h"
#include "GameScene.h"
#include "VisibleRect.h"

USING_NS_CC;

int BlockBase::mIDCounter = 0;

BlockBase::BlockBase() {
    mID = mIDCounter++;
    mColor = Color3B::WHITE;
}

BlockBase::~BlockBase() {
    mSprite->removeFromParent();
    
#if EDITOR_MODE
    mIDLabel->removeFromParent();
#endif
    
    if(mButton) {
        delete mButton;
        mButton = nullptr;
    }
}

void BlockBase::create(const cocos2d::Point& pt) {
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
    
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
    
    initPhysics();
}

void BlockBase::create(const cocos2d::Rect& rect) {
    mSprite = GameUtils::createRect(rect, getColor());
    
#if EDITOR_MODE
    initIDLabel();
#endif
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
    
    initPhysics();
}

void BlockBase::create(const cocos2d::Point& pt, const cocos2d::Size& size) {
    
    Rect r;
    r.origin = Point::ZERO;
    r.size = size;
    mSprite = GameUtils::createRect(r, getColor());
    
    setPosition(pt);
    
#if EDITOR_MODE
    initIDLabel();
#endif
    
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
    
    initPhysics();
}

void BlockBase::setPosition(const cocos2d::Point& pt) {
    mSprite->setPosition(pt);
    if(mButton) {
        mButton->updatePosition();
    }
}

void BlockBase::rotate() {
    auto w = getWidth();
    auto h = getThickness();
    setWidth(h);
    setHeight(w);
    
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
}

bool BlockBase::canPush() {
    //return !mPath.empty();
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
    
    if(mKind != KIND_DEATH_CIRCLE)
        setSize(mRestoreSize);
    
    setPosition(mRestorePosition);
    
    if(mButton) {
        mButton->reset();
    }
    
    mSprite->setRotation(0);
}

void BlockBase::updateOpenClose(float dt) {
    if(mStatus == CLOSING) {
        if(mDownDirDoor) {
            float upperbound = mRestorePosition.y + mRestoreSize.height / 2;
            float curHeight = getThickness();
            curHeight += dt * mOpenCloseSpeed;
            if(curHeight >= mRestoreSize.height) {
                curHeight = mRestoreSize.height;
                mStatus = IDLE;
            }
            setHeight(curHeight);
            setPositionY(upperbound - curHeight / 2);
        } else {
            float lowerbound = mRestorePosition.y - mRestoreSize.height / 2;
            float curHeight = getThickness();
            curHeight += dt * mOpenCloseSpeed;
            if(curHeight >= mRestoreSize.height) {
                curHeight = mRestoreSize.height;
                mStatus = IDLE;
            }
            setHeight(curHeight);
            setPositionY(lowerbound + curHeight / 2);
            
        }
    } else if(mStatus == OPENING) {
        if(mDownDirDoor) {
            float upperbound = mRestorePosition.y + mRestoreSize.height / 2;
            float curHeight = getThickness();
            curHeight -= dt * mOpenCloseSpeed;
            if(curHeight <= 1) {
                curHeight = 1;
                mStatus = IDLE;
            }
            setHeight(curHeight);
            setPositionY(upperbound - curHeight / 2);
        } else {
            float lowerbound = mRestorePosition.y - mRestoreSize.height / 2;
            float curHeight = getThickness();
            curHeight -= dt * mOpenCloseSpeed;
            if(curHeight <= 1) {
                curHeight = 1;
                mStatus = IDLE;
            }
            setHeight(curHeight);
            setPositionY(lowerbound + curHeight / 2);
            
        }
    }
}

void BlockBase::postUpdate(float dt) {
    if(mKind == KIND_PUSHABLE || mKind == KIND_HERO){
        getSprite()->getPhysicsBody()->setVelocityLimit(1000);
    }
}

void BlockBase::update(float dt) {
    
    if(!mPath.empty()) {
        auto pos = mSprite->getPosition();
        auto newPos = pos;
        auto size = getSize();
        
        Vec2 outsize(1,1);
        mPath.update(dt, newPos, outsize);
        
        mSprite->setPosition(newPos);
        mSprite->setScale(outsize.x * mRestoreSize.width / mImageSize,
                          outsize.y * mRestoreSize.height / mImageSize);
        
        auto newSize = getSize();
        
        mMovementThisFrame = newPos - pos;
        mUpSideMovement    = (newPos + Vec2(0, newSize.height/2)) - (pos + Vec2(0, size.height/2));
        mDownSideMovement  = (newPos + Vec2(0,-newSize.height/2)) - (pos + Vec2(0,-size.height/2));
        mRightSideMovement  = (newPos + Vec2( newSize.width/2,0)) - (pos + Vec2( size.width/2,0));
        mLeftSideMovement = (newPos + Vec2(-newSize.width/2,0)) - (pos + Vec2(-size.width/2,0));
        
        auto it = GameLogic::Game->mGroups.find(this);
        if(it != GameLogic::Game->mGroups.end()) {
            for(auto& c : it->second) {
                auto p = c->getPosition();
                switch(mFollowMode) {
                    case F_CENTER:
                        p += mMovementThisFrame;
                        break;
                    case F_LEFT:
                        p += mLeftSideMovement;
                        break;
                    case F_RIGHT:
                        p += mRightSideMovement;
                        break;
                    case F_DOWN:
                        p += mDownSideMovement;
                        break;
                    case F_UP:
                        p += mUpSideMovement;
                        break;
                }
                
                c->getSprite()->setPosition(p);
            }
        }
        
    } else {
        mMovementThisFrame.set(0, 0);
    }
    
    if(mButton) {
        mButton->update(dt);
    } else {
        updateOpenClose(dt);
    }
    
    if(mRotationSpeed > 0) {
        auto r = mSprite->getRotation();
        r += mRotationSpeed * dt;
        if(r > 360) r-=360;
        mSprite->setRotation(r);
    }
    
#if EDITOR_MODE
    if(mIDLabel) {
        if(mShowIDLabel) {
            mIDLabel->setVisible(true);
            mIDLabel->setPosition(mSprite->getPosition() -
                                  Vec2(mIDLabel->getBoundingBox().size.width/2,
                                       mIDLabel->getBoundingBox().size.height/2));
        } else {
            mIDLabel->setVisible(false);
        }
    }
#endif
}

void BlockBase::setVisible(bool val) {
    mSprite->setVisible(val);
    mSprite->getPhysicsBody()->setResting(val);
}

#if EDITOR_MODE
void BlockBase::initIDLabel() {
    char buffer[10];
    sprintf(buffer, "%d", mID);
    mIDLabel = LabelAtlas::create(buffer, "images/numbers.png", 37, 60, '0');
    mIDLabel->setScale(0.3);
    auto size = mIDLabel->getBoundingBox().size;
    GameScene::Scene->addChild(mIDLabel, 800);
    mIDLabel->setPosition(mSprite->getPosition() - Vec2(size.width/2, size.height/2));
    mShowIDLabel = GameScene::Scene->mShowGrid;
    mIDLabel->setVisible(mShowIDLabel);
}
void BlockBase::updateIDLabel() {
    char buffer[10];
    sprintf(buffer, "%d", mID);
    mIDLabel->setString(buffer);
}
#endif

void BlockBase::initPhysics() {
    
    auto size = getBoundingBox().size;
    PhysicsBody* pbody = nullptr;
    if(mKind != KIND_DEATH_CIRCLE)
        pbody = PhysicsBody::createBox(size);
    else
        pbody = PhysicsBody::createCircle(std::max(size.height/2, size.width/2));
    
    pbody->setDynamic(false);
    pbody->setContactTestBitmask(1);
    
    mSprite->setPhysicsBody(pbody);
}

void BlockBase::setPhysicsDynamic(bool val) {
    mSprite->getPhysicsBody()->setDynamic(val);
}

void BlockBase::setKind(BlockKind kind) {
    if(kind == mKind) return;
    
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
    
    mColor = GameLogic::Game->mBlockColors[kind];
    mKind = kind;
    mCastShadow = castShadow[kind];
    
    mSprite->setZOrder(kindZOrder[kind]);
    mSprite->setColor(mColor);
    
    if(kind == KIND_BUTTON) {
        mButton = new Button(this);
    } else {
        delete mButton;
        mButton = nullptr;
    }
    
    initPhysics();
    
    mSprite->setRotation(0);
    
    if(kind == KIND_DEATH_CIRCLE) {
        auto s = getBoundingBox().size;
        auto size = std::max(s.width, s.height);
        mRotationSpeed = 50;
        
        // update image
        mImageSize = 10;
        
		Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(textureName);
        mSprite->setTexture(texture);
        
        setWidth(size);
        setHeight(size);
        mRestoreSize = Size(size,size);
    } else {
        mImageSize = 10;
        mRotationSpeed = 0;
        
        mSprite->setTexture("images/rect.png");
        
        setSize(getBoundingBox().size);
    }
    
    if(kind != KIND_HERO) {
        if(kind == KIND_PUSHABLE) {
            mSprite->getPhysicsBody()->setDynamic(true);
            mSprite->getPhysicsBody()->setRotationEnable(false);
            mSprite->getPhysicsBody()->setMass(40);
            //mSprite->getPhysicsBody()->setGravityEnable(false);
        } else {
            mSprite->getPhysicsBody()->setDynamic(false);
        }
    }
}

void BlockBase::moveX(float val) {
    if(mPath.empty()) {
        auto p = mSprite->getPosition();
        setPosition(p.x + val, p.y);
        
    } else {
        mPath.translatePoints(Vec2(val, 0));
    }
    mRestorePosition = getPosition();
}

void BlockBase::moveY(float val) {
    if(mPath.empty()) {
        auto p = mSprite->getPosition();
        setPosition(p.x, p.y + val);
    } else {
        mPath.translatePoints(Vec2(0, val));
    }
    mRestorePosition = getPosition();
}

void BlockBase::addThickness(int val) {
    auto t = getThickness();
    t += val;
    t = std::min<int>(t, VisibleRect::top().x * 1.2);
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(t / mImageSize);
    else
        mSprite->setScale(mSprite->getScaleX(), t / mImageSize);
    
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
}

void BlockBase::subThickness(int val) {
    auto t = getThickness();
    t -= val;
    t = std::max<int>(t, 5);
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(t / mImageSize);
    else
        mSprite->setScale(mSprite->getScaleX(), t / mImageSize);
    
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
}

void BlockBase::addWidth(int val) {
    auto w = getWidth();
    w += val;
    w = std::min<int>(w, VisibleRect::right().x * 1.2);
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(w / mImageSize);
    else
        mSprite->setScale(w / mImageSize, mSprite->getScaleY());
    
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
}

void BlockBase::subWidth(int val) {
    auto w = getWidth();
    w -= val;
    w = std::max<int>(w, 5);
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(w / mImageSize);
    else
        mSprite->setScale(w / mImageSize, mSprite->getScaleY());
    
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
}

void BlockBase::setWidth(float val) {
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(val / mImageSize);
    else
        mSprite->setScale(val / mImageSize, mSprite->getScaleY());
}

void BlockBase::setHeight(float val) {
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(val / mImageSize);
    else
        mSprite->setScale(mSprite->getScaleX(), val / mImageSize);
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
    mRestoreSize = getBoundingBox().size;
}

cocos2d::Size BlockBase::getSize() {
    return Size(mSprite->getScaleX() * mImageSize, mSprite->getScaleY() * mImageSize);
}

void BlockBase::switchToNormalImage() {
    mSprite->setColor(getColor());
}

void BlockBase::switchToSelectionImage() {
    mSprite->setColor(Color3B(0,0,255));
}

void BlockBase::addToScene(cocos2d::Node* parent) {
    parent->addChild(mSprite, mZOrder);
#if EDITOR_MODE
    parent->addChild(mPath.mHelperNode, mZOrder + 1);
#endif
}

void BlockBase::getPointsForShadow(const cocos2d::Vec2& source,
                                   std::vector<cocos2d::Vec2>& out) {
    auto b = mSprite->getBoundingBox();
    auto p = mSprite->getPosition();
    out.resize(4);
    out[0] = p + Vec2(-b.size.width/2,  b.size.height/2);
    out[1] = p + Vec2(-b.size.width/2, -b.size.height/2);
    out[2] = p + Vec2( b.size.width/2,  b.size.height/2);
    out[3] = p + Vec2( b.size.width/2, -b.size.height/2);
}

void Hero::initPhysics() {
    auto size = getBoundingBox().size;
    PhysicsBody* pbody = nullptr;
    pbody = PhysicsBody::createBox(size);
    pbody->setDynamic(true);
    pbody->setRotationEnable(false);
    pbody->setMoment(0);
    pbody->setMass(0.8);
    pbody->setContactTestBitmask(1);
    getSprite()->setPhysicsBody(pbody);
}
