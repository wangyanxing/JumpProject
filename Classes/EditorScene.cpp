#include "EditorScene.h"
#include "VisibleRect.h"
#include "GameUtils.h"
#include "MapSerial.h"
#include "UILayer.h"
#include "Events.h"
#include "Shadows.h"
#include "cocos-ext.h"
#include "LogicManager.h"

#include "UIColorEditor.h"


#if EDITOR_MODE

#include <iostream>

USING_NS_CC;
USING_NS_CC_EXT;

#define UI_LAYER_HIGHT 100
////////////////////////

EditorScene::~EditorScene() {
    delete mGame;
    mGame = nullptr;
}

EditorScene* EditorScene::Scene = nullptr;

// on "init" you need to initialize your instance
bool EditorScene::init() {
    Scene = this;
    
    if ( !Layer::init() ) {
        return false;
    }
    
    MapSerial::saveRemoteMaps();
    
    getScheduler()->scheduleUpdate(this, -2, false);    
    getScheduler()->scheduleUpdate(&mPostUpdater, 100, false);
    
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(EditorScene::keyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(EditorScene::keyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(EditorScene::mouseDown, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(EditorScene::mouseUp, this);
    mouseListener->onMouseMove = CC_CALLBACK_1(EditorScene::mouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactPreSolve = CC_CALLBACK_2(EditorScene::onContactPreSolve, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    mGame = new GameLogic(this);
    
    mLightPoint = Sprite::create("images/sun.png");
    addChild(mLightPoint, 100);
    mLightPoint->setPosition(300,520);
    mGame->mShadows->mLightPos = mLightPoint->getPosition();
    mGame->mShadows->mOriginLightPos = mGame->mShadows->mLightPos;
    
    mGradientCenterPoint = Sprite::create("images/daisy.png");
    addChild(mGradientCenterPoint, 100);
    mGradientCenterPoint->setPosition(100,50);
    mGradientCenterPoint->setScale(0.3);
    mGame->setBackGradientCenter(mGradientCenterPoint->getPosition());
    
    mSpawnPoint = Sprite::create("images/cross.png");
    addChild(mSpawnPoint, 100);
    //mSpawnPoint->setPosition(VisibleRect::center());
    mSpawnPoint->setPosition(50,100);
    mGame->mSpawnPos = mSpawnPoint->getPosition();
    mSpawnPoint->setScale(0.3);
    
    MapSerial::loadLastEdit();
    
    UIColorEditor::colorEditor->onSetColorFunc = [&](int index, cocos2d::Color3B color){
        for (auto sel : mSelections) {
            //sel->mColor = color;
			sel->setColor(index);
        }
    };
    
    return true;
}

bool EditorScene::onContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve) {
    return mGame->onContactPreSolve(contact, solve);
}

void EditorScene::mouseDown(cocos2d::Event* event) {
    auto mouse = (EventMouse*)event;

	auto target = static_cast<Sprite*>(mouse->getCurrentTarget());

	Point pt(mouse->getCursorX(), mouse->getCursorY());
	convertMouse(pt);

	Size size = target->getContentSize();
	Rect rect = Rect(0, 0, size.width, size.height - UI_LAYER_HIGHT);

	if (!rect.containsPoint(pt)){
		return ;
	}
    
    if (mPressingM && !mGame->mGameMode) {
        mSpawnPoint->setPosition(pt);
        mGame->mSpawnPos = mSpawnPoint->getPosition();
        return;
    }
    if (mPressingN && !mGame->mGameMode) {
        mLightPoint->setPosition(pt);
        mGame->mShadows->mLightPos = mLightPoint->getPosition();
        mGame->mShadows->mOriginLightPos = mLightPoint->getPosition();
        return;
    }
    if (mPressingB && !mGame->mGameMode) {
        mGradientCenterPoint->setPosition(pt);
        mGame->setBackGradientCenter(mGradientCenterPoint->getPosition());
        return;
    }
    
    if (mPressingShift) {
        mGame->createBlock(pt, KIND_BLOCK);

        mMovingBlock = nullptr;
    } else {
        // pick up
        if(!mPressingCtrl) {
            mSelections.clear();
            mPathMode = false;
            mSelectionHead = nullptr;
        }
        
        mGame->blockTraversal([&](BlockBase* bl){
            bl->switchToNormalImage();
            auto box = bl->getSprite()->getBoundingBox();
            if(box.containsPoint(pt) && bl->mCanPickup) {
                mSelections.insert(bl);
                mMovingBlock = bl;
                mSelectionHead = bl;
            }
        });
        
        mLastPoint = pt;
        
        for(auto sel: mSelections) {
            sel->switchToSelectionImage();
        }
        if(mSelectionHead)
            mSelectionHead->getSprite()->setColor(Color3B(200,0,255));
    }
}

void EditorScene::mouseUp(cocos2d::Event* event) {
	auto mouse = (EventMouse*)event;

	auto target = static_cast<Sprite*>(mouse->getCurrentTarget());

	Point pt(mouse->getCursorX(), mouse->getCursorY());
	convertMouse(pt);

	Size size = target->getContentSize();
	Rect rect = Rect(0, 0, size.width, size.height - UI_LAYER_HIGHT);

	if (!rect.containsPoint(pt)){
		return;
	}

    mMovingBlock = nullptr;
}

void EditorScene::mouseMove(cocos2d::Event* event) {
	auto mouse = (EventMouse*)event;

	auto target = static_cast<Sprite*>(mouse->getCurrentTarget());

	Point pt(mouse->getCursorX(), mouse->getCursorY());
	convertMouse(pt);

	Size size = target->getContentSize();
	Rect rect = Rect(0, 0, size.width, size.height - UI_LAYER_HIGHT);

	if (!rect.containsPoint(pt)){
		return;
	}

    if(!mMovingBlock)
        return;
    
    Point dt = pt - mLastPoint;
    mLastPoint = pt;
    
    for(auto sel: mSelections) {
        sel->moveX(dt.x);
        sel->moveY(dt.y);
    }
}

void EditorScene::convertMouse(cocos2d::Point& pt) {
    auto visRect = Director::getInstance()->getOpenGLView()->getVisibleRect();
    auto height = visRect.origin.y + visRect.size.height;
    
    pt.y = height + pt.y;
    pt = convertToNodeSpace(pt);
}

void EditorScene::keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
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
            mGame->createFixedBlocks();
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
        
#if EDITOR_MODE
        // also show or hide ID labels
        mGame->blockTraversal([&](BlockBase* b){
            b->mIDLabel->setVisible(mShowGrid);
            b->mShowIDLabel = mShowGrid;
        });
#endif
    }
    
    // path mode
    if (keyCode == EventKeyboard::KeyCode::KEY_F && mSelectionHead && !mGame->mGameMode) {
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
    
    if (keyCode == EventKeyboard::KeyCode::KEY_H /*&& mSelectionHead && !mGame->mGameMode*/) {
        //mSelectionHead->mRotator.push(0);
        //mSelectionHead->mRotator.push(90);
        auto g = mGame->mHero->getSprite()->getPhysicsBody()->isGravityEnabled();
        mGame->mHero->getSprite()->getPhysicsBody()->setGravityEnable(!g);
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
        // only work for one selection
        if(mPathMode) {
            mPathMode = false;
        }
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_SPACE) {
        if(mGame->mGameMode)
            mGame->jump();
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_RETURN ||
        keyCode == EventKeyboard::KeyCode::KEY_ENTER ||
        keyCode == EventKeyboard::KeyCode::KEY_L) {
        enableGame(!mGame->mGameMode);
    }
    
    if (keyCode == EventKeyboard::KeyCode::KEY_M) {
        mPressingM = true;
    }
    if (keyCode == EventKeyboard::KeyCode::KEY_N) {
        mPressingN = true;
    }
    if (keyCode == EventKeyboard::KeyCode::KEY_B) {
        mPressingB = true;
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
                mGame->deleteBlock(sel);
            }
        }
        mSelections.clear();
    }
    
    if (mGame->mGameMode) {
        if (keyCode == EventKeyboard::KeyCode::KEY_A)
            mGame->mMoveLeft = true;
        if (keyCode == EventKeyboard::KeyCode::KEY_D)
            mGame->mMoveRight = true;
    }
}

void EditorScene::keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
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
    if (keyCode == EventKeyboard::KeyCode::KEY_N) {
        mPressingN = false;
    }
    if (keyCode == EventKeyboard::KeyCode::KEY_B) {
        mPressingB = false;
    }
    
    if (mGame->mGameMode) {
        if (keyCode == EventKeyboard::KeyCode::KEY_A)
            mGame->mMoveLeft = false;
        if (keyCode == EventKeyboard::KeyCode::KEY_D)
            mGame->mMoveRight = false;
    }
}

