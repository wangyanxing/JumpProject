//
//  ColorPalette.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#ifndef ColorPalette_h
#define ColorPalette_h

#include "Prerequisites.h"
#include "Defines.h"

class ColorPalette {
public:
  ColorPalette(const std::string &file);

  ~ColorPalette();

  cocos2d::Color3B getColor(int index) {
    return mPalette[index];
  }

  void setColor(int index, cocos2d::Color3B color) {
    mPalette[index] = color;
  }

  cocos2d::Color3B getDefaultColor(BlockKind kind) {
    return mDefaultColors[kind];
  }

  void setDefaultColor(BlockKind kind, cocos2d::Color3B color) {
    mDefaultColors[kind] = color;
  }

  cocos2d::Color3B getBackgroundColor(BlockKind kind) {
    return mBackgroundColor;
  }

  void setBackgroundColor(cocos2d::Color3B color) {
    mBackgroundColor = color;
  }

  void clear() {
    mPalette.clear();
  }

private:
  void load();

private:
  std::string mFileName;

  std::map<int, cocos2d::Color3B> mPalette;

  cocos2d::Color3B mDefaultColors[KIND_MAX];

  cocos2d::Color3B mBackgroundColor;
};

#endif /* ColorPalette_h */
