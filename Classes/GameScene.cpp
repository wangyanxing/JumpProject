#include "GameScene.h"
#include "VisibleRect.h"
#include "GameUtils.h"
#include "MapSerial.h"
#include "UILayer.h"
#include "Events.h"
#include "Shadows.h"
#include "cocos-ext.h"
//#include "extensions/GUI/CCControlColourPicker.h"

#if 0
#include "fx/ens2DSoftShadowNode.h"
using namespace ens;
#endif

#include <iostream>

USING_NS_CC;
USING_NS_CC_EXT;

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
    getScheduler()->scheduleUpdate(&mPostUpdater, 100, false);
    
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
    
#if 0
    auto mShadowRoot = new C2DSoftShadowRoot();
    //mShadowRoot->autorelease();
    mShadowRoot->init();
    mShadowRoot->setIsDrawDebug(false);
    mShadowRoot->setShadowDarkness(0.15);
    addChild(mShadowRoot, 3);
    
    auto mLight=new ClightNode();
    mLight->autorelease();
    //mLight->setIsDrawDebug(true);
    mLight->init(10);
    mShadowRoot->setLight(mLight);
    mLight->setPosition(800,400);
    
    {
        auto m_shadowObj=new C2DSoftShadowObj();
        m_shadowObj->autorelease();
        m_shadowObj->init(makeRectPolygon(20,20));
        m_shadowObj->setLight(mLight);
        //m_shadowObj->setIsDrawDebug(true);
        mShadowRoot->addObj(m_shadowObj);
        m_shadowObj->setPosition(350,200);
    }
    {
        auto m_shadowObj=new C2DSoftShadowObj();
        m_shadowObj->autorelease();
        m_shadowObj->init(makeRectPolygon(20,20));
        m_shadowObj->setLight(mLight);
        //m_shadowObj->setIsDrawDebug(true);
        mShadowRoot->addObj(m_shadowObj);
        m_shadowObj->setPosition(420,200);
    }
#endif
    
    mSpawnPoint = Sprite::create("images/cross.png");
    addChild(mSpawnPoint, 100);
    //mSpawnPoint->setPosition(VisibleRect::center());
    mSpawnPoint->setPosition(50,100);
    mSpawnPoint->setScale(0.3);
    
    mHero = new Hero();
    //mHero->create(VisibleRect::center());
    mHero->create(Vec2(32,100));
    mHero->setSize(Size(25,25));
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
        if(thisBlock == mHero)
            GameScene::Scene->mDeadFlag = true;
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
        
        if(pushObject == mHero) {
            if(!mHero->mCanJump) {
            }
            mHero->mCanJump = true;
        }
        
        auto h = pushedObject->getBoundingBox().size.height/2 + pushObject->getBoundingBox().size.height/2;
        if(onMovingPlatform && pushObject == mHero) {
            h -= 1;
        }
        
        if(normal.y < -0.9){
            if(onButton) h -= 1;
            else h += 1;
            pushObject->setPositionY(pushedObject->getSprite()->getPositionY() - h);
        }else{
            pushObject->setPositionY(pushedObject->getSprite()->getPositionY() + h);
            
            if(pushObject->mKind == KIND_PUSHABLE || pushObject->mKind == KIND_HERO) {
                pushObject->getSprite()->getPhysicsBody()->resetForces();
                pushObject->getSprite()->getPhysicsBody()->setVelocityLimit(0);
            }
        }
        
        auto v = pushObject->getSprite()->getPhysicsBody()->getVelocity();
        pushObject->getSprite()->getPhysicsBody()->setVelocity(Vec2(v.x, 0));
    }
    
    pushObject->getSprite()->getPhysicsBody()->getFirstShape()->_forceUpdateShape();
    
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
    
    if (keyCode == EventKeyboard::KeyCode::KEY_J) {
        group();
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
                
                // find it in groups
                for(auto& i : mGroups) {
                    auto ii = std::find(i.second.begin(), i.second.end(), sel);
                    if(ii != i.second.end()) {
                        i.second.erase(ii);
                    }
                }
                
                auto itg = mGroups.find(sel);
                if(itg != mGroups.end()) {
                    mGroups.erase(itg);
                }
                
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

void GameScene::enableGame(bool val, bool force) {
    if( mGameMode == val && !force) return;
    mGameMode = val;
    
    mMoveLeft = false;
    mMoveRight = false;
    
    mSpawnPoint->setVisible(!mGameMode);
    mHero->setVisible(mGameMode);
    mHero->mCanJump = false;
    mHero->setPosition(mSpawnPoint->getPosition());
    mHero->getSprite()->getPhysicsBody()->setVelocityLimit(1000);
    
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
        
        b->reset();
    }
    
    die();
    mHero->getSprite()->getPhysicsBody()->setGravityEnable(val);
}

