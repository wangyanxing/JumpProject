//
//  ColorPalette.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#include "ColorPalette.h"
#include "JsonParser.h"
#include "JsonFormat.h"

#if EDITOR_MODE
# include "UIColorEditor.h"
#endif

ColorPalette::ColorPalette(const std::string &file) : mFileName(file) {
  load();
}

ColorPalette::~ColorPalette() {
}

void ColorPalette::load() {
  JsonParser parser(JsonParser::getBuffer(mFileName));

  if (!parser) {
    CCLOGERROR("Cannot load the palette file: %s", mFileName.c_str());
  }

  CCLOG("Loading palette file: %s", mFileName.c_str());

  auto &doc = parser.getCurrentDocument();
  mBackgroundColor = doc[PALETTE_BACKGROUND].GetColor();
  
  mDefaultColors[KIND_HERO] = doc[PALETTE_HERO].GetColor();
  mDefaultColors[KIND_BLOCK] = doc[PALETTE_NORMAL].GetColor();
  mDefaultColors[KIND_DEATH] = doc[PALETTE_DEATH].GetColor();
  mDefaultColors[KIND_DEATH_CIRCLE] = doc[PALETTE_DEATH_CIRCLE].GetColor();
  mDefaultColors[KIND_BUTTON] = doc[PALETTE_BUTTON].GetColor();;
  mDefaultColors[KIND_PUSHABLE] = doc[PALETTE_PUSHABLE].GetColor();

#if EDITOR_MODE
  UIColorEditor::colorEditor->cleanColors();
#endif
  mPalette.clear();

  parser.parseArray(doc, PALETTE_PALETTE_ARRAY, [&](JsonSizeT i, JsonValueT& val) {
    int id = val[PALETTE_PALETTE_ID].GetInt();
    auto color = val[PALETTE_PALETTE_COLOR].GetColor();
    mPalette[id] = color;
#if EDITOR_MODE
    UIColorEditor::colorEditor->addColor(id, color);
#endif
  });

#if EDITOR_MODE
  UIColorEditor::colorEditor->updateColorButtonDisplay();
#endif
}
