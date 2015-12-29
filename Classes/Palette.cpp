//
//  Palette.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/14/15.
//
//

#include "Palette.h"

USING_NS_CC;

Palette *Palette::sInstance = nullptr;

Palette::Palette() {
  mDefaultBlockColors[KIND_HERO] = Color3B::BLACK;
  mDefaultBlockColors[KIND_BLOCK] = Color3B::WHITE;
  mDefaultBlockColors[KIND_DEATH] = Color3B::BLACK;
  mDefaultBlockColors[KIND_DEATH_CIRCLE] = Color3B::BLACK;
  mDefaultBlockColors[KIND_BUTTON] = Color3B(254, 225, 50);
  mDefaultBlockColors[KIND_PUSHABLE] = Color3B(220, 150, 168);

  mPalette.insert(std::pair<int, Color3B>(1, Color3B(0xFF, 0xFF, 0xFF)));
  mPalette.insert(std::pair<int, Color3B>(2, Color3B(0xFF, 0x00, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(3, Color3B(0x00, 0xFF, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(4, Color3B(0x00, 0x00, 0xFF)));

  mPalette.insert(std::pair<int, Color3B>(5, Color3B(0xCC, 0xCC, 0xCC)));
  mPalette.insert(std::pair<int, Color3B>(6, Color3B(0xCC, 0x00, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(7, Color3B(0x00, 0xCC, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(8, Color3B(0x00, 0x00, 0xCC)));

  mPalette.insert(std::pair<int, Color3B>(9, Color3B(0x99, 0x99, 0x99)));
  mPalette.insert(std::pair<int, Color3B>(10, Color3B(0x99, 0x00, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(11, Color3B(0x00, 0x99, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(12, Color3B(0x00, 0x00, 0x99)));

  mPalette.insert(std::pair<int, Color3B>(13, Color3B(0x66, 0x66, 0x66)));
  mPalette.insert(std::pair<int, Color3B>(14, Color3B(0x66, 0x00, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(15, Color3B(0x00, 0x66, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(16, Color3B(0x00, 0x00, 0x66)));

  mPalette.insert(std::pair<int, Color3B>(17, Color3B(0x33, 0x33, 0x33)));
  mPalette.insert(std::pair<int, Color3B>(18, Color3B(0x33, 0x00, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(19, Color3B(0x00, 0x33, 0x00)));
  mPalette.insert(std::pair<int, Color3B>(20, Color3B(0x00, 0x00, 0x33)));
}
