#include "UIColorEditor.h"
#if 1
    #include "UILayer.h"
#endif

#include "LogicManager.h"

#include "GameUtils.h"
#include "VisibleRect.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define COLOR_BUTTON_SIZE 30
#define SPACE_BETWEEN_TWO_BUTTON 5
#define BUTTON_COLS 10
#define BUTTON_ROWS 2
#define BUTTON_NUM 20

UIColorEditor* UIColorEditor::colorEditor = nullptr;

int indexData[BUTTON_NUM];

void UIColorEditor::init(cocos2d::Node* parent){
    colorEditor = this;

    for (int i = 0; i < BUTTON_NUM; i++){
        indexData[i] = i;
    }

    Rect r;
    r.origin = Point::ZERO;
    r.size = Size(COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE);
    mShowhideColorButton = GameUtils::createRect(r, Color3B(0xff, 0xff, 0xff));	//Color3B
    mShowhideColorButton->setPosition(Vec2(COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE*1.5f));
    mShowhideColorButton->setTexture("images/sakura.png");
    mShowhideColorButton->setScale(0.5f);

    parent->addChild(mShowhideColorButton);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(UIColorEditor::beginTouchShowHide, this);
    EventDispatcher* _eventDispatcher = Director::getInstance()->getEventDispatcher();
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, mShowhideColorButton);


    initColorButtons(parent);
}

bool UIColorEditor::beginTouchShowHide(cocos2d::Touch* touch, cocos2d::Event* event){
    auto target = static_cast<Sprite*>(event->getCurrentTarget());

    Point locationInNode = target->convertTouchToNodeSpace(touch);

    Size size = target->getContentSize();
    Rect rect = Rect(0, 0, size.width, size.height);

    if (rect.containsPoint(locationInNode)){
        mColorButtonShow = !mColorButtonShow;
        updateColorButtonDisplay();
    }
    return false;
}

bool UIColorEditor::beginTouchColor(cocos2d::Touch* touch, cocos2d::Event* event){
    auto target = static_cast<Sprite*>(event->getCurrentTarget());

    Point locationInNode = target->convertTouchToNodeSpace(touch);

    Size size = target->getContentSize();
    Rect rect = Rect(0, 0, size.width, size.height);

    if (rect.containsPoint(locationInNode)){
        void* p = target->getUserData();
        int index = *(int*)p;
        char num[10];
        sprintf(num, "%d", index);
        UILayer::Layer->addMessage(num);
        GameLogic::Game->mSelections;

        if (onSetColorFunc != nullptr)
            onSetColorFunc(mPaletteIndexArray[index], mPaletteColorArray[index]);
    }
    return false;
}

void UIColorEditor::updateColorButtonDisplay(){
    for (int i = 0; i < mColorButtons.size(); i++){
        if (mPaletteIndexArray[i]>-1 && mColorButtonShow)
            mColorButtons[i]->setVisible(true);
        else
            mColorButtons[i]->setVisible(false);
    }
}

void UIColorEditor::initColorButtons(cocos2d::Node* parent){
    for (int i = 0; i < BUTTON_ROWS; i++) {
        for (int j = 0; j < BUTTON_COLS; j++) {
            Rect r;
            r.origin = Point::ZERO;
            r.size = Size(COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE);

            float colorValue = (i*BUTTON_COLS*1.0+j)/(BUTTON_ROWS * BUTTON_COLS * 1.0f)*255.0f;
            int colorInt = colorValue;
            auto button = GameUtils::createRect(r, Color3B(colorInt, colorInt, colorInt));
            button->setPosition(Vec2(COLOR_BUTTON_SIZE*(j + 2.5f) + SPACE_BETWEEN_TWO_BUTTON*j, COLOR_BUTTON_SIZE*(2 - i) - SPACE_BETWEEN_TWO_BUTTON*i));
            button->setTexture("images/rect.png");
            void* p = (void*)&indexData[BUTTON_COLS*i + j];
            button->setUserData(p);
            parent->addChild(button);

            mPaletteIndexArray.push_back(i*BUTTON_COLS + j);
            mPaletteColorArray.push_back(Color3B(colorInt, colorInt, colorInt));

            mColorButtons.push_back(button);
        }
    }

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(UIColorEditor::beginTouchColor, this);
        
    EventDispatcher* _eventDispatcher = Director::getInstance()->getEventDispatcher();
    for (int i = 0; i < mColorButtons.size(); i++){
        if (i==0)
            _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, mColorButtons[i]);
        else
            _eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), mColorButtons[i]);
    }
    
    updateColorButtonDisplay();
}

void UIColorEditor::cleanColors(){
    for (int i = 0; i < mPaletteIndexArray.size(); i++){
        mPaletteIndexArray[i] = -1;
    }
}