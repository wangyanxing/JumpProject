#include "GameScene.h"
#include "VisibleRect.h"
#include "GameUtils.h"
#include "MapSerial.h"
#include "UILayer.h"
#include "Events.h"

#include <iostream>

USING_NS_CC;

void colorMix(const Color4B& src, const Color4B& dst, float r, Color4B& out) {
    out.r = dst.r * r + src.r * (1 - r);
    out.g = dst.g * r + src.g * (1 - r);
    out.b = dst.b * r + src.b * (1 - r);
    out.a = dst.a * r + src.a * (1 - r);
}

ShadowManager::ShadowManager(cocos2d::Node* parentNode) {
    
    mRenderer = DrawNodeEx::create("images/rect.png");
    parentNode->addChild(mRenderer, 2);
    
    mLightPos = VisibleRect::center();
    mLightPos.x = 300;
    mLightPos.y = VisibleRect::top().y - 10;
}

ShadowManager::~ShadowManager() {
}

void ShadowManager::update(const std::map<int,BlockBase*>& blocks) {
    mRenderer->clear();
    std::vector<V2F_C4B_T2F_Triangle> triangles;
    
    Color4B colorBase = Color4B::BLACK;
    colorBase.a = 25;
    
    for(auto b : blocks) {
        auto block = b.second;
        if(!block->mCastShadow || !block->isVisible()) continue;
        
        std::vector<Vec2> pts;
        block->getPointsForShadow(mLightPos, pts);
        
        float minRet = 1000;
        float maxRet = -1000;
        Vec2 minPt, maxPt;
        
        for(const auto& p : pts) {
            float x = abs(p.x - mLightPos.x);
            float y = abs(p.y - mLightPos.y);
            float ret = atan2(y, x);
            if(p.y - mLightPos.y < 0) {
                ret = 180-ret;
            }
            if(p.x - mLightPos.x < 0) {
                ret = -ret;
            }
            if(ret < minRet) {
                minRet = ret;
                minPt = p;
            }
            if(ret > maxRet) {
                maxRet = ret;
                maxPt = p;
            }
        }
        
        Vec2 minPtFar = minPt - mLightPos;
        float lengthMin = minPtFar.length();
        
        Vec2 maxPtFar = maxPt - mLightPos;
        float lengthMax = maxPtFar.length();
        
        float maxLen = std::max(lengthMax, lengthMin) + block->mShadowLength;
        
        minPtFar.x /= lengthMin;
        minPtFar.y /= lengthMin;
        
        maxPtFar.x /= lengthMax;
        maxPtFar.y /= lengthMax;
        
        auto dir0 = minPtFar;
        auto dir1 = minPtFar;
        auto curPt0 = minPt;
        auto curPt1 = maxPt;
        auto len0 = maxLen - lengthMin;
        auto len1 = maxLen - lengthMax;
        
        for (int i = 0; i < block->mShadowFadeSegments + 1; i++) {
            
            float curlen0 = (i==0) ? block->mShadowFadeRatio * len0 :
                (1.0-block->mShadowFadeRatio) * len0 / block->mShadowFadeSegments;
            
            float curlen1 = (i==0) ? block->mShadowFadeRatio * len1 :
                (1.0-block->mShadowFadeRatio) * len0 / block->mShadowFadeSegments;
            
            Color4B color0 = colorBase;
            Color4B color1 = colorBase;
            
            if(i > 0) {
                colorMix(colorBase, Color4B(0,0,0,0), (i-1.0) / block->mShadowFadeSegments, color0);
                colorMix(colorBase, Color4B(0,0,0,0), (i-1.0) / block->mShadowFadeSegments, color1);
            }
            
            auto pt0 = curPt0;
            auto pt1 = curPt1;
            
            curPt0 = curPt0 + dir0 * curlen0;
            curPt1 = curPt1 + dir1 * curlen1;
            
            V2F_C4B_T2F_Triangle t;
            t.a.vertices = pt0;
            t.a.colors = color0;
            
            t.b.vertices = curPt0;
            t.b.colors = color0;
            
            t.c.vertices = pt1;
            t.c.colors = color0;
            
            triangles.push_back(t);
            
            t.a.vertices = pt1;
            t.a.colors = color1;
            
            t.b.vertices = curPt0;
            t.b.colors = color1;
            
            t.c.vertices = curPt1;
            t.c.colors = color1;
            
            triangles.push_back(t);
        }
    }
    
    if(!triangles.empty())
        mRenderer->drawTriangles(triangles);
}

////////////////////////////////////////////////

int BlockBase::mIDCounter = 0;

BlockBase::BlockBase() {
    mID = mIDCounter++;
    mColor = Color3B::WHITE;
}

BlockBase::~BlockBase() {
    mSprite->removeFromParent();
    
#ifdef EDITOR_MODE
    mIDLabel->removeFromParent();
#endif
    
    delete mButton;
    mButton = nullptr;
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
    
#ifdef EDITOR_MODE
    initIDLabel();
#endif
    
    mRestoreSize = getBoundingBox().size;
    mRestorePosition = getPosition();
    
    initPhysics();
}

