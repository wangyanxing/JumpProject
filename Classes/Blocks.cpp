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
    
    mRestoreSize = r.size;
    mRestorePosition = getPosition();
    
    initPhysics();
}

void BlockBase::create(const cocos2d::Rect& rect) {
    mSprite = GameUtils::createRect(rect, getColor());
    
#if EDITOR_MODE
    initIDLabel();
#endif
    mRestoreSize = rect.size;
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
    
    mRestoreSize = size;
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
    
    mRestoreSize = Size(h,w);
    mRestorePosition = getPosition();
    
    //change uv
    auto neww = h;
    auto newh = w;
    mSprite->resetUV();
    if(neww >= newh) {
        float l = neww / newh;
        mSprite->setUVWidth(l);
    } else {
        float l = newh / neww;
        mSprite->setUVWidth(l);
        mSprite->rotateUV();
    }
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

void BlockBase::updatePathMove() {
    if(mKind == KIND_PUSHABLE) return;
    auto lastpos = mSprite->getPosition();
    mSprite->setPosition(mRestorePosition + mMovementToRestore);
    
    mMovementThisFrame = mSprite->getPosition() - lastpos;
}

void BlockBase::preUpdate() {
    mMovementToRestore = Vec2(0,0);
    mUpSideMovement    = Vec2(0,0);
    mDownSideMovement  = Vec2(0,0);
    mRightSideMovement = Vec2(0,0);
    mLeftSideMovement  = Vec2(0,0);
}

void BlockBase::update(float dt) {
    
    if(!mRotator.empty()) {
        auto rot = mRestoreRotation;
        auto size = mRestoreSize;
        float newRot = rot;
        Vec2 outsize(1,1);
        mRotator.update(dt, newRot, outsize);
        mSprite->setRotation(newRot);
    }
    
    if(!mPath.empty()) {
        auto pos = mRestorePosition;//mSprite->getPosition();
        auto newPos = pos;
        auto size = mRestoreSize;
        
        Vec2 outsize(1,1);
        mPath.update(dt, newPos, outsize);
        
        //mSprite->setPosition(newPos);
        mSprite->setScale(outsize.x * mRestoreSize.width / mImageSize,
                          outsize.y * mRestoreSize.height / mImageSize);
        
        auto newSize = getSize();
        
        mMovementToRestore += newPos - pos;
        mUpSideMovement    = (newPos + Vec2(0, newSize.height/2)) - (pos + Vec2(0, size.height/2));
        mDownSideMovement  = (newPos + Vec2(0,-newSize.height/2)) - (pos + Vec2(0,-size.height/2));
        mRightSideMovement = (newPos + Vec2( newSize.width/2, 0)) - (pos + Vec2( size.width/2, 0));
        mLeftSideMovement  = (newPos + Vec2(-newSize.width/2, 0)) - (pos + Vec2(-size.width/2, 0));
        
        auto it = GameLogic::Game->mGroups.find(this);
        if(it != GameLogic::Game->mGroups.end()) {
            for(auto& c : it->second) {

                switch(mFollowMode) {
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
                
                //c->getSprite()->setPosition(p);
            }
        }        
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

    /**
    * event continue time
    */
    if (mTriggerEventsCalled){
        mTriggerEventContinueTime += dt;
        mTriggerEventsCalled = false;
    }
    else{
        mTriggerEventContinueTime = 0.0f;
        if (mHeroOpacityChanged){
            GameLogic::Game->mHero->getSprite()->setOpacity(255);
            mHeroOpacityChanged = false;
        }
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
    //mSprite->getPhysicsBody()->setResting(val);
}

#if EDITOR_MODE
void BlockBase::initIDLabel() {
    char buffer[10];
    sprintf(buffer, "%d", mID);
    mIDLabel = LabelAtlas::create(buffer, "images/numbers.png", 37, 60, '0');
    mIDLabel->setScale(0.3);
    auto size = mIDLabel->getBoundingBox().size;
    EditorScene::Scene->addChild(mIDLabel, 800);
    mIDLabel->setPosition(mSprite->getPosition() - Vec2(size.width/2, size.height/2));
    mShowIDLabel = EditorScene::Scene->mShowGrid;
    mIDLabel->setVisible(mShowIDLabel);
}
void BlockBase::updateIDLabel() {
    char buffer[10];
    sprintf(buffer, "%d", mID);
    mIDLabel->setString(buffer);
}
#endif

void BlockBase::initPhysics() {
    
    auto size = Size(mSprite->getScaleX() * mImageSize,
                     mSprite->getScaleY() * mImageSize);
    PhysicsBody* pbody = nullptr;
    if(mKind != KIND_DEATH_CIRCLE)
        pbody = PhysicsBody::createBox(size);
    else
        pbody = PhysicsBody::createCircle(std::max(size.height/2, size.width/2));
    
    pbody->setDynamic((mKind == KIND_PUSHABLE || mKind == KIND_HERO) ? true : false);
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
    
	if (mPaletteIndex == -1)
		mColor = GameLogic::Game->mBlockColors[kind];
	else
		setColor(mPaletteIndex);

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
    
    if (kind == KIND_DEATH || kind == KIND_DEATH_CIRCLE){
        if (mTriggerEvents.empty()){
            mTriggerEvents.push_back("die");
        }
    }

    if(kind == KIND_DEATH_CIRCLE) {
        auto s = Size(mSprite->getScaleX() * mImageSize,
                      mSprite->getScaleY() * mImageSize);
        auto size = std::max(s.width, s.height);
        mRotationSpeed = 50;
        
        // update image
        mImageSize = 8;
        
        Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(mTextureName);
        mSprite->setTexture(texture);
        
        setWidth(size);
        setHeight(size);
        mRestoreSize = Size(size,size);
    }else if(kind == KIND_DEATH) {
        if (mTriggerEvents.size() == 0 && mTriggerEvents.at(0) == "die")
            mTextureName = "images/saw.png";

        Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(mTextureName);
        mSprite->setTexture(texture);
        mSprite->setupTexParameters();
    } else {
        mImageSize = 8;
        mRotationSpeed = 0;
        
        mTextureName = "images/rect.png";
        mSprite->setTexture("images/rect.png");
        
        setSize(Size(mSprite->getScaleX() * mImageSize,
                     mSprite->getScaleY() * mImageSize));
    }
    
    if(kind != KIND_HERO) {
        if(kind == KIND_PUSHABLE) {
            mSprite->getPhysicsBody()->setRotationEnable(false);
            mSprite->getPhysicsBody()->setMass(40);
        }
    }
}

void BlockBase::setColor(int index){
	mPaletteIndex = index;
	Color3B color = GameLogic::Game->getColorFromPalette(index);
	mColor = color;
	getSprite()->setColor(color);
}

void BlockBase::moveX(float val) {
    if(mPath.empty()) {
        auto p = mSprite->getPosition();
        setPosition(p.x + val, p.y);
        
    } else {
        mPath.translatePoints(Vec2(val, 0));
    }
    mRestorePosition.x += val;
}

void BlockBase::moveY(float val) {
    if(mPath.empty()) {
        auto p = mSprite->getPosition();
        setPosition(p.x, p.y + val);
    } else {
        mPath.translatePoints(Vec2(0, val));
    }
    mRestorePosition.y += val;
}

void BlockBase::addThickness(int val) {
    auto t = getThickness();
    t += val;
    t = std::min<int>(t, VisibleRect::top().x * 1.2);
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(t / mImageSize);
    else
        mSprite->setScale(mSprite->getScaleX(), t / mImageSize);
    
    mRestoreSize = Size(mSprite->getScaleX() * mImageSize,
                        mSprite->getScaleY() * mImageSize);
    
    auto size = getSize();
    mSprite->setUVWidth(size.width / size.height);
    mSprite->setUVHeight(1);
}

void BlockBase::subThickness(int val) {
    auto t = getThickness();
    t -= val;
    t = std::max<int>(t, 5);
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(t / mImageSize);
    else
        mSprite->setScale(mSprite->getScaleX(), t / mImageSize);
    
    mRestoreSize = Size(mSprite->getScaleX() * mImageSize,
                        mSprite->getScaleY() * mImageSize);
    
    auto size = getSize();
    mSprite->setUVWidth(size.width / size.height);
    mSprite->setUVHeight(1);
}

void BlockBase::addWidth(int val) {
    auto w = getWidth();
    w += val;
    w = std::min<int>(w, VisibleRect::right().x * 1.2);
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(w / mImageSize);
    else
        mSprite->setScale(w / mImageSize, mSprite->getScaleY());
    
    mRestoreSize = Size(mSprite->getScaleX() * mImageSize,
                        mSprite->getScaleY() * mImageSize);
    
    auto size = getSize();
    mSprite->setUVWidth(size.width / size.height);
    mSprite->setUVHeight(1);
}

void BlockBase::subWidth(int val) {
    auto w = getWidth();
    w -= val;
    w = std::max<int>(w, 5);
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(w / mImageSize);
    else
        mSprite->setScale(w / mImageSize, mSprite->getScaleY());
    
    mRestoreSize = Size(mSprite->getScaleX() * mImageSize,
                        mSprite->getScaleY() * mImageSize);
    
    auto size = getSize();
    mSprite->setUVWidth(size.width / size.height);
    mSprite->setUVHeight(1);
}

void BlockBase::setWidth(float val) {
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(val / mImageSize);
    else
        mSprite->setScale(val / mImageSize, mSprite->getScaleY());
    
    auto size = getSize();
    mSprite->setUVWidth(size.width / size.height);
    mSprite->setUVHeight(1);
}

void BlockBase::setHeight(float val) {
    
    if(mKind == KIND_DEATH_CIRCLE)
        mSprite->setScale(val / mImageSize);
    else
        mSprite->setScale(mSprite->getScaleX(), val / mImageSize);
    
    auto size = getSize();
    mSprite->setUVWidth(size.width / size.height);
    mSprite->setUVHeight(1);
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

    mSprite->setUVWidth(size.width / size.height);
    mSprite->setUVHeight(1);
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
    auto size = getSize();
    auto p = mSprite->getPosition();
    out.resize(4);
    out[0] = p + Vec2(-size.width/2,  size.height/2);
    out[1] = p + Vec2(-size.width/2, -size.height/2);
    out[2] = p + Vec2( size.width/2,  size.height/2);
    out[3] = p + Vec2( size.width/2, -size.height/2);
}

void BlockBase::callTriggerEvent(){
    if (mTriggerEvents.empty()) return;

    mTriggerEventsCalled = true;

    for (size_t i = 0; i < mTriggerEvents.size(); ++i) {
        Events::callEvent(mTriggerEvents[i].c_str(), this);
    }
}

void Hero::initPhysics() {
    PhysicsBody* pbody = nullptr;
    pbody = PhysicsBody::createBox(getSize());
    pbody->setDynamic(true);
    pbody->setRotationEnable(false);
    pbody->setMoment(0);
    pbody->setMass(0.8);
    pbody->setContactTestBitmask(1);
    getSprite()->setPhysicsBody(pbody);
}
