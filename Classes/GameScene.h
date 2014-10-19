#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "DrawNodeEx.h"

#define EDITOR_MODE

class BlockBase;
class Hero;

enum BlockKind {
    KIND_HERO = 0,
    KIND_BLOCK,
    KIND_DEATH,
    KIND_DEATH_CIRCLE,
    KIND_BUTTON,
    KIND_PUSHABLE,

    KIND_MAX
};

class ShadowManager {
public:
    
    ShadowManager(cocos2d::Node* parentNode);
    ~ShadowManager();
    
    void update(const std::map<int,BlockBase*>& blocks);
  
    cocos2d::Vec2 mLightPos;
    cocos2d::DrawNodeEx* mRenderer{ nullptr };
};

class Button {
public:
    
    enum PushDir {
        DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT
    };
    
    Button(BlockBase* parent);
    
    ~Button();
    
    void setParentHeight(float v);
    
    void setParentWidth(float v);
    
    // call in case collsion detected
    bool push(const cocos2d::Vec2& normal, BlockBase* hero);
    
    void update(float dt);
    
    void rotateDir();
    
    void reset();
    
    bool mEnable{ true };
    
    bool mPushing{ false };
    
    bool mCanRestore{ true };
    
    void updatePosition();
    
    void callPushEvent();
    
    void callPushingEvent();
    
    void callRestoreEvent();
    
    PushDir mDir{ DIR_DOWN };
    
    BlockBase* mParent{ nullptr };
    
    std::string mPushedEvent;
    
    std::string mRestoredEvent;
    
    std::string mPushingEvent;
    
    bool mPushedEventCalled{ false };
    
    bool mRestoredEventCalled{ false };

#ifdef EDITOR_MODE
    void updateHelper();
    void showHelper(bool val) {mHelperNode->setVisible(val);}
    cocos2d::DrawNode* mHelperNode{ nullptr };
#endif
};

class Path {
public:
    
    Path() {
#ifdef EDITOR_MODE
        mHelperNode = cocos2d::Node::create();
        mHelperNode->setPosition(0, 0);
        mSegmentNode = cocos2d::DrawNode::create();
        mHelperNode->addChild(mSegmentNode,90);
#endif
    }
    
    virtual ~Path() {
        clear();
        mHelperNode->removeFromParent();
    }
    
    struct PathPoint {
        PathPoint(const cocos2d::Vec2& p, float wt, float w, float h) {
            pt = p;
            waitTime = wt;
            width = w;
            height = h;
        }
        cocos2d::Vec2 pt;
        float waitTime{ -1 };
        float width{ 1 };
        float height{ 1 };
    };
    
    int nextPoint() {
        int nextPt;
        if(mPingPong) {
            nextPt = mDirection ? mCurPt + 1 : mCurPt - 1;
            nextPt %= mPoints.size();
        } else {
            nextPt = (mCurPt + 1) % mPoints.size();
        }
        return nextPt;
    }
    
    void reset() {
        mDirection = true;
        mCurPt = 0;
        mCurDist = 0;
        mWaitingTimer = 0;
    }
    
    size_t getNumPoints() {
        return mPoints.size();
    }
    
    const PathPoint& getPoint(size_t i) const {
        return mPoints[i];
    }
    
    void update(float dt, cocos2d::Vec2& out) {
        if(mPoints.empty()) return;
        if(mPause) dt = 0;
        if(mDisable) {
            out = mPoints[0].pt;
        } else {
            
            int nextPt = nextPoint();
            
            if(mWaitingTimer > mPoints[mCurPt].waitTime) {
                auto pt = mPoints[mCurPt];
                auto ptNext = mPoints[nextPt];
                float dist = pt.pt.distance(ptNext.pt);
                mCurDist += mSpeed * dt;
                if(mCurDist >= dist) {
                    // move to next point
                    mCurDist -= dist;
                    mWaitingTimer = 0;
                    
                    if(mDirection) {
                        if(nextPt == mPoints.size() - 1 && mPingPong) {
                            mDirection = !mDirection;
                        }
                    } else {
                        if(nextPt == 0 && mPingPong) {
                            mDirection = !mDirection;
                        }
                    }
                    mCurPt = nextPt;
                    nextPt = nextPoint();
                    
                    pt = mPoints[mCurPt];
                    ptNext = mPoints[nextPt];
                }
                auto dir = ptNext.pt - pt.pt;
                dir.x /= dist;
                dir.y /= dist;
                out = pt.pt + dir * mCurDist;
                
            } else {
                mWaitingTimer += dt;
            }
        }
        
#ifdef EDITOR_MODE
        mSegmentNode->clear();
        for(size_t i = 0; i < mPoints.size() - 1; ++i) {
            mSegmentNode->drawSegment(mPoints[i].pt, mPoints[i+1].pt, 1, cocos2d::Color4F(0,1,0,1));
        }
#endif
    }
    
    bool empty() {
        return mPoints.empty();
    }
    
    void clear() {
        while(!empty()) pop();
    }
    
    void pop() {
        if(empty()) return;
        
#ifdef EDITOR_MODE
        auto back = mPoints.back();
        auto node = mHelperNode->getChildByTag(mPoints.size() - 1);
        node->removeFromParent();
#else
        mPoints.back();
#endif
        
        mPoints.pop_back();
    }
    
    cocos2d::Vec2 getBackPos() {
        auto back = mPoints.back();
        return back.pt;
    }
    
    void setBackPos(const cocos2d::Vec2& pos) {
        mPoints.back().pt = pos;
        auto node = mHelperNode->getChildByTag(mPoints.size()-1);
        node->setPosition(pos);
    }
    
    void translatePoints(const cocos2d::Vec2& d) {
        for(size_t i = 0; i < mPoints.size(); ++i) {
            mPoints[i].pt += d;
            auto node = mHelperNode->getChildByTag(i);
            node->setPosition(mPoints[i].pt);
        }
    }
    
    void push(const cocos2d::Vec2& pos, float waitTime = -1, float width = 1, float height = 1) {
        PathPoint pt(pos, waitTime, width ,height);
        mPoints.push_back(pt);
#ifdef EDITOR_MODE
        auto sprite = cocos2d::Sprite::create("images/circle.png");
        sprite->setTag(mPoints.size() - 1);
        sprite->setPosition(pos);
        sprite->setScale(0.7);
        mHelperNode->addChild(sprite, 100);
#endif
    }
    
#ifdef EDITOR_MODE
    void setSegmentNodeScale(float scale) {
        for(size_t i = 0; i < mPoints.size(); ++i) {
            auto node = mHelperNode->getChildByTag(i);
            node->setScale(scale);
        }
    }
#endif
    
    bool mPingPong{ true };
    float mSpeed{ 50 };
    bool mDirection{ true };
    int mCurPt{ 0 };
    float mCurDist{ 0 };
    float mWaitingTimer{ 0 };
    bool mDisable{ true };
    bool mPause{ false };
    
#ifdef EDITOR_MODE
    cocos2d::DrawNode* mSegmentNode{ nullptr };
    cocos2d::Node* mHelperNode{ nullptr };
#endif
    
private:
    
    std::vector<PathPoint> mPoints;
};


class BlockBase {
public:
    
    enum Status {
        IDLE,
        OPENING,
        CLOSING
    };
    
    BlockBase();
    
    virtual ~BlockBase();
    
    virtual void setPosition(const cocos2d::Point& pt);
    
    void setPosition(float x, float y) {
        setPosition(cocos2d::Point(x,y));
    }
    
    void setPositionX(float x) {
        setPosition(cocos2d::Point(x,getPosition().y));
    }
    
    void setPositionY(float y) {
        setPosition(cocos2d::Point(getPosition().x, y));
    }
    
    void rotate();
    
    cocos2d::Point getPosition() { return mSprite->getPosition(); }
    
    virtual void create(const cocos2d::Point& pt);
    
    virtual void create(const cocos2d::Point& pt, const cocos2d::Size& size);
    
    virtual void create(const cocos2d::Rect& rect);
    
    virtual void setPhysicsDynamic(bool val);
    
    virtual void addToScene(cocos2d::Node* parent);
    
    virtual void addThickness(int val);
    
    virtual void subThickness(int val);
    
    virtual void addWidth(int val);
    
    virtual void subWidth(int val);
    
    void setWidth(float val);
    
    void setHeight(float val);
    
    virtual void setSize(cocos2d::Size size);
    
    cocos2d::Size getSize();
    
    virtual void moveX(float val);
    
    virtual void moveY(float val);
    
    virtual void switchToSelectionImage();
    
    virtual void switchToNormalImage();
    
    virtual int getZOrder(){ return mZOrder; }
    
    virtual float getWidth();
    
    virtual float getThickness();
    
    virtual void initPhysics();
    
    virtual void setVisible(bool val);
    
    virtual bool canPush();
    
    virtual bool pushable(){ return mKind == KIND_PUSHABLE; }
    
    virtual int pushPriority();
    
    bool isVisible() { return mSprite->isVisible(); };
    
    cocos2d::Rect getBoundingBox() { return mSprite->getBoundingBox(); }
    
    virtual void setKind(BlockKind kind);
    
    virtual cocos2d::Color3B getColor(){ return mColor; }
    