void BlockBase::create(const cocos2d::Rect& rect) {
    mSprite = GameUtils::createRect(rect, getColor());
    
#ifdef EDITOR_MODE
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
    
#ifdef EDITOR_MODE
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

void BlockBase::resetOpenClose() {
    mStatus = IDLE;
    
    setSize(mRestoreSize);
    setPosition(mRestorePosition);
    
    if(mButton) {
        mButton->reset();
    }
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
    if(mKind == KIND_PUSHABLE){
        getSprite()->getPhysicsBody()->setVelocityLimit(1000);
    }
}

void BlockBase::update(float dt) {
    auto pos = mSprite->getPosition();
    mPath.update(dt, pos);
    
    if(!mPath.empty()) {
        mMovementThisFrame = pos - mSprite->getPosition();
    } else {
        mMovementThisFrame.set(0, 0);
    }
    
    mSprite->setPosition(pos);
    
    if(mButton) {
        mButton->update(dt);
        //printf("%g\n",mRestorePosition.y);
    } else {
        updateOpenClose(dt);
    }
    
#ifdef EDITOR_MODE
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

#ifdef EDITOR_MODE
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
    
    mColor = GameScene::Scene->mBlockColors[kind];
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
    
    if(kind == KIND_DEATH_CIRCLE) {
        auto s = getBoundingBox().size;
        auto size = std::max(s.width, s.height);
        
        // update image
        mImageSize = 10;
        
        Texture2D *texture = Director::getInstance()->getTextureCache()->addImage("images/saw2.png");
        mSprite->setTexture(texture);
        
        setWidth(size);
        setHeight(size);
    } else {
        mImageSize = 10;

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
        mRestorePosition = getPosition();
    } else {
        mPath.translatePoints(Vec2(val, 0));
    }
}

void BlockBase::moveY(float val) {
    if(mPath.empty()) {
        auto p = mSprite->getPosition();
        setPosition(p.x, p.y + val);
        mRestorePosition = getPosition();
    } else {
        mPath.translatePoints(Vec2(0, val));
    }
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
#ifdef EDITOR_MODE
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

////////////////////////

GameScene* GameScene::Scene = nullptr;

// on "init" you need to initialize your instance
bool GameScene::init() {
    Scene = this;
    
    if ( !Layer::init() ) {
        return false;
    }
    
    // default colors
    mBlockColors[KIND_HERO] = Color3B::BLACK;
    mBlockColors[KIND_BLOCK] = Color3B::WHITE;
    mBlockColors[KIND_DEATH] = Color3B::BLACK;
    mBlockColors[KIND_DEATH_CIRCLE] = Color3B::BLACK;
    mBlockColors[KIND_BUTTON] = Color3B(254, 225, 50);
    mBlockColors[KIND_PUSHABLE] = Color3B(220, 150, 168);
    mBackgroundColor = Color3B(30, 181, 199);
    
    MapSerial::saveRemoteMaps();
    
    getScheduler()->scheduleUpdate(this, -2, false);
    
    getScheduler()->scheduleUpdate(&mPostUpdater, -1, false);
    
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::keyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::keyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(GameScene::mouseDown, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(GameScene::mouseUp, this);
    mouseListener->onMouseMove = CC_CALLBACK_1(GameScene::mouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactPreSolve = CC_CALLBACK_2(GameScene::onContactPreSolve, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    mSpawnPoint = Sprite::create("images/cross.png");
    addChild(mSpawnPoint, 100);
    //mSpawnPoint->setPosition(VisibleRect::center());
    mSpawnPoint->setPosition(50,100);
    mSpawnPoint->setScale(0.3);
    
    mHero = new Hero();
    //mHero->create(VisibleRect::center());
    mHero->create(Vec2(32,100));
    mHero->setSize(Size(30,30));
    mHero->setVisible(false);
    mHero->setKind(KIND_HERO);
    mHeroShape = mHero->getSprite()->getPhysicsBody()->getShapes().front();
    mHero->addToScene(this);
    mBlockTable[mHero->getSprite()] = mHero;
    
    mShadows = new ShadowManager(this);
    
    auto back = GameUtils::createRect(VisibleRect::getVisibleRect(), Color3B(30, 181, 199));
    back->setTag(1000);
    addChild(back, 0);
    
    createBackground();
    
    enableGame(false);
    
    
    MapSerial::loadLastEdit();
    
    return true;
}

bool GameScene::onContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve) {
    
    auto iA = mBlockTable.find(contact.getShapeA()->getBody()->getNode());
    auto iB = mBlockTable.find(contact.getShapeB()->getBody()->getNode());
    if(iA == mBlockTable.end() || iB == mBlockTable.end()) {
        return true;
    }
    
    BlockBase* blockA = iA->second;
    BlockBase* blockB = iB->second;
    
    PhysicsShape* pusherShape = mHeroShape;
    if(blockA != mHero && blockB != mHero) {
        pusherShape = blockA->pushable() ? contact.getShapeA() : contact.getShapeB();
    }
    
    auto data = contact.getContactData();
    auto normal = data->normal;
    auto otherShape = pusherShape == contact.getShapeA() ? contact.getShapeB() : contact.getShapeA();
    
    bool onMovingPlatform = false;
    bool onButton = false;
    
    if(pusherShape == contact.getShapeA()) normal *= -1;
    
    auto otherNode = otherShape->getBody()->getNode();
    
    BlockBase* otherBlock = nullptr;
    BlockBase* thisBlock = nullptr;
    auto i = mBlockTable.find(otherNode);
    if(i != mBlockTable.end()) {
        otherBlock = i->second;
    } else {
        return false;
    }
    
    if(pusherShape == mHeroShape) {
        thisBlock = mHero;
    } else {
        auto it = mBlockTable.find(pusherShape->getBody()->getNode());
        thisBlock = it->second;
    }
    
    BlockBase* pushedObject = nullptr;
    BlockBase* pushObject = nullptr;
    
    if(!otherBlock || !otherBlock->pushable() ||
       (normal.y > 0.9 || normal.y < -0.9 ||
        (otherBlock->pushable() && !otherBlock->mCanPush))) {
        otherBlock->mCanPush = true;
        pushObject = thisBlock;
        pushedObject = otherBlock;
    } else {
        // pushing
        pushObject = otherBlock;
        pushedObject = thisBlock;
        if(pushedObject == mHero) {
            mHero->mPushing = true;
        }
    }
    
    if( otherBlock->mKind == KIND_DEATH ||
       otherBlock->mKind == KIND_DEATH_CIRCLE) {
        // process dead logic
        die();
        return false;
    } else if( otherBlock->mKind == KIND_BLOCK ) {
        if(normal.y > 0.9) {
            auto p = pushObject->getPosition();
            if(!otherBlock->mPath.empty()) {
                p += otherBlock->mMovementThisFrame;
                onMovingPlatform = true;
            }
            pushObject->setPosition(p);
        }
    } else if( otherBlock->mKind == KIND_BUTTON ) {
        
        if(otherBlock->mButton->push(normal, pushObject)) {
            return false;
        }
        if(otherBlock->mButton->mDir == Button::DIR_UP)
            onButton = true;
        
        if(!blockA->canPush() && !blockB->canPush())
            return false;
    }
    
    if(!blockA->canPush() && !blockB->canPush()) {
        if(normal.x > 0.9 || normal.x < -0.9) {
            if(blockA->pushable()) {
                blockA->mCanPush = false;
            } else if(blockB->pushable()) {
                blockB->mCanPush = false;
            } else {
                return true;
            }
        } else if(normal.y > 0.9 || normal.y < -0.9) {
            if(!blockA->pushable() && !blockB->pushable()) {
                return true;
            }
        }
    }
    
    if(normal.x > 0.9 || normal.x < -0.9) {
        
        auto h = pushedObject->getSprite()->getBoundingBox().size.width/2 +
                 pushObject->getBoundingBox().size.width/2;
        if(onMovingPlatform)
            h += 1;
        auto phyPos = pushObject->getSprite()->getPhysicsBody()->getPosition();
        if (pushObject->getPosition().x < pushedObject->getSprite()->getPositionX())
            pushObject->setPosition(pushedObject->getSprite()->getPositionX() - h, phyPos.y);
        else
            pushObject->setPosition(pushedObject->getSprite()->getPositionX() + h, phyPos.y);
    }
    
    if(normal.y > 0.9 || normal.y < -0.9) {
        
        if(pushObject == mHero)
            mHero->mCanJump = true;
        
        auto h = pushedObject->getBoundingBox().size.height/2 + pushObject->getBoundingBox().size.height/2;

        if(onMovingPlatform && pushObject == mHero) {
            h -= 1;
        }
        
        if (pushObject->getPosition().y < pushedObject->getSprite()->getPositionY()){
            if(onButton) h -= 1;
            else h += 1;
            pushObject->setPositionY(pushedObject->getSprite()->getPositionY() - h);
        }else{
            pushObject->setPositionY(pushedObject->getSprite()->getPositionY() + h);
            
            if(pushObject->mKind == KIND_PUSHABLE) {
                //printf("PRESOLVE: %g\n",pushObject->getPosition().y);
                pushObject->getSprite()->getPhysicsBody()->setVelocityLimit(0);
//                pushObject->getSprite()->getPhysicsBody()->resetForces();
//                pushObject->getSprite()->getPhysicsBody()->setGravityEnable(false);
            }
        }
        
        auto v = pushObject->getSprite()->getPhysicsBody()->getVelocity();
        pushObject->getSprite()->getPhysicsBody()->setVelocity(Vec2(v.x, 0));
    }
    
    return false;
}

void GameScene::mouseDown(cocos2d::Event* event) {
    auto mouse = (EventMouse*)event;
    Point pt(mouse->getCursorX(), mouse->getCursorY());
    convertMouse(pt);
    
    if (mPressingM) {
        mSpawnPoint->setPosition(pt);
        return;
    }
    
    if (mPressingShift) {
        BlockBase* block = new BlockBase();
        block->create(pt);
        block->setKind(KIND_BLOCK);
        block->addToScene(this);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;

        mMovingBlock = nullptr;
    } else {
        // pick up
        if(!mPressingCtrl) {
            mSelections.clear();
            mPathMode = false;
            mSelectionHead = nullptr;
        }
        
        for(auto it = mBlocks.begin(); it != mBlocks.end(); ++it) {
            auto bl = it->second;
            bl->switchToNormalImage();
            auto box = bl->getBoundingBox();
            if(box.containsPoint(pt) && bl->mCanPickup) {
                mSelections.insert(bl);
                mMovingBlock = bl;
                mSelectionHead = bl;
            }
        }
        
        mLastPoint = pt;
        
        for(auto sel: mSelections) {
            sel->switchToSelectionImage();
        }
        if(mSelectionHead)
            mSelectionHead->getSprite()->setColor(Color3B(200,0,255));
    }
}

void GameScene::mouseUp(cocos2d::Event* event) {
    mMovingBlock = nullptr;
}

void GameScene::mouseMove(cocos2d::Event* event) {
    if(!mMovingBlock)
        return;
    
    auto mouse = (EventMouse*)event;
    Point pt(mouse->getCursorX(), mouse->getCursorY());
    convertMouse(pt);
    
    Point dt = pt - mLastPoint;
    mLastPoint = pt;
    
    for(auto sel: mSelections) {
        sel->moveX(dt.x);
        sel->moveY(dt.y);
    }
}

void GameScene::convertMouse(cocos2d::Point& pt) {
    auto visRect = Director::getInstance()->getOpenGLView()->getVisibleRect();
    auto height = visRect.origin.y + visRect.size.height;
    
    pt.y = height + pt.y;
    pt = convertToNodeSpace(pt);
}

void GameScene::keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
    if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT) {
        mPressingShift = true;
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_CTRL) {
        mPressingCtrl = true;
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_ALT) {
        mPressingAlt = true;
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_V && mPressingCtrl) {
        duplicate();
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_EQUAL) {
        for(auto sel : mSelections) {
            sel->addWidth(mPressingAlt ? 20 : 5);
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_MINUS) {
        for(auto sel : mSelections) {
            sel->subWidth(mPressingAlt ? 20 : 5);
        }
    }
    
    if (keyCode >= EventKeyboard::KeyCode::KEY_1 &&
        keyCode <= EventKeyboard::KeyCode::KEY_9) {
        setKind((int)keyCode - (int)EventKeyboard::KeyCode::KEY_1 + 1);
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_BRACKET) {
        for(auto sel : mSelections) {
            sel->subThickness(mPressingAlt ? 10 : 5);
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_BRACKET) {
        for(auto sel : mSelections) {
            sel->addThickness(mPressingAlt ? 10 : 5);
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
        if(mPathMode && mSelectionHead) {
            auto p = mSelectionHead->mPath.getBackPos();
            p.y += mPressingAlt ? 20 : 1;
            mSelectionHead->mPath.setBackPos(p);
        } else {
            if(mPressingShift) {
                if(mSelectionHead) alignUp();
            }else{
                for(auto sel : mSelections) {
                    sel->moveY(mPressingAlt ? 20 : 1);
                }
            }
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
        if(mPathMode && mSelectionHead) {
            auto p = mSelectionHead->mPath.getBackPos();
            p.y -= mPressingAlt ? 20 : 1;
            mSelectionHead->mPath.setBackPos(p);
        } else {
            if(mPressingShift) {
                if(mSelectionHead) alignDown();
            }else{
                for(auto sel : mSelections) {
                    sel->moveY(mPressingAlt ? -20 : -1);
                }
            }
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
        if(mPathMode && mSelectionHead) {
            auto p = mSelectionHead->mPath.getBackPos();
            p.x -= mPressingAlt ? 20 : 1;
            mSelectionHead->mPath.setBackPos(p);
        } else {
            if(mPressingShift) {
                if(mSelectionHead) alignLeft();
            }else{
                for(auto sel : mSelections) {
                    sel->moveX(mPressingAlt ? -20 : -1);
                }
            }
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
        if(mPathMode && mSelectionHead) {
            auto p = mSelectionHead->mPath.getBackPos();
            p.x += mPressingAlt ? 20 : 1;
            mSelectionHead->mPath.setBackPos(p);
        } else {
            if(mPressingShift) {
                if(mSelectionHead) alignRight();
            }else{
                for(auto sel : mSelections) {
                    sel->moveX(mPressingAlt ? 20 : 1);
                }
            }
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_F1) {
        auto d = getScene()->getPhysicsWorld()->getDebugDrawMask();
        if(d == PhysicsWorld::DEBUGDRAW_ALL)
            getScene()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
        else
            getScene()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_P) {
        for(auto sel : mSelections) {
            sel->rotate();
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_N) {
        if(mPressingCtrl) {
            clean(true);
            createBackground();
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_O) {
        if(mPressingCtrl) {
            // open map
            MapSerial::loadMap();
        } else {
            for(auto sel : mSelections) {
                if(sel->mButton) {
                    sel->mButton->rotateDir();
                }
            }
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_S) {
        if(mPressingCtrl) {
            // save map
            if(!mCurFileName.empty())
                MapSerial::saveMap(mCurFileName.c_str());
            else
                MapSerial::saveMap();
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_F5) {
        MapSerial::loadMap(std::string(mCurFileName).c_str());
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_F4) {
        MapSerial::saveRemoteMaps();
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_G) {
        mShowGrid = !mShowGrid;
        
        // also show or hide ID labels
        for(auto b : mBlocks) {
            b.second->mIDLabel->setVisible(mShowGrid);
            b.second->mShowIDLabel = mShowGrid;
        }
    }
    
    // path mode
    if (keyCode == EventKeyboard::KeyCode::KEY_F && mSelectionHead && !mGameMode) {
        // only work for one selection
        mPathMode = true;
        
        if(mSelectionHead->mPath.empty()) {
            auto pos = mSelectionHead->getPosition();
            mSelectionHead->mPath.push(pos);
            mSelectionHead->mPath.push(pos + (mPressingShift ? Vec2(0,50): Vec2(50,0)));
        } else {
            auto pos = mSelectionHead->mPath.getBackPos();
            mSelectionHead->mPath.push(pos + (mPressingShift ? Vec2(0,50): Vec2(50,0)));
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
        // only work for one selection
        if(mPathMode) {
            mPathMode = false;
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_SPACE) {
        jump();
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_RETURN ||
        keyCode == EventKeyboard::KeyCode::KEY_ENTER ||
        keyCode == EventKeyboard::KeyCode::KEY_L) {
        enableGame(!mGameMode);
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_M) {
        mPressingM = true;
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_DELETE ||
        keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
        if(mPathMode && mSelectionHead) {
            mSelectionHead->mPath.pop();
            if(mSelectionHead->mPath.empty()) {
                mPathMode = false;
            }
        } else {
            for(auto sel : mSelections) {

                auto it = mBlocks.begin();
                for (; it != mBlocks.end(); ++it )
                    if (it->second == sel)
                        break;
                
                if (it != mBlocks.end()) {
                    auto tableit = mBlockTable.find(sel->getSprite());
                    mBlockTable.erase(tableit);
                    if(sel == mSelectionHead)
                        mSelectionHead = nullptr;
                    delete sel;
                    mBlocks.erase(it);
                }
            }
        }
        mSelections.clear();
    }
    
    if (mGameMode) {
        if (keyCode == EventKeyboard::KeyCode::KEY_A)
            mMoveLeft = true;
        if (keyCode == EventKeyboard::KeyCode::KEY_D)
            mMoveRight = true;
    }
}

void GameScene::keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
    if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT) {
        mPressingShift = false;
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_CTRL) {
        mPressingCtrl = false;
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_ALT) {
        mPressingAlt = false;
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_M) {
        mPressingM = false;
    }
    
    if (mGameMode) {
        if (keyCode == EventKeyboard::KeyCode::KEY_A)
            mMoveLeft = false;
        if (keyCode == EventKeyboard::KeyCode::KEY_D)
            mMoveRight = false;
    }
}

BlockBase* GameScene::findBlock(int id) {
    auto it = mBlocks.find(id);
    if(it != mBlocks.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

void GameScene::createBackground() {
    
    auto width = VisibleRect::right().x;
    auto height = VisibleRect::top().y;
    int frameSize = 20;
    {
        BlockBase* block = new BlockBase();
        block->mCanPickup = false;
        block->create(Rect(0,0,width,frameSize));
        block->addToScene(this);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
    }
    {
        BlockBase* block = new BlockBase();
        block->mCanPickup = false;
        block->create(Rect(0,height-frameSize,width,frameSize));
        block->addToScene(this);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
    }
    {
        BlockBase* block = new BlockBase();
        block->mCanPickup = false;
        block->create(Rect(0,0,frameSize,height));
        block->addToScene(this);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
    }
    {
        BlockBase* block = new BlockBase();
        block->mCanPickup = false;
        block->create(Rect(width-frameSize,0,frameSize,height));
        block->addToScene(this);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
    }
}

void GameScene::enableGame(bool val) {
    if( mGameMode == val ) return;
    mGameMode = val;
    
    mSpawnPoint->setVisible(!mGameMode);
    mHero->setVisible(mGameMode);
    mHero->setPosition(mSpawnPoint->getPosition());
    
    for(auto bc : mBlocks) {
        auto b = bc.second;
        if(val) {
            b->mPath.reset();
        }
        b->mPath.mDisable = !val;
        b->mPath.mHelperNode->setVisible(!val);
        if(b->mButton) {
            b->mButton->showHelper(!val);
        }
        
        b->resetOpenClose();
    }
    
    if(!val) {
        die();
    }
    mHero->getSprite()->getPhysicsBody()->setGravityEnable(val);
}

void GameScene::updateGame(float dt){
    mHero->getSprite()->getPhysicsBody();
    
    float speed = mHero->mPushing ? 80 : 200;
    if(mMoveLeft){
        mHero->moveX(dt * -speed);
    } else if(mMoveRight){
        mHero->moveX(dt * speed);
    }
    
    mHero->mPushing = false;
}

void GameScene::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
    Layer::draw(renderer, transform, flags);
    
    static CustomCommand _customCommand;
    if(mShowGrid) {
        _customCommand.init(400);
        _customCommand.func = CC_CALLBACK_0(GameScene::onDrawPrimitive, this, transform, flags);
        renderer->addCommand(&_customCommand);
    }
}

void GameScene::onDrawPrimitive(const Mat4 &transform, uint32_t flags) {
    Director* director = Director::getInstance();
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
    
    auto size = mHero->getBoundingBox().size;
    
    DrawPrimitives::setDrawColor4B(200,200,200,255);
    
    float y = 0;
    while(y < VisibleRect::top().y) {
        
        DrawPrimitives::drawLine( Vec2(0, y), Vec2(VisibleRect::right().x, y) );
        y += size.height;
    }
    
    float x = 0;
    while(x < VisibleRect::right().x+50) {
        
        DrawPrimitives::drawLine( Vec2(x, 0), Vec2(x, VisibleRect::top().y) );
        x += size.width;
    }
    
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void GameScene::jump(){
    if(mHero->mCanJump) {
        mHero->getSprite()->getPhysicsBody()->applyImpulse(Vec2(0,400));
        mHero->mCanJump = false;
    }
}

void GameScene::die() {
    auto p = mSpawnPoint->getPosition();
    mHero->getSprite()->getPhysicsBody()->resetForces();
    mHero->getSprite()->getPhysicsBody()->setVelocity(Vec2(0,0));
    mHero->setPosition(p);
}

void GameScene::postUpdate(float dt) {
    for(auto b : mBlocks) {
        b.second->postUpdate(dt);
    }
}

void GameScene::update(float dt){
    if(mGameMode) {
        updateGame(dt);
    }
    
    for(auto b : mBlocks) {
        b.second->update(dt);
    }
    
    auto b = mBlocks;
    b[mHero->mID] = mHero;
    mShadows->update(b);
}

void GameScene::setKind(int kind) {
    if(kind >= KIND_MAX) return;
    
    for(auto sel : mSelections) {
        
        sel->setKind(BlockKind(kind));
    }
}

void GameScene::duplicate() {
    std::set<BlockBase*> duplicated;
    
    for(auto sel : mSelections) {
        BlockBase* block = new BlockBase();
        auto pos = sel->getPosition();
        pos.x += 30;
        pos.y += 30;
        auto size = sel->getBoundingBox().size;
        block->create(pos, size);
        block->addToScene(this);
        block->setKind(sel->mKind);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
        
        duplicated.insert(block);
    }
    
    for(auto sel : mSelections) {
        sel->getSprite()->setColor(sel->getColor());
    }
    
    mSelections.clear();
    
    mSelections = duplicated;
    for(auto sel : mSelections) {
        sel->switchToSelectionImage();
    }
}

void GameScene::setBackgroundColor(const cocos2d::Color3B& color) {
    mBackgroundColor = color;
    getChildByTag(1000)->setColor(mBackgroundColor);
}

void GameScene::alignLeft() {
    auto p = mSelectionHead->getPosition();
    auto w = mSelectionHead->getBoundingBox().size.width;
    float mostLeft = p.x - w/2;
    
    for(auto sel : mSelections) {
        if(sel != mSelectionHead) {
            auto selWid = sel->getBoundingBox().size.width;
            sel->setPositionX(mostLeft + selWid/2);
        }
    }
}

void GameScene::alignRight() {
    auto p = mSelectionHead->getPosition();
    auto w = mSelectionHead->getBoundingBox().size.width;
    float mostRight = p.x + w/2;
    
    for(auto sel : mSelections) {
        if(sel != mSelectionHead) {
            auto selWid = sel->getBoundingBox().size.width;
            sel->setPositionX(mostRight - selWid/2);
        }
    }
}

void GameScene::alignUp() {
    auto p = mSelectionHead->getPosition();
    auto h = mSelectionHead->getBoundingBox().size.height;
    float mostUp = p.y + h/2;
    
    for(auto sel : mSelections) {
        if(sel != mSelectionHead) {
            auto selHei = sel->getBoundingBox().size.height;
            sel->setPositionY(mostUp - selHei/2);
        }
    }
}

void GameScene::alignDown() {
    auto p = mSelectionHead->getPosition();
    auto h = mSelectionHead->getBoundingBox().size.height;
    float mostDown = p.y - h/2;
    
    for(auto sel : mSelections) {
        if(sel != mSelectionHead) {
            auto selHei = sel->getBoundingBox().size.height;
            sel->setPositionY(mostDown + selHei/2);
        }
    }
}

void GameScene::clean(bool save) {
    
    if(save && !mCurFileName.empty())
        MapSerial::saveMap(mCurFileName.c_str());
    
    mSelectionHead = nullptr;
    mSelections.clear();
    
    for (auto b : mBlocks) {
        delete b.second;
    }
    mBlocks.clear();
    
    enableGame(false);
    BlockBase::mIDCounter = 1;
    
    mCurFileName = "";
    UILayer::Layer->setFileName("untitled");
}

///////////////

void Button::reset() {
    mPushing = false;
    mPushedEventCalled = false ;
    mRestoredEventCalled = false ;
    
    callRestoreEvent();
    
#ifdef EDITOR_MODE
    updateHelper();
#endif
}

bool Button::push(const cocos2d::Vec2& normal, BlockBase* hero) {
    if(!mEnable) return true;
    
    if(normal.y > 0.9 && mDir == DIR_DOWN) {
        mPushing = true;
    } else if(normal.y < -0.9 && mDir == DIR_UP) {
        mPushing = true;
    } else if(normal.x > 0.9  && mDir == DIR_LEFT) {
        mPushing = true;
    } else if(normal.x < -0.9 && mDir == DIR_RIGHT) {
        mPushing = true;
    }
    
    if(mPushing) {
        auto pos = mParent->getPosition();
        auto size = mParent->getBoundingBox().size;
        
        auto posHero = hero->getPosition();
        auto sizeHero = hero->getBoundingBox().size;
        
        float length = 0;
        
        if(mDir == DIR_UP) {
            length = (posHero.y + sizeHero.height/2) - (pos.y - size.height/2);
        }else if(mDir == DIR_DOWN){
            length = (pos.y + size.height/2) - (posHero.y - sizeHero.height/2);
        }else if(mDir == DIR_LEFT){
            length = (pos.x + size.width/2) - (posHero.x - sizeHero.width/2);
        }else if(mDir == DIR_RIGHT){
            length = (posHero.x + sizeHero.width/2) - (pos.x - size.width/2);
        }
        
        if(length < 0) {
            mPushing = false;
            return mDir == DIR_UP ? true : false;
        }
        
        float newLength = 0;
        
        if(mDir == DIR_LEFT) {
            newLength = std::max(mParent->getWidth() - length, 1.0f);
            setParentWidth(newLength);
            float leftBound = mParent->mRestorePosition.x - mParent->mRestoreSize.width/2;
            mParent->setPositionX(leftBound + newLength / 2);
        } else if(mDir == DIR_RIGHT) {
            newLength = std::max(mParent->getWidth() - length, 1.0f);
            setParentWidth(newLength);
            float rightBound = mParent->mRestorePosition.x + mParent->mRestoreSize.width/2;
            mParent->setPositionX(rightBound - newLength / 2);
        } else if(mDir == DIR_UP) {
            newLength = std::max(mParent->getThickness() - length, 1.0f);
            setParentHeight(newLength);
            float upBound = mParent->mRestorePosition.y + mParent->mRestoreSize.height/2;
            mParent->setPositionY(upBound - newLength / 2);
        } else if(mDir == DIR_DOWN) {
            newLength = std::max(mParent->getThickness() - length, 1.0f);
            setParentHeight(newLength);
            float downBound = mParent->mRestorePosition.y - mParent->mRestoreSize.height/2;
            mParent->setPositionY(downBound + newLength / 2);
        }
        
        bool callpushing = false;
        if( mDir == DIR_LEFT || mDir == DIR_RIGHT ){
            callpushing = newLength < mParent->mRestoreSize.width / 2;
        } else if( mDir == DIR_UP || mDir == DIR_DOWN ) {
            callpushing = newLength < mParent->mRestoreSize.height / 2;
        }

        if( callpushing ) {
            callPushingEvent();
            
            mRestoredEventCalled = false;
            if(!mPushedEventCalled) {
                callPushEvent();
            }
        }
        
        if( newLength < 1 ) {
            mEnable = false;
        }
        return true;
    } else {
        return mDir == DIR_UP ? true : false;
    }
}

void Button::rotateDir() {
    static PushDir dirs[] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    auto id = (int)mDir;
    mDir = dirs[++id % 4];
    updateHelper();
}

void Button::callPushEvent() {
    mPushedEventCalled = true;
    if(mPushedEvent.empty()) return;
    
    Events::callEvent(mPushedEvent.c_str());
}

void Button::callRestoreEvent() {
    mRestoredEventCalled = true;
    if(mRestoredEvent.empty()) return;
    
    Events::callEvent(mRestoredEvent.c_str());
}

void Button::callPushingEvent() {
    if(mPushingEvent.empty()) return;
    
    Events::callEvent(mPushingEvent.c_str());
}

void Button::update(float dt) {
    if(!mPushing && mCanRestore) {
        // restore
        auto size = mParent->getBoundingBox().size;
        if(mDir == DIR_LEFT || mDir == DIR_RIGHT) {
            if(size.width >= mParent->mRestoreSize.width) {
                return;
            }
        } if(mDir == DIR_UP || mDir == DIR_DOWN) {
            if(size.height >= mParent->mRestoreSize.height) {
                return;
            }
        }
        
        mParent->getSprite()->getPhysicsBody()->setEnable(true);
        
        float length = dt * 100;
        float newLength = 0;
        if(mDir == DIR_LEFT) {
            newLength = std::min(mParent->getWidth() + length, mParent->mRestoreSize.width);
            setParentWidth(newLength);
            float leftBound = mParent->mRestorePosition.x - mParent->mRestoreSize.width/2;
            mParent->setPositionX(leftBound + newLength / 2);
        } else if(mDir == DIR_RIGHT) {
            newLength = std::min(mParent->getWidth() + length, mParent->mRestoreSize.width);
            setParentWidth(newLength);
            float rightBound = mParent->mRestorePosition.x + mParent->mRestoreSize.width/2;
            mParent->setPositionX(rightBound - newLength / 2);
        } else if(mDir == DIR_UP) {
            newLength = std::min(mParent->getThickness() + length, mParent->mRestoreSize.height);
            setParentHeight(newLength);
            float upBound = mParent->mRestorePosition.y + mParent->mRestoreSize.height/2;
            mParent->setPositionY(upBound - newLength / 2);
        } else if(mDir == DIR_DOWN) {
            newLength = std::min(mParent->getThickness() + length, mParent->mRestoreSize.height);
            setParentHeight(newLength);
            float downBound = mParent->mRestorePosition.y - mParent->mRestoreSize.height/2;
            mParent->setPositionY(downBound + newLength / 2);
        }
        
        bool callrestore = false;
        if( mDir == DIR_LEFT || mDir == DIR_RIGHT ){
            callrestore = newLength >= mParent->mRestoreSize.width / 2;
        } else if( mDir == DIR_UP || mDir == DIR_DOWN ) {
            callrestore = newLength >= mParent->mRestoreSize.height / 2;
        }
        
        if( callrestore ) {
            mPushedEventCalled = false;
            if(!mRestoredEventCalled) {
                callRestoreEvent();
            }
        }

        if(newLength > 0.05) {
            mEnable = true;
        }
    } else {
        mPushing = false;
    }
}

void Button::setParentWidth(float v) {
    if(mParent->getSprite()->getRotation() > 1)
        mParent->setHeight(v);
    else
        mParent->setWidth(v);
}

void Button::setParentHeight(float v) {
    if(mParent->getSprite()->getRotation() > 1)
        mParent->setWidth(v);
    else
        mParent->setHeight(v);
}

void Button::updatePosition() {
#ifdef EDITOR_MODE
    auto p = mParent->getPosition();
    mHelperNode->setPosition(p);
#endif
}

Button::Button(BlockBase* parent) : mParent(parent) {
    //mRestoreSize = mParent->getBoundingBox().size;
    
#ifdef EDITOR_MODE
    mHelperNode = DrawNode::create();
    mParent->getSprite()->getParent()->addChild(mHelperNode,1000);
    updateHelper();
#endif
}

Button::~Button() {
#ifdef EDITOR_MODE
    mHelperNode->removeFromParentAndCleanup(true);
#endif
}

#ifdef EDITOR_MODE
void Button::updateHelper() {
    Color4F color(123/255.,221/255.,45/255.,1.0);
    auto p = mParent->getPosition();
    auto b = mParent->getBoundingBox();
    float arrawH = 20;
    float arrawW = 5;
    mHelperNode->clear();
    if(mDir == DIR_UP) {
        mHelperNode->drawTriangle(Vec2(0, -b.size.height/2),
                                  Vec2(-arrawW, -b.size.height/2 - arrawH),
                                  Vec2( arrawW, -b.size.height/2 - arrawH),
                                  color);
    } else if(mDir == DIR_DOWN) {
        mHelperNode->drawTriangle(Vec2(0, b.size.height/2),
                                  Vec2(-arrawW, b.size.height/2 + arrawH),
                                  Vec2( arrawW, b.size.height/2 + arrawH),
                                  color);
    } else if(mDir == DIR_RIGHT) {
        mHelperNode->drawTriangle(Vec2(-b.size.width/2, 0),
                                  Vec2(-b.size.width/2 - arrawH, -arrawW),
                                  Vec2(-b.size.width/2 - arrawH,  arrawW),
                                  color);
    } else if(mDir == DIR_LEFT) {
        mHelperNode->drawTriangle(Vec2(b.size.width/2, 0),
                                  Vec2(b.size.width/2 + arrawH, -arrawW),
                                  Vec2(b.size.width/2 + arrawH,  arrawW),
                                  color);
    }
    mHelperNode->setPosition(p);
}
#endif