void EditorScene::enableGame(bool val, bool force) {
    
    mGame->enableGame(val,force);
    mSpawnPoint->setVisible(!val);
    mLightPoint->setVisible(!val);
    mGradientCenterPoint->setVisible(!val);
    if(val) {
        mPressingB = mPressingN = mPressingM = false;
    }
}

void EditorScene::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
    Layer::draw(renderer, transform, flags);
    
    static CustomCommand _customCommand;
    _customCommand.init(400);
    _customCommand.func = CC_CALLBACK_0(EditorScene::onDrawPrimitive, this, transform, flags);
    renderer->addCommand(&_customCommand);
}

void EditorScene::onDrawPrimitive(const Mat4 &transform, uint32_t flags) {
    Director* director = Director::getInstance();
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
    
    if(mShowGrid) {
        auto size = mGame->mHero->getSize();
        
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
    
    if(!mGame->mGameMode) {
        DrawPrimitives::setDrawColor4B(17,47,245,255);
        for(auto g : mGame->mGroups){
            auto head = g.first;
            
            for(auto m : g.second) {
                DrawPrimitives::drawSolidCircle(m->getPosition(), 3, 0, 20, 1, 1);
                DrawPrimitives::drawLine(head->getPosition(), m->getPosition());
            }
        }
    }
    
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
void EditorScene::update(float dt){
    
    mGame->update(dt);
}

void EditorScene::setKind(int kind) {
    if(kind >= KIND_MAX) return;
    
    for(auto sel : mSelections) {
        
        sel->setKind(BlockKind(kind));
    }
}

void EditorScene::duplicate() {
    std::set<BlockBase*> duplicated;
    
    Vec2 bias(20,20);
    
    for(auto sel : mSelections) {
        
        BlockBase* block = new BlockBase();
        auto pos = sel->getPosition() + bias;
        block->create(pos, sel->getSize());
        block->addToScene(this);
        block->setKind(sel->mKind);
        block->mPath.cloneFrom(sel->mPath,bias);
        
        block->mRestoreSize = block->getSize();
        block->mRestorePosition = block->getPosition();
        
        mGame->mBlockTable[block->getSprite()] = block;
        mGame->mBlocks[block->mID] = block;
        
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

void EditorScene::alignLeft() {
    auto p = mSelectionHead->getPosition();
    auto w = mSelectionHead->getSize().width;
    float mostLeft = p.x - w/2;
    
    for(auto sel : mSelections) {
        if(sel != mSelectionHead) {
            auto selWid = sel->getSize().width;
            sel->setPositionX(mostLeft + selWid/2);
        }
    }
}

void EditorScene::alignRight() {
    auto p = mSelectionHead->getPosition();
    auto w = mSelectionHead->getSize().width;
    float mostRight = p.x + w/2;
    
    for(auto sel : mSelections) {
        if(sel != mSelectionHead) {
            auto selWid = sel->getSize().width;
            sel->setPositionX(mostRight - selWid/2);
        }
    }
}

void EditorScene::alignUp() {
    auto p = mSelectionHead->getPosition();
    auto h = mSelectionHead->getSize().height;
    float mostUp = p.y + h/2;
    
    for(auto sel : mSelections) {
        if(sel != mSelectionHead) {
            auto selHei = sel->getSize().height;
            sel->setPositionY(mostUp - selHei/2);
        }
    }
}

void EditorScene::alignDown() {
    auto p = mSelectionHead->getPosition();
    auto h = mSelectionHead->getSize().height;
    float mostDown = p.y - h/2;
    
    for(auto sel : mSelections) {
        if(sel != mSelectionHead) {
            auto selHei = sel->getSize().height;
            sel->setPositionY(mostDown + selHei/2);
        }
    }
}

void EditorScene::group() {
    if(mSelections.empty() || !mSelectionHead) {
        return;
    }
    
    auto it = mGame->mGroups.find(mSelectionHead);
    if(it != mGame->mGroups.end()) {
        for(auto s : it->second) {
            s->reset();
        }
        mGame->mGroups.erase(it);
        UILayer::Layer->addMessage("Ungroup");
    } else  {
        mGame->mGroups[mSelectionHead].clear();
        for(auto s : mSelections) {
            if(s == mSelectionHead) continue;
            mGame->mGroups[mSelectionHead].push_back(s);
        }
        
        UILayer::Layer->addMessage("Group");
    }
}

void EditorScene::clean(bool save) {

    if(save && !mCurFileName.empty())
        MapSerial::saveMap(mCurFileName.c_str());
    
    mSelectionHead = nullptr;
    mSelections.clear();
    
    mGame->clean();
    
    mCurFileName = "";
    UILayer::Layer->setFileName("untitled");
}

#endif