void GameScene::updateGame(float dt){
    if(mDeadFlag) {
        die();
        return;
    }
    
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
    _customCommand.init(400);
    _customCommand.func = CC_CALLBACK_0(GameScene::onDrawPrimitive, this, transform, flags);
    renderer->addCommand(&_customCommand);
}

void GameScene::onDrawPrimitive(const Mat4 &transform, uint32_t flags) {
    Director* director = Director::getInstance();
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
    
    if(mShowGrid) {
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
    }
    
    if(!mGameMode) {
        DrawPrimitives::setDrawColor4B(17,47,245,255);
        for(auto g : mGroups){
            auto head = g.first;
            
            for(auto m : g.second) {
                DrawPrimitives::drawSolidCircle(m->getPosition(), 3, 0, 20, 1, 1);
                DrawPrimitives::drawLine(head->getPosition(), m->getPosition());
            }
        }
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
    mDeadFlag = false;
}

void GameScene::postUpdate(float dt) {
    for(auto b : mBlocks) {
        b.second->postUpdate(dt);
    }
    mHero->postUpdate(dt);
    
    auto nb = mBlocks;
    nb[mHero->mID] = mHero;
    mShadows->update(nb);
}

void GameScene::update(float dt){
    
    for(auto b : mBlocks) {
        b.second->update(dt);
    }
    
    if(mGameMode) {
        updateGame(dt);
    }
}

void GameScene::setKind(int kind) {
    if(kind >= KIND_MAX) return;
    
    for(auto sel : mSelections) {
        
        sel->setKind(BlockKind(kind));
    }
}

void GameScene::duplicate() {
    std::set<BlockBase*> duplicated;
    
    Vec2 bias(20,20);
    
    for(auto sel : mSelections) {
        
        BlockBase* block = new BlockBase();
        auto pos = sel->getPosition() + bias;
        auto size = sel->getBoundingBox().size;
        block->create(pos, size);
        block->addToScene(this);
        block->setKind(sel->mKind);
        block->mPath.cloneFrom(sel->mPath,bias);
        
        block->mRestoreSize = block->getBoundingBox().size;
        block->mRestorePosition = block->getPosition();
        
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

void GameScene::group() {
    if(mSelections.empty() || !mSelectionHead) {
        return;
    }
    
    auto it = mGroups.find(mSelectionHead);
    if(it != mGroups.end()) {
        for(auto s : it->second) {
            s->reset();
        }
        mGroups.erase(it);
        UILayer::Layer->addMessage("Ungroup");
    } else  {
        mGroups[mSelectionHead].clear();
        for(auto s : mSelections) {
            if(s == mSelectionHead) continue;
            mGroups[mSelectionHead].push_back(s);
        }
        
        UILayer::Layer->addMessage("Group");
    }
}

void GameScene::clean(bool save) {

    if(save && !mCurFileName.empty())
        MapSerial::saveMap(mCurFileName.c_str());
    
    mSelectionHead = nullptr;
    mSelections.clear();
    
    mGroups.clear();
    
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

