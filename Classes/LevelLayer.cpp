//
//  LevelScene.cpp
//
//  Created by cocos2d-x on 14-5-7.
//
//

#include "LevelScene.h"
#include "LevelLayer.h"
#include "VisibleRect.h"
#include "LevelSelector.h"

#define SHORTEST_SLIDE_LENGTH 5
#define TAP_LENGTH 20

USING_NS_CC;

LevelLayer::LevelLayer()
  :pageNode(0)
  ,curPageNode(0) {
}

LevelLayer::~LevelLayer() {
}
  
bool LevelLayer::init() {
    if (!Layer::init()) {
		return false;
	}
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(LevelLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(LevelLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(LevelLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    WINDOW_WIDTH = VisibleRect::right().x;
    WINDOW_HEIGHT = VisibleRect::top().y;
    return true;
}  
  
void LevelLayer::goToCurrNode() {
    this->runAction(MoveTo::create(0.2f, Vec2(-curPageNode * WINDOW_WIDTH, 0)));
}

void LevelLayer::addNode(Node *level) {
    if (level) {
        level->setContentSize(Size::Size(WINDOW_WIDTH, WINDOW_HEIGHT));
        level->setPosition(Point(WINDOW_WIDTH * pageNode, 0));
        level->setTag(pageNode);
        this->addChild(level);
        pageNode++;
    }
}

bool LevelLayer::onTouchBegan(Touch *touch, Event  *event) {
    touchDownPoint = touch->getLocation();
    touchCurPoint = touchDownPoint;
    return true;
}

void LevelLayer::onTouchMoved(Touch *touch, Event  *event) {
    Point touchPoint = touch->getLocation();
    auto currX = this->getPositionX() + touchPoint.x - touchCurPoint.x;
    Point posPoint = Vec2(currX, getPositionY());
    auto dis= fabsf(touchPoint.x - touchCurPoint.x);
    if (dis >= SHORTEST_SLIDE_LENGTH ) {
        setPosition(posPoint);
    }
    touchCurPoint = touchPoint;
}

void LevelLayer::onTouchEnded(Touch *touch, Event *event) {
    touchUpPoint = touch->getLocation();
    auto dis= touchUpPoint.getDistance(touchDownPoint);
    auto width = WINDOW_WIDTH/5;
    
    if(dis <= TAP_LENGTH) {
        Node* page = getChildByTag(curPageNode);
        LevelSelLayer* sel = dynamic_cast<LevelSelLayer*>(page);
        if(sel && curPageNode < (pageNode - 1) && sel->isTapToMove()) {
            ++curPageNode;
            goToCurrNode();
        }
        return;
    }
    
    if (dis >= SHORTEST_SLIDE_LENGTH ) {
        int offset = getPositionX() + curPageNode * WINDOW_WIDTH;
        if (offset > width) {
            if (curPageNode > 0) {
                --curPageNode;
            }
        } else if (offset < -width) {
            if (curPageNode < (pageNode - 1)) {
                ++curPageNode;
            }
        }
        
        goToCurrNode();
    }
}
