#ifndef __JumpEdt__EditorScene__
#define __JumpEdt__EditorScene__

#include "cocos2d.h"
#include "DrawNodeEx.h"
#include "Defines.h"
#include "Path.h"
#include "Button.h"
#include "Blocks.h"
#include "LogicManager.h"

#if EDITOR_MODE

class BlockBase;
class ShadowManager;
class Hero;
class GameLogic;

class EditorScene : public cocos2d::Layer
{
public:
    
    EditorScene(){}
    
    virtual ~EditorScene();
    
    struct PostUpdater {
        void update(float dt) {
            GameLogic::Game->postUpdate(dt);
        }
    };
    
    static EditorScene* Scene;

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    void keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    
    void keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    
    void mouseDown(cocos2d::Event* event);
    
    void mouseUp(cocos2d::Event* event);
    
    void mouseMove(cocos2d::Event* event);
    
    void convertMouse(cocos2d::Point& pt);
    
    bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);
    
    // implement the "static create()" method manually
    CREATE_FUNC(EditorScene);
    
public:
    
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);
    
    void onDrawPrimitive(const cocos2d::Mat4 &transform, uint32_t flags);
    
    void duplicate();
    
    void update(float dt);
    
    void enableGame(bool val, bool force = false);
    
    void setKind(int kind);
    
    void alignLeft();
    
    void alignRight();
    
    void alignUp();
    
    void alignDown();
    
    void clean(bool save);
    
    void group();
    
    bool mPressingShift{ false };
    
    bool mPressingCtrl{ false };
    
    bool mPressingAlt{ false };
    
    bool mPressingM{ false };
    bool mPressingN{ false };
    bool mPressingB{ false };
    
    BlockBase* mMovingBlock{ nullptr };
    
    cocos2d::Point mLastPoint;
    
    cocos2d::Sprite* mSpawnPoint{ nullptr };
    
    cocos2d::Sprite* mLightPoint{ nullptr };
    
    cocos2d::Sprite* mGradientCenterPoint{ nullptr };
    
    std::set<BlockBase*> mSelections;
    
    BlockBase* mSelectionHead{ nullptr };
    
    bool mPathMode{ false };
    
    bool mShowGrid{ false };
    
    std::string mCurFileName;
    
    PostUpdater mPostUpdater;
    
    GameLogic* mGame{ nullptr };
};

#endif
#endif
