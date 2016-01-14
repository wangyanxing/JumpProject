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

class ColorPalette {
public:
  ColorPalette(const std::string &file);

  ~ColorPalette();

  cocos2d::Color3B getColor(int index) {
    return mPalette[index];
  }

  void setColor(int index, const cocos2d::Color3B& color) {
    mPalette[index] = color;
  }

  void clear() {
    mPalette.clear();
  }

private:
  void load();

private:
  std::string mFileName;

  std::map<int, cocos2d::Color3B> mPalette;
};

#endif /* ColorPalette_h */
