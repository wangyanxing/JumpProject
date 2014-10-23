//
//  LogicManager.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__LogicManager__
#define __JumpEdt__LogicManager__

#include "cocos2d.h"
#include "Defines.h"

class BlockBase;
class Hero;
class ShadowManager;

class GameLogic {
public:
    
    static GameLogic* Game;
    
    GameLogic(cocos2d::Layer* parent);
    
    ~GameLogic();
    
    void createFixedBlocks();
    
    bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);
    
    void update(float dt);
    
    void clean();
    
    void enableGame(bool val, bool force = false);
    
    void postUpdate(float dt);
    
    void updateGame(float dt);
    
    BlockBase* findBlock(int id);
    
    void jump();
    
    void die();
    
public:
    
    cocos2d::Layer* mParentLayer{ nullptr };
    
    std::map<BlockBase*,std::vector<BlockBase*>> mGroups;
    
    Hero* mHero{ nullptr };
    
    cocos2d::PhysicsShape* mHeroShape{ nullptr };
    
    std::map<int,BlockBase*> mBlocks;
    
    std::set<BlockBase*> mSelections;
    
    bool mMoveLeft{ false };
    
    bool mMoveRight{ false };
    
    BlockBase* mSelectionHead{ nullptr };
    
    std::map<cocos2d::Node*,BlockBase*> mBlockTable;
    
    ShadowManager* mShadows{ nullptr };
    
    cocos2d::Color3B mBlockColors[KIND_MAX];
    
    cocos2d::Color3B mBackgroundColor;
    
    bool mDeadFlag{ false };
    
    bool mGameMode{ false };
    
    cocos2d::Vec2 mSpawnPos;
};

#endif /* defined(__JumpEdt__LogicManager__) */
