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
class LightBeam;

class GameLogic {
public:
    
    static GameLogic* Game;
    
    GameLogic(cocos2d::Layer* parent);
    
    ~GameLogic();
    
    BlockBase* createBlock(const cocos2d::Vec2& pos, BlockKind kind);
    
    void showGameScene(bool val);
    
    void blockTraversal(const std::function<void(BlockBase*)>& func);
    
    void createFixedBlocks();
    
    void deleteBlock(BlockBase* b);
    
    bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);
    
    void update(float dt);
    
    void clean();
    
    void enableGame(bool val, bool force = false);
    
    void postUpdate(float dt);
    
    void updateGame(float dt);
    
    void setBackgroundColor(const cocos2d::Color3B& color);

	cocos2d::Color3B getColorFromPalette(int index){ return mPalette[index]; }
    
    BlockBase* findBlock(int id);
    
    void jump();
    
    void die();
    
    void win();
    
    void setBackGradientCenter(const cocos2d::Vec2& p);
    
    void setBackGradientColor(const cocos2d::Color3B& colorSrc,
                              const cocos2d::Color3B& colorDst);
    
    cocos2d::Node* createParticle(const cocos2d::Vec2& pos);
    
    void loadFxFromList();
    
    void loadStarFromList();
    
    void clearFx();
    
    void clearStars();
    
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
	std::map<int, cocos2d::Color3B> mPalette;
	std::string mPaletteFileName{"palette/color01.json"};
    
    cocos2d::Color3B mBackgroundColor;
    
    cocos2d::Sprite* mBack{ nullptr };
    
    bool mDeadFlag{ false };
    
    bool mWinFlag{ false };
    
    bool mGameMode{ false };
    
    cocos2d::Vec2 mSpawnPos;
    
    cocos2d::Vec2 mGradientCenter{0,0};
    cocos2d::Color3B mGradientColorSrc;
    cocos2d::Color3B mGradientColorDst;
    
    float mGameTimer{ 0 };
    
    std::function<void()> mWinGameEvent;
    
    std::vector<std::string> mFxList;
    std::vector<cocos2d::ParticleBatchNode*> mFxNodes;
    
    std::vector<cocos2d::Vec2> mStarList;
    std::vector<cocos2d::Node*> mStarNodes;
    
    bool mRejectInput{ false };
    
    std::vector<LightBeam*> mLightBeams;
};

#endif /* defined(__JumpEdt__LogicManager__) */
