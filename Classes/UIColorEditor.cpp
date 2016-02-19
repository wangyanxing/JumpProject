//
//  UIColorEditor.cpp
//  JumpEdt
//
//  Created by Yanxing Wang.
//
//

#include "UIColorEditor.h"
#include "SpriteUV.h"
#include "RectDrawNode.h"

#include "VisibleRect.h"
#include "cocos-ext.h"
#include "Defines.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define COLOR_BUTTON_SIZE 30
#define BUTTON_MARGIN 5
#define BUTTON_COLS 10
#define BUTTON_ROWS 2
#define BUTTON_NUM BUTTON_COLS * BUTTON_ROWS

UIColorEditor *UIColorEditor::colorEditor = nullptr;

static int indexData[BUTTON_NUM];

void UIColorEditor::init(cocos2d::Node *parent) {
  colorEditor = this;

  for (int i = 0; i < BUTTON_NUM; i++) {
    indexData[i] = i;
  }

  auto background = DrawNode::create();
  background->drawSolidRect(Vec2(0, VisibleRect::getFrameSize().height - UI_LAYER_HIGHT),
                            Vec2(VisibleRect::getFrameSize().width,
                                 VisibleRect::getFrameSize().height),
                            Color4F::GRAY);
  parent->addChild(background);
  initColorButtons(parent);
}

bool UIColorEditor::beginTouchColor(cocos2d::Touch *touch, cocos2d::Event *event) {
  auto target = static_cast<Sprite *>(event->getCurrentTarget());
  auto loc = touch->getLocation();
  auto rect = target->getBoundingBox();
  if (rect.containsPoint(loc)) {
    void *p = target->getUserData();
    int index = *(int *) p;
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
  float leftMargin = COLOR_BUTTON_SIZE / 2 + 10;
  for (int i = 0; i < BUTTON_ROWS; i++) {
    for (int j = 0; j < BUTTON_COLS; j++) {
      auto button = RectDrawNode::create(Size(COLOR_BUTTON_SIZE, COLOR_BUTTON_SIZE),
                                         Color3B::WHITE);
      button->setPosition(Vec2(leftMargin + COLOR_BUTTON_SIZE * j + BUTTON_MARGIN * j,
                               COLOR_BUTTON_SIZE * (2 - i) - BUTTON_MARGIN * i + EDT_UI_YBIAS));
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
