//
//  ColorPalette.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#include "ColorPalette.h"
#include "JsonParser.h"

#if EDITOR_MODE
# include "UIColorEditor.h"
#endif

ColorPalette::ColorPalette(const std::string &file) : mFileName(file) {
  load();
}

ColorPalette::~ColorPalette() {
}

void ColorPalette::load() {
  JsonParser parser(mFileName);

  if (!parser) {
    CCLOGERROR("Cannot load the palette file: %s", mFileName.c_str());
  }

  CCLOG("Loading palette file: %s", mFileName.c_str());

  auto &doc = parser.getCurrentDocument();
  mBackgroundColor = doc["backgroundColor"].GetColor();
  
  mDefaultColors[KIND_HERO] = doc["heroColor"].GetColor();
  mDefaultColors[KIND_BLOCK] = doc["normalBlockColor"].GetColor();
  mDefaultColors[KIND_DEATH] = doc["deathBlockColor"].GetColor();
  mDefaultColors[KIND_DEATH_CIRCLE] = doc["deathCircleColor"].GetColor();
  mDefaultColors[KIND_BUTTON] = doc["buttonColor"].GetColor();;
  mDefaultColors[KIND_PUSHABLE] = doc["pushableBlockColor"].GetColor();

#if EDITOR_MODE
  UIColorEditor::colorEditor->cleanColors();
#endif
  mPalette.clear();

  parser.parseArray(doc, "palette", [&](JsonSizeT i, JsonValueT& val) {
    int id = val["index"].GetInt();
    auto color = val["color"].GetColor();
    mPalette[id] = color;
#if EDITOR_MODE
    UIColorEditor::colorEditor->addColor(id, color);
#endif
  });

#if EDITOR_MODE
  UIColorEditor::colorEditor->updateColorButtonDisplay();
#endif
}
