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
    
    CREATE_FUNC(GameScene);
    
private:
    
    void onTouch(const cocos2d::Vec2& pos);
    void onEndTouch(const cocos2d::Vec2& pos);
    
    void createControlPad();
    
private:
    
    GameLogic* mGame{ nullptr };
    
    PostUpdater mPostUpdater;
    
    cocos2d::Sprite* mLeftButton{ nullptr };
    cocos2d::Sprite* mRightButton{ nullptr };
    cocos2d::Sprite* mJumpButton{ nullptr };
};

#endif /* defined(__JumpEdt__GameScene__) */
