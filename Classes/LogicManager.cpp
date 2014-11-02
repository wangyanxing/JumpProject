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
#include "Shake.h"
#include "VisibleRect.h"

#if EDITOR_MODE
#   include "EditorScene.h"
#else
#   include "GameScene.h"
#endif

#define GRADIENT 1
#define DIE_FX_TAG 1001

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

	mPalette.insert(std::pair<int, Color3B>(1, Color3B(0xFF, 0xFF, 0xFF)));
	mPalette.insert(std::pair<int, Color3B>(2, Color3B(0xFF, 0x00, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(3, Color3B(0x00, 0xFF, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(4, Color3B(0x00, 0x00, 0xFF)));

	mPalette.insert(std::pair<int, Color3B>(5, Color3B(0xCC, 0xCC, 0xCC)));
	mPalette.insert(std::pair<int, Color3B>(6, Color3B(0xCC, 0x00, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(7, Color3B(0x00, 0xCC, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(8, Color3B(0x00, 0x00, 0xCC)));

	mPalette.insert(std::pair<int, Color3B>(9, Color3B(0x99, 0x99, 0x99)));
	mPalette.insert(std::pair<int, Color3B>(10, Color3B(0x99, 0x00, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(11, Color3B(0x00, 0x99, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(12, Color3B(0x00, 0x00, 0x99)));

	mPalette.insert(std::pair<int, Color3B>(13, Color3B(0x66, 0x66, 0x66)));
	mPalette.insert(std::pair<int, Color3B>(14, Color3B(0x66, 0x00, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(15, Color3B(0x00, 0x66, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(16, Color3B(0x00, 0x00, 0x66)));

	mPalette.insert(std::pair<int, Color3B>(17, Color3B(0x33, 0x33, 0x33)));
	mPalette.insert(std::pair<int, Color3B>(18, Color3B(0x33, 0x00, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(19, Color3B(0x00, 0x33, 0x00)));
	mPalette.insert(std::pair<int, Color3B>(20, Color3B(0x00, 0x00, 0x33)));
    
    mSpawnPos = VisibleRect::center();

    mHero = new Hero();
    mHero->create(VisibleRect::center());
    mHero->setSize(Size(30,30));
    mHero->setVisible(false);
    mHero->setKind(KIND_HERO);
    mHeroShape = mHero->getSprite()->getPhysicsBody()->getShapes().front();
    mHero->addToScene(mParentLayer);
    mBlockTable[mHero->getSprite()] = mHero;
    
    mShadows = new ShadowManager(mParentLayer);
    
    // background
#if GRADIENT
    mBack = GameUtils::createRect(VisibleRect::getVisibleRect(), Color3B(255,255,255));
    
    #if EDITOR_MODE
    auto shaderfile = FileUtils::getInstance()->fullPathForFilename("shaders/back_editor.fsh");
    #else
    auto shaderfile = FileUtils::getInstance()->fullPathForFilename("shaders/back.fsh");
    #endif
    // init shader
    GLchar * fragSource = (GLchar*)String::createWithContentsOfFile(shaderfile.c_str())->getCString();
    auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource);
    auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);
    
    float screenWidth = VisibleRect::getFrameSize().width;
    float screenHeight = VisibleRect::getFrameSize().height;
    
    mGradientColorSrc = Color3B(50,201,219);
    mGradientColorDst = Color3B(30,181,199);
    
    mBack->setGLProgramState(glProgramState);
    glProgramState->setUniformVec4("data", Vec4(screenWidth, screenHeight, 0, 0));
    //glProgramState->setUniformVec4("color", Vec4(251.0/255.0, 3.0/255.0,137.0/255.0, 0.4));
    //glProgramState->setUniformVec4("colorDest", Vec4(173.0/255.0, 3.0/255.0, 58.0/255.0, 0));
    glProgramState->setUniformVec4("color", Vec4(50.0/255.0, 201.0/255.0,219.0/255.0, 0.4));
    glProgramState->setUniformVec4("colorDest", Vec4(30.0/255.0, 181.0/255.0,199.0/255.0, 0.4));
    
#else
    mBack = GameUtils::createRect(VisibleRect::getVisibleRect(), Color3B(30,181,199));
#endif
    mParentLayer->addChild(mBack, 0);

#if EDITOR_MODE
    createFixedBlocks();
#endif
    
    enableGame(false);
}

Node* GameLogic::createParticle(const Vec2& pos) {
    
    auto parent = Node::create();
    auto pg = Sprite::create("images/glow.png");
    auto p = Sprite::create("images/particle.png");
    
    float fadeTime = 10 + rand()%10;
    fadeTime /= 10;
    
    pg->runAction(RepeatForever::create(Sequence::create(FadeOut::create(fadeTime),FadeIn::create(fadeTime), NULL)));
    p->setScale(0.5);
    
    parent->addChild(pg, 0);
    parent->addChild(p, 1);
    
    float scale = 3 + rand()%2;
    scale /= 10;
    
    parent->setScale(scale);
    
    mParentLayer->addChild(parent, 15);
    
    parent->setPosition(pos);
    
    return parent;
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
        if (thisBlock == mHero){
            //mDeadFlag = true;
            otherBlock->callTriggerEvent();
        }
            
        return false;
    } else if( otherBlock->mKind == KIND_BLOCK ) {
        if(normal.y > 0.9) {
            auto p = pushObject->getSprite()->getPhysicsBody()->getPosition();
            if(otherBlock->mMovementThisFrame != Vec2::ZERO) {
                p += otherBlock->mMovementThisFrame;
                onMovingPlatform = true;
                pushObject->setPosition(p);
            }
        }
    } else if( otherBlock->mKind == KIND_BUTTON ) {
        
        if(otherBlock->mButton->push(normal, pushObject))
            return false;
        
        if(otherBlock->mButton->mDir == Button::DIR_UP)
            onButton = true;
        
        if(!blockA->canPush() && !blockB->canPush())
            return false;
        
        if(normal.y > 0.9) {
            auto p = pushObject->getSprite()->getPhysicsBody()->getPosition();
            if(otherBlock->mMovementThisFrame != Vec2::ZERO) {
                p += otherBlock->mMovementThisFrame;
                onMovingPlatform = true;
                pushObject->setPosition(p);
            }
        }
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
        
        auto h = pushedObject->getSize().width/2 + pushObject->getSize().width/2;
        if(onMovingPlatform)
            h += 1;
        auto phyPos = pushObject->getSprite()->getPhysicsBody()->getPosition();
        if (pushObject->getPosition().x < pushedObject->getSprite()->getPositionX())
            pushObject->setPosition(pushedObject->getSprite()->getPositionX() - h, phyPos.y);
        else
            pushObject->setPosition(pushedObject->getSprite()->getPositionX() + h, phyPos.y);
    }
    
    if(normal.y > 0.9 || normal.y < -0.9) {
        
        if(pushObject == mHero && normal.y > 0.9) {
            mHero->mCanJump = true;
        }
        
        auto h = pushedObject->getSize().height/2 + pushObject->getSize().height/2;
        if(onMovingPlatform && pushObject == mHero) {
            h -= 1;
        }
        
        if(normal.y < -0.9){
            if(onButton) h -= 1;
            else h += 1;
            pushObject->setPositionY(pushedObject->getSprite()->getPhysicsBody()->getPosition().y - h);
        }else{
            pushObject->setPositionY(pushedObject->getSprite()->getPhysicsBody()->getPosition().y + h);
            
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

void GameLogic::setBackGradientColor(const cocos2d::Color3B& colorSrc,
                                     const cocos2d::Color3B& colorDst) {
    mGradientColorDst = colorDst;
    mGradientColorSrc = colorSrc;
    
    auto glProgramState = mBack->getGLProgramState();
    glProgramState->setUniformVec4("color", Vec4(mGradientColorSrc.r/255.0,
                                                 mGradientColorSrc.g/255.0,
                                                 mGradientColorSrc.b/255.0, 0.4));
    glProgramState->setUniformVec4("colorDest", Vec4(mGradientColorDst.r/255.0,
                                                 mGradientColorDst.g/255.0,
                                                 mGradientColorDst.b/255.0, 0.4));
    
    mShadows->updateShaderParam();
}

void GameLogic::setBackGradientCenter(const cocos2d::Vec2& pos) {
    Vec2 p = pos;
    p.x -= VisibleRect::center().x;
    p.x *= -1;
    p.x /= VisibleRect::center().x;
    
    p.y -= VisibleRect::center().y;
    p.y *= -1;
    p.y /= VisibleRect::center().x;
    
    float screenWidth = VisibleRect::getFrameSize().width;
    float screenHeight = VisibleRect::getFrameSize().height;
    
    mGradientCenter = pos;
    mBack->getGLProgramState()->setUniformVec4("data",
                                               Vec4(screenWidth, screenHeight, p.x, p.y));
    
    mShadows->updateShaderParam();
}

void GameLogic::showGameScene(bool val) {
    mBack->setVisible(val);
}

void GameLogic::createFixedBlocks() {
    auto width = VisibleRect::right().x;
    auto height = VisibleRect::top().y;
    int frameSize = 10;
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
    if(mHero->mCanJump && !mRejectInput) {
        mHero->getSprite()->getPhysicsBody()->applyImpulse(Vec2(0,325));
        float scale = mHero->getSprite()->getScaleX();
        mHero->getSprite()->runAction(Sequence::create(ScaleTo::create(0.2,scale*0.8,scale*1.2),ScaleTo::create(0.2,scale,scale), NULL));
        mHero->mCanJump = false;
    }
}

void GameLogic::win() {
    mWinFlag = false;
    
    // trigger a win-game callback
    if(mWinGameEvent) {
        mWinGameEvent();
    }
}

void GameLogic::die() {
    if(mParentLayer->getChildByTag(DIE_FX_TAG)) {
        mParentLayer->removeChildByTag(DIE_FX_TAG);
    }
    
    mHero->getSprite()->getPhysicsBody()->resetForces();
    mHero->getSprite()->getPhysicsBody()->setVelocity(Vec2(0,0));
    mHero->setPosition(mSpawnPos);
    mHero->getSprite()->setOpacity(255);
    mDeadFlag = false;
}

void GameLogic::postUpdate(float dt) {
    for(auto b : mBlocks) {
        b.second->postUpdate(dt);
    }
    mHero->postUpdate(dt);
    
    mShadows->update(dt);
}

void GameLogic::updateGame(float dt){
    if(mDeadFlag && !mRejectInput) {
        // play dead effect
        ParticleSystem* m_emitter0 = ParticleSystemQuad::create("fx/diefx.plist");
        ParticleBatchNode *batch0 = ParticleBatchNode::createWithTexture(m_emitter0->getTexture());
        batch0->addChild(m_emitter0);
        batch0->setPosition(mHero->getPosition());
        mParentLayer->addChild(batch0,15,DIE_FX_TAG);
        
        //mParentLayer->runAction(CCShake::create(0.3, 3));
        
        mRejectInput = true;
        mHero->getSprite()->runAction(ScaleTo::create(0.2,0.1,0.1));
        mParentLayer->runAction(Sequence::create(DelayTime::create(0.4), CallFunc::create([this]{
#if EDITOR_MODE
            EditorScene::Scene->showDieFullScreenAnim();
#else
            GameScene::Scene->showDieFullScreenAnim();
#endif
            enableGame(true,true);
        }), NULL));
        
        return;
    }
    if(mWinFlag) {
        win();
        return;
    }
    
    mGameTimer += dt;
    
    if(!mRejectInput) {
        float speed = mHero->mPushing ? 80 : 200;
        if(mMoveLeft){
            mHero->moveX(dt * -speed);
        } else if(mMoveRight){
            mHero->moveX(dt * speed);
        }
    }
    
    mHero->mPushing = false;
}

void GameLogic::update(float dt){
    
    for(auto b : mBlocks) {
        b.second->preUpdate();
    }
    
    for(auto b : mBlocks) {
        b.second->update(dt);
    }
    
    for(auto b : mBlocks) {
        b.second->updatePathMove();
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
    if( mGameMode == val && !force)
        return;
    
    if(val) {
        mRejectInput = false;
    }
    
    mGameMode = val;
    
    mMoveLeft = false;
    mMoveRight = false;
    
    mShadows->reset();
    
    die();
    
    mHero->setVisible(mGameMode);
    mHero->mCanJump = false;
    mHero->setSize(mHero->mRestoreSize);
    mHero->getSprite()->getPhysicsBody()->setVelocityLimit(1000);
    
    for(auto bc : mBlocks) {
        auto b = bc.second;
        if(val) {
            b->mPath.reset();
            b->mRotator.reset();
        }
        b->mRotator.mDisable = !val;
        b->mPath.mDisable = !val;
#if EDITOR_MODE
        b->mPath.mHelperNode->setVisible(!val);
        if(b->mButton) {
            b->mButton->showHelper(!val);
        }
#endif
        b->reset();
    }
    
    mHero->getSprite()->getPhysicsBody()->setGravityEnable(val);
    
    mGameTimer = 0;
}

void GameLogic::setBackgroundColor(const cocos2d::Color3B& color) {
    mBackgroundColor = color;
#if GRADIENT == 0
    mBack->setColor(mBackgroundColor);
#endif
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

void GameLogic::clearStars() {
    for(auto f : mStarNodes) {
        f->removeFromParent();
    }
    mStarNodes.clear();
    mStarList.clear();
}

void GameLogic::clearFx() {
    for(auto f : mFxNodes) {
        f->removeFromParent();
    }
    mFxNodes.clear();
    mFxList.clear();
}

void GameLogic::clean() {
    
    mGroups.clear();
    
    for (auto b : mBlocks) {
        delete b.second;
    }
    mBlocks.clear();
    
    clearStars();
    clearFx();
    
    enableGame(false);
    BlockBase::mIDCounter = 1;
}

void GameLogic::blockTraversal(const std::function<void(BlockBase*)>& func) {
    for (auto b : mBlocks) {
        func(b.second);
    }
}

void GameLogic::loadStarFromList() {
    for(auto f : mStarNodes) {
        f->removeFromParent();
    }
    mStarNodes.clear();
    
    for(auto p : mStarList) {
        mStarNodes.push_back(createParticle(p));
    }
}

void GameLogic::loadFxFromList() {
    
    for(auto f : mFxNodes) {
        f->removeFromParent();
    }
    mFxNodes.clear();
    
    for(auto i : mFxList) {
        ParticleSystem* m_emitter0 = ParticleSystemQuad::create(i);
        ParticleBatchNode *batch0 = ParticleBatchNode::createWithTexture(m_emitter0->getTexture());
        batch0->addChild(m_emitter0);
        mParentLayer->addChild(batch0,5);
        
        mFxNodes.push_back(batch0);
    }
}