    virtual void getPointsForShadow(const cocos2d::Vec2& source,
                                    std::vector<cocos2d::Vec2>& out);
    
    cocos2d::Sprite* getSprite() { return mSprite; }
    
    virtual void update(float dt);
    
    virtual void postUpdate(float dt);
    
    virtual void openDoor(float speed = 250, bool downDirDoor = true);
    
    virtual void closeDoor(float speed = 250, bool downDirDoor = true);
    
    virtual void resetOpenClose();
    
    void updateOpenClose(float dt);
    
    bool mCastShadow{ true };
    
    float mShadowLength{ 100 };
    
    float mShadowFadeRatio{ 0.4 };
    
    int mShadowFadeSegments{ 20 };
    
    BlockKind mKind{ KIND_BLOCK };
    
    int mZOrder{ 20 };
    
    cocos2d::Color3B mColor{ cocos2d::Color3B::WHITE };
    
    Path mPath;
    
    float mImageSize{ 10 };
    
    Button* mButton{ nullptr };
    
    cocos2d::Vec2 mMovementThisFrame{ 0, 0 };
    
    bool mCanPush{ true };
    
    bool mCanPickup{ true };
    
    int mID{ 0 };
    
    cocos2d::Size mRestoreSize;
    
    cocos2d::Vec2 mRestorePosition;
    
    float mOpenCloseSpeed{ 100 };
    
    bool mDownDirDoor{ true };
    
    Status mStatus{ IDLE };
    
    static int mIDCounter;
    
#ifdef EDITOR_MODE
    bool mShowIDLabel{ false };
    void initIDLabel();
    cocos2d::LabelAtlas* mIDLabel{ nullptr };
#endif
    
private:
    cocos2d::Sprite* mSprite{ nullptr };
};

class Hero : public BlockBase {
public:
    
    Hero() {
        mShowIDLabel = false;
    }
  
    virtual void initPhysics();
    
    virtual cocos2d::Color3B getColor(){ return cocos2d::Color3B::BLACK; }
    
    virtual int getZOrder(){ return 30; }
    
    virtual bool canPush() { return true; }
    
    virtual void openDoor(float speed, bool downDirDoor){}
    
    virtual void closeDoor(float speed, bool downDirDoor){}
    
    bool mCanJump{ false };
    
    bool mPushing{ false };
};

class GameScene : public cocos2d::Layer
{
public:
    
    struct PostUpdater {
        void update(float dt) {
            GameScene::Scene->postUpdate(dt);
        }
    };
    
    static GameScene* Scene;

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    void createBackground();
    
    void keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    
    void keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    
    void mouseDown(cocos2d::Event* event);
    
    void mouseUp(cocos2d::Event* event);
    
    void mouseMove(cocos2d::Event* event);
    
    void convertMouse(cocos2d::Point& pt);
    
    bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);
    
public:
    
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);
    
    void onDrawPrimitive(const cocos2d::Mat4 &transform, uint32_t flags);
    
    BlockBase* findBlock(int id);
    
    void jump();
    
    void die();
    
    void duplicate();
    
    void update(float dt);
    
    void postUpdate(float dt);
    
    void updateGame(float dt);
    
    void enableGame(bool val);
    
    void setKind(int kind);
    
    void alignLeft();
    
    void alignRight();
    
    void alignUp();
    
    void alignDown();
    
    void clean(bool save);
    
    void setBackgroundColor(const cocos2d::Color3B& color);
    
    bool mPressingShift{ false };
    
    bool mPressingCtrl{ false };
    
    bool mPressingAlt{ false };
    
    bool mPressingM{ false };
    
    bool mGameMode{ false };
    
    bool mMoveLeft{ false };
    
    bool mMoveRight{ false };
    
    BlockBase* mMovingBlock{ nullptr };
    
    cocos2d::Point mLastPoint;
    
    cocos2d::Sprite* mSpawnPoint{ nullptr };
    
    Hero* mHero{ nullptr };
    
    cocos2d::PhysicsShape* mHeroShape{ nullptr };
    
    std::map<int,BlockBase*> mBlocks;
    
    std::set<BlockBase*> mSelections;
    
    BlockBase* mSelectionHead{ nullptr };
    
    std::map<cocos2d::Node*,BlockBase*> mBlockTable;
    
    ShadowManager* mShadows{ nullptr };
    
    bool mPathMode{ false };
    
    bool mShowGrid{ false };
    
    cocos2d::Color3B mBlockColors[KIND_MAX];
    
    cocos2d::Color3B mBackgroundColor;
    
    std::string mCurFileName;
    
    PostUpdater mPostUpdater;
};

#endif
