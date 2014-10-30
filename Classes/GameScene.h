//
//  GameScene.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/27/14.
//
//

#ifndef __JumpEdt__GameScene__
#define __JumpEdt__GameScene__

#include "cocos2d.h"
#include "LogicManager.h"

class GameLogic;
class BlockBase;

class GameScene : public cocos2d::Layer
{
public:
    
    GameScene() = default;
    
    ~GameScene();
    
    static GameScene* Scene;
    
    struct PostUpdater {
        void update(float dt) {
            GameLogic::Game->postUpdate(dt);
        }
    };
    
    virtual bool init();
    
    void update(float dt);
    
    bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);
    
    void enableGame(bool v);
    
    void loadChooseLevel(const std::string& name);
    
    CREATE_FUNC(GameScene);
    
private:
    
    void onTouch(const cocos2d::Vec2& pos);
    void onEndTouch(const cocos2d::Vec2& pos);
    
    void createControlPad();
    void createMenuButtons();
    
    void updateChoosingLevel(float dt);
    void enterGame(const std::string& name);
    
    void toMainMenu();
    
    void showHideMenu(bool force = false);
    
private:
    
    GameLogic* mGame{ nullptr };
    
    PostUpdater mPostUpdater;
    
    cocos2d::Sprite* mLeftButton{ nullptr };
    cocos2d::Sprite* mRightButton{ nullptr };
    cocos2d::Sprite* mJumpButton{ nullptr };
    
    cocos2d::Label* mTimerLabel{ nullptr };
    cocos2d::MenuItemImage* mBackMenu{ nullptr };
    cocos2d::MenuItemImage* mRestartMenu{ nullptr };
    
    bool mChoosingLevel{ false };
    BlockBase* mLightPath{ nullptr };
    std::map<int,BlockBase*> mCurrentLevels;
    std::vector<cocos2d::Label*> mLevelLabels;
};

#endif /* defined(__JumpEdt__GameScene__) */
