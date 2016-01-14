//
//  ColorPalette.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#include "ColorPalette.h"
#include "Defines.h"
#include "SerializationUtils.h"

#if EDITOR_MODE
# include "UIColorEditor.h"
#endif

ColorPalette::ColorPalette(const std::string &file) : mFileName(file) {
  load();
}

ColorPalette::~ColorPalette() {
}

void ColorPalette::load() {
  auto &serialUtil = SerializationUtils::instance();

  if (!serialUtil.beginJson(mFileName)) {
    CCLOGERROR("Cannot load the palette file: %s", mFileName.c_str());
  }

#if EDITOR_MODE
    UIColorEditor::colorEditor->cleanColors();
#endif
  mPalette.clear();

  serialUtil.parseArray("palette", [&](JsonSizeT i, JsonValueT& val) {
    int id = val["index"].GetInt();
    auto color = serialUtil.parseColor(val["color"]);
    mPalette[id] = color;
#if EDITOR_MODE
    UIColorEditor::colorEditor->addColor(id, color);
#endif
  });

  SerializationUtils::instance().endJson();

#if EDITOR_MODE
  UIColorEditor::colorEditor->updateColorButtonDisplay();
#endif
}
