//
//  Blocks.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__Blocks__
#define __JumpEdt__Blocks__

#include "cocos2d.h"
#include "Defines.h"
#include "Path.h"
#include "Rotator.h"
#include "SpriteUV.h"

class Button;

class BlockBase {
public:
    
    enum Status {
        IDLE,
        OPENING,
        CLOSING,
        OPENED,
        CLOSED
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
    
    void normalizeUV();
    
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
    
    virtual void setKind(BlockKind kind);
    
    virtual cocos2d::Color3B getColor(){ return mColor; }
    
    virtual void getPointsForShadow(const cocos2d::Vec2& source,
                                    std::vector<cocos2d::Vec2>& out);
    
    SpriteUV* getSprite() { return mSprite; }
    
	void setColor(int index);

    virtual void update(float dt);
    
    virtual void preUpdate();
    
    virtual void updatePathMove();
    
    virtual void postUpdate(float dt);
    
    virtual void openDoor(float speed = 250, bool downDirDoor = true);
    
    virtual void closeDoor(float speed = 250, bool downDirDoor = true);
    
    virtual void reset();
    
    void updateOpenClose(float dt);
    
    bool mCastShadow{ true };
    
    float mShadowLength{ 100 };
    
    float mShadowFadeRatio{ 0.4 };
    
    int mShadowFadeSegments{ 20 };
    
    BlockKind mKind{ KIND_BLOCK };
    
    int mZOrder{ 20 };

	int mPaletteIndex{ -1 };
	cocos2d::Color3B mColor{ cocos2d::Color3B::WHITE };

    Path mPath;
    
    Rotator mRotator;
    
    float mImageSize{ 8 };
    
    Button* mButton{ nullptr };
    
    cocos2d::Vec2 mMovementThisFrame{ 0, 0 };
    
    cocos2d::Vec2 mMovementToRestore{ 0, 0 };
    cocos2d::Vec2 mUpSideMovement{ 0, 0 };
    cocos2d::Vec2 mDownSideMovement{ 0, 0 };
    cocos2d::Vec2 mLeftSideMovement{ 0, 0 };
    cocos2d::Vec2 mRightSideMovement{ 0, 0 };
    
    FollowMode mFollowMode{ F_CENTER };
    
    bool mCanPush{ true };
    
    bool mCanPickup{ true };
    
    int mID{ 0 };
    
    cocos2d::Size mRestoreSize;
    
    cocos2d::Vec2 mRestorePosition;
    
    float mRestoreRotation{ 0 };
    
    float mOpenCloseSpeed{ 100 };
    
    int mRotationSpeed{ 0 };
    
    bool mDownDirDoor{ true };

	void callTriggerEvent();
    
    Status mStatus{ IDLE };
    
    bool mUVFlipped{ false };
    
    static int mIDCounter;
    
    std::string mTextureName{ "images/saw3.png" };

	std::vector<std::string> mTriggerEvents;
    bool mTriggerEventsCalled{ false };
    bool mHeroOpacityChanged{ false };
    float mTriggerEventContinueTime{ 0.0 };

#if EDITOR_MODE
    bool mShowIDLabel{ false };
    void initIDLabel();
    void updateIDLabel();
    cocos2d::LabelAtlas* mIDLabel{ nullptr };
#endif
    
private:
    SpriteUV* mSprite{ nullptr };
};

class Hero : public BlockBase {
public:
    
    Hero() {
#if EDITOR_MODE
        mShowIDLabel = false;
#endif
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


#endif /* defined(__JumpEdt__Blocks__) */
