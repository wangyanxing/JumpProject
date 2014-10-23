//
//  LogicManager.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "LogicManager.h"
#include "Blocks.h"
#include "GameUtils.h"
#include "Shadows.h"
#include "Button.h"
#include "VisibleRect.h"

USING_NS_CC;

GameLogic* GameLogic::Game = nullptr;

GameLogic::GameLogic(cocos2d::Layer* parent) {
    mParentLayer = parent;
    Game = this;
    
    // default colors
    mBlockColors[KIND_HERO] = Color3B::BLACK;
    mBlockColors[KIND_BLOCK] = Color3B::WHITE;
    mBlockColors[KIND_DEATH] = Color3B::BLACK;
    mBlockColors[KIND_DEATH_CIRCLE] = Color3B::BLACK;
    mBlockColors[KIND_BUTTON] = Color3B(254, 225, 50);
    mBlockColors[KIND_PUSHABLE] = Color3B(220, 150, 168);
    mBackgroundColor = Color3B(30, 181, 199);
    
    mHero = new Hero();
    //mHero->create(VisibleRect::center());
    mHero->create(Vec2(32,100));
    mHero->setSize(Size(25,25));
    mHero->setVisible(false);
    mHero->setKind(KIND_HERO);
    mHeroShape = mHero->getSprite()->getPhysicsBody()->getShapes().front();
    mHero->addToScene(mParentLayer);
    mBlockTable[mHero->getSprite()] = mHero;
    
    mShadows = new ShadowManager(mParentLayer);
    
    // background
    auto back = GameUtils::createRect(VisibleRect::getVisibleRect(), Color3B(30, 181, 199));
    back->setTag(1000);
    mParentLayer->addChild(back, 0);
    
    createFixedBlocks();
    
    enableGame(false);
}

GameLogic::~GameLogic() {
    clean();
    
    delete mHero;
    mHero = nullptr;
    
    delete mShadows;
    mShadows = nullptr;
}

bool GameLogic::onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve) {
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
            mDeadFlag = true;
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

void GameLogic::createFixedBlocks() {
    auto width = VisibleRect::right().x;
    auto height = VisibleRect::top().y;
    int frameSize = 20;
    {
        BlockBase* block = new BlockBase();
        block->mCanPickup = false;
        block->create(Rect(0,0,width,frameSize));
        block->addToScene(mParentLayer);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
    }
    {
        BlockBase* block = new BlockBase();
        block->mCanPickup = false;
        block->create(Rect(0,height-frameSize,width,frameSize));
        block->addToScene(mParentLayer);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
    }
    {
        BlockBase* block = new BlockBase();
        block->mCanPickup = false;
        block->create(Rect(0,0,frameSize,height));
        block->addToScene(mParentLayer);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
    }
    {
        BlockBase* block = new BlockBase();
        block->mCanPickup = false;
        block->create(Rect(width-frameSize,0,frameSize,height));
        block->addToScene(mParentLayer);
        mBlockTable[block->getSprite()] = block;
        mBlocks[block->mID] = block;
    }
}

BlockBase* GameLogic::findBlock(int id) {
    auto it = mBlocks.find(id);
    if(it != mBlocks.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}


void GameLogic::jump(){
    if(mHero->mCanJump) {
        mHero->getSprite()->getPhysicsBody()->applyImpulse(Vec2(0,400));
        mHero->mCanJump = false;
    }
}

void GameLogic::die() {

    mHero->getSprite()->getPhysicsBody()->resetForces();
    mHero->getSprite()->getPhysicsBody()->setVelocity(Vec2(0,0));
    mHero->setPosition(mSpawnPos);
    mDeadFlag = false;
}

void GameLogic::postUpdate(float dt) {
    for(auto b : mBlocks) {
        b.second->postUpdate(dt);
    }
    mHero->postUpdate(dt);
    
    auto nb = mBlocks;
    nb[mHero->mID] = mHero;
    mShadows->update(nb);
}

void GameLogic::updateGame(float dt){
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

void GameLogic::update(float dt){
    
    for(auto b : mBlocks) {
        b.second->update(dt);
    }
    
    if(mGameMode) {
        updateGame(dt);
    }
}

void GameLogic::deleteBlock(BlockBase* sel) {
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

void GameLogic::enableGame(bool val, bool force) {
    if( mGameMode == val && !force) return;
    mGameMode = val;
    
    mMoveLeft = false;
    mMoveRight = false;
    
//  mSpawnPoint->setVisible(!mGameMode);
    mHero->setVisible(mGameMode);
    mHero->mCanJump = false;
    mHero->setPosition(mSpawnPos);
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

void GameLogic::setBackgroundColor(const cocos2d::Color3B& color) {
    mBackgroundColor = color;
    mParentLayer->getChildByTag(1000)->setColor(mBackgroundColor);
}

BlockBase* GameLogic::createBlock(const cocos2d::Vec2& pos, BlockKind kind) {
    BlockBase* block = new BlockBase();
    block->create(pos);
    block->setKind(kind);
    block->addToScene(mParentLayer);
    mBlockTable[block->getSprite()] = block;
    mBlocks[block->mID] = block;
    return block;
}

void GameLogic::clean() {
    
    mGroups.clear();
    
    for (auto b : mBlocks) {
        delete b.second;
    }
    mBlocks.clear();
    
    enableGame(false);
    BlockBase::mIDCounter = 1;
}

void GameLogic::blockTraversal(const std::function<void(BlockBase*)>& func) {
    for (auto b : mBlocks) {
        func(b.second);
    }
}