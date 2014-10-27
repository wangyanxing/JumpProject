#ifndef _UI_COLOR_EDITOR_
#define _UI_CLOR_EDITOR_

#include "cocos2d.h"

class UIColorEditor{
public:
    void init(cocos2d::Node* parent);

private:
    void updateColorButtonDisplay();
    bool beginTouchShowHide(cocos2d::Touch* touch, cocos2d::Event* event);
    bool beginTouchColor(cocos2d::Touch* touch, cocos2d::Event* event);
    void initColorButtons(cocos2d::Node* parent);
    void cleanColors();

public:
    static UIColorEditor* colorEditor;
    std::function<void(int, cocos2d::Color3B)> onSetColorFunc{nullptr};
private:
    cocos2d::Sprite* mShowhideColorButton;

    std::vector<cocos2d::Sprite*> mColorButtons;
    //std::vector<cocos2d::EventListenerTouchOneByOne*> mEventListners;

    std::vector<int> mPaletteIndexArray;
    std::vector<cocos2d::Color3B> mPaletteColorArray;

    bool mColorButtonShow{ true };
};

#endif