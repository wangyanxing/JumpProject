#include "UIColorEditor.h"
#include "LogicManager.h"
#include "SpriteUV.h"

#include "GameUtils.h"
#include "VisibleRect.h"
#include "cocos-ext.h"
#include "Defines.h"

#define _COLOR_DEBUG_  1

#if _COLOR_DEBUG_

# include "UILayer.h"

#endif

USING_NS_CC;
USING_NS_CC_EXT;

#define COLOR_BUTTON_SIZE 30
#define SPACE_BETWEEN_TWO_BUTTON 5
#define BUTTON_COLS 10
#define BUTTON_ROWS 2
#define BUTTON_NUM BUTTON_COLS * BUTTON_ROWS

UIColorEditor *UIColorEditor::colorEditor = nullptr;

int indexData[BUTTON_NUM];

void UIColorEditor::init(cocos2d::Node *parent) {
  colorEditor = this;

  for (int i = 0; i < BUTTON_NUM; i++) {
    indexData[i] = i;
  }

  auto background = GameUtils::createRect(Rect(0, 0,
                                               VisibleRect::getFrameSize().width,
                                               UI_LAYER_HIGHT),
                                          Color3B::GRAY, false);
  background->setPositionY(EDT_UI_YBIAS + UI_LAYER_HIGHT / 2);
  parent->addChild(background);

  Rect r;
  r.origin = Point::ZERO;
  r.size = Size(COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE);
  mShowhideColorButton = GameUtils::createRect(r, Color3B::WHITE, false);
  mShowhideColorButton->setPositionX(COLOR_BUTTON_SIZE);
  mShowhideColorButton->setPositionY(COLOR_BUTTON_SIZE * 1.5f + EDT_UI_YBIAS);
  mShowhideColorButton->setTexture("images/sakura.png");
  mShowhideColorButton->setScale(0.5f);

  parent->addChild(mShowhideColorButton, 100);

  auto listener = EventListenerTouchOneByOne::create();
  listener->setSwallowTouches(true);
  listener->onTouchBegan = CC_CALLBACK_2(UIColorEditor::beginTouchShowHide, this);
  EventDispatcher *_eventDispatcher = Director::getInstance()->getEventDispatcher();
  _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, mShowhideColorButton);

  initColorButtons(parent);
}

bool UIColorEditor::beginTouchShowHide(cocos2d::Touch *touch, cocos2d::Event *event) {
  auto target = static_cast<Sprite *>(event->getCurrentTarget());
  auto loc = touch->getLocation();
  auto rect = target->getBoundingBox();
  if (rect.containsPoint(loc)) {
    mColorButtonShow = !mColorButtonShow;
    updateColorButtonDisplay();
  }
  return false;
}

bool UIColorEditor::beginTouchColor(cocos2d::Touch *touch, cocos2d::Event *event) {
  auto target = static_cast<Sprite *>(event->getCurrentTarget());
  auto loc = touch->getLocation();
  auto rect = target->getBoundingBox();
  if (rect.containsPoint(loc)) {
    void *p = target->getUserData();
    int index = *(int *) p;

#if _COLOR_DEBUG_
    char num[10];
    sprintf(num, "%d", index);
    UILayer::Layer->addMessage(num);
#endif

    if (onSetColorFunc) {
      onSetColorFunc(mPaletteIndexArray[index], mPaletteColorArray[index]);
    }
  }
  return false;
}

void UIColorEditor::updateColorButtonDisplay() {
  for (int i = 0; i < mColorButtons.size(); i++) {
    if (mPaletteIndexArray[i] > -1 && mColorButtonShow) {
      mColorButtons[i]->setColor(mPaletteColorArray[i]);
      mColorButtons[i]->setVisible(true);
    } else {
      mColorButtons[i]->setVisible(false);
    }
  }
}

void UIColorEditor::initColorButtons(cocos2d::Node *parent) {
  for (int i = 0; i < BUTTON_ROWS; i++) {
    for (int j = 0; j < BUTTON_COLS; j++) {
      Rect r;
      r.origin = Point::ZERO;
      r.size = Size(COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE);

      auto button = GameUtils::createRect(r, Color3B(0xFF, 0xFF, 0xFF), false);
      button->setPosition(Vec2(COLOR_BUTTON_SIZE * (j + 2.5f) + SPACE_BETWEEN_TWO_BUTTON * j,
                               COLOR_BUTTON_SIZE * (2 - i) - SPACE_BETWEEN_TWO_BUTTON * i +
                               EDT_UI_YBIAS));
      button->setTexture("images/rect.png");
      void *p = (void *) &indexData[BUTTON_COLS * i + j];
      button->setUserData(p);
      parent->addChild(button);
      mColorButtons.push_back(button);
    }
  }

  auto listener = EventListenerTouchOneByOne::create();
  listener->setSwallowTouches(true);
  listener->onTouchBegan = CC_CALLBACK_2(UIColorEditor::beginTouchColor, this);

  EventDispatcher *_eventDispatcher = Director::getInstance()->getEventDispatcher();
  for (int i = 0; i < mColorButtons.size(); i++) {
    if (i == 0) {
      _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, mColorButtons[i]);
    } else {
      _eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), mColorButtons[i]);
    }
  }

  cleanColors();
  addColor(1, Color3B(0xFF, 0xFF, 0xFF));
  addColor(2, Color3B(0xFF, 0x00, 0x00));
  addColor(3, Color3B(0x00, 0xFF, 0x00));
  addColor(4, Color3B(0x00, 0x00, 0xFF));

  addColor(5, Color3B(0xCC, 0xCC, 0xCC));
  addColor(6, Color3B(0xCC, 0x00, 0x00));
  addColor(7, Color3B(0x00, 0xCC, 0x00));
  addColor(8, Color3B(0x00, 0x00, 0xCC));

  addColor(9, Color3B(0x99, 0x99, 0x99));
  addColor(10, Color3B(0x99, 0x00, 0x00));
  addColor(11, Color3B(0x00, 0x99, 0x00));
  addColor(12, Color3B(0x00, 0x00, 0x99));

  addColor(13, Color3B(0x66, 0x66, 0x66));
  addColor(14, Color3B(0x66, 0x00, 0x00));
  addColor(15, Color3B(0x00, 0x66, 0x00));
  addColor(16, Color3B(0x00, 0x00, 0x66));

  addColor(17, Color3B(0x33, 0x33, 0x33));
  addColor(18, Color3B(0x33, 0x00, 0x00));
  addColor(19, Color3B(0x00, 0x33, 0x00));
  addColor(20, Color3B(0x00, 0x00, 0x33));

  updateColorButtonDisplay();
}

void UIColorEditor::cleanColors() {
  mColorTableEndIndex = 0;
  for (int i = 0; i < mPaletteIndexArray.size(); i++) {
    mPaletteIndexArray[i] = -1;
    mPaletteColorArray[i] = Color3B(0xFF, 0xFF, 0xFF);
  }
}

void UIColorEditor::addColor(int index, cocos2d::Color3B color) {
  if (mColorTableEndIndex >= BUTTON_NUM) {
    return;
  }

  int size = (int) mPaletteIndexArray.size();
  if (size - 1 < mColorTableEndIndex) {
    mPaletteIndexArray.push_back(index);
    mPaletteColorArray.push_back(color);
  } else {
    mPaletteIndexArray[mColorTableEndIndex] = index;
    mPaletteColorArray[mColorTableEndIndex] = color;
  }
  mColorTableEndIndex++;
}
