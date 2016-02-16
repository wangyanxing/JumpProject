#ifndef __JumpEdt__UIColorEditor__
#define __JumpEdt__UIColorEditor__

#include "Prerequisites.h"

class UIColorEditor {
public:
  void init(cocos2d::Node *parent);

  void cleanColors();

  void addColor(int index, cocos2d::Color3B color);

  void updateColorButtonDisplay();

public:
  static UIColorEditor *colorEditor;

  std::function<void(int, cocos2d::Color3B)> onSetColorFunc{nullptr};

private:
  bool beginTouchColor(cocos2d::Touch *touch, cocos2d::Event *event);

  void initColorButtons(cocos2d::Node *parent);

private:
  std::vector<cocos2d::DrawNode *> mColorButtons;

  std::vector<int> mPaletteIndexArray;

  std::vector<cocos2d::Color3B> mPaletteColorArray;

  int mColorTableEndIndex{0};

  bool mColorButtonShow{true};
};

#endif
