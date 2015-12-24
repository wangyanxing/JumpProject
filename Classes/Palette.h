//
//  Palette.hpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/14/15.
//
//

#ifndef Palette_hpp
#define Palette_hpp

#include "Defines.h"

class Palette {
public:
  static Palette *getInstance() {
    if (!sInstance) {
      sInstance = new Palette();
    }
    return sInstance;
  }

  cocos2d::Color3B getDefaultBlockColors(BlockKind kind) {
    return mDefaultBlockColors[kind];
  }

  void setDefaultBlockColors(BlockKind kind, cocos2d::Color3B color) {
    mDefaultBlockColors[kind] = color;
  }

  std::string getPaletteFileName() {
    return mPaletteFileName;
  }

  void setPaletteFileName(const std::string &file) {
    mPaletteFileName = file;
  }

  cocos2d::Color3B getColorFromPalette(int index) {
    return mPalette[index];
  }

  void setColorFromPalette(int index, cocos2d::Color3B color) {
    mPalette[index] = color;
  }

  void clearPalette() {
    mPalette.clear();
  }

  const std::map<int, cocos2d::Color3B> &getPalette() {
    return mPalette;
  }

private:
  static Palette *sInstance;

  Palette();

  cocos2d::Color3B mDefaultBlockColors[KIND_MAX];

  std::map<int, cocos2d::Color3B> mPalette;

  std::string mPaletteFileName = "palette/color01.json";
};

#endif /* Palette_hpp */
