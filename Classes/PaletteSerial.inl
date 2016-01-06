//
//  PaletteSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 1/1/16.
//
//

void MapSerial::loadPalette(const char *file) {
  std::ifstream paletteFileStream(file);
  
  if (!paletteFileStream) {
    CCLOGWARN("Warning: cannot access the palette file : %s\nUsing default value", file);
  } else {
    std::string paletteBuffer((std::istreambuf_iterator<char>(paletteFileStream)),
                              std::istreambuf_iterator<char>());
    Document dPalette;
    ParseResult ok = dPalette.Parse<kParseDefaultFlags>(paletteBuffer.c_str());
    if (!ok) {
      printf("JSON parse error: %d (%lu)\n", ok.Code(), ok.Offset());
    }
    
    if (CHECK_ARRAY(dPalette, "palette")) {
      auto size = dPalette["palette"].Size();
      if (size > 0) {
        Palette::getInstance()->clearPalette();
#if EDITOR_MODE
        UIColorEditor::colorEditor->cleanColors();
#endif
      }
      
      for (auto i = 0; i < size; i++) {
        auto &palette = dPalette["palette"][i];
        if (palette["index"].IsInt() && palette["color"].IsString()) {
          Palette::getInstance()->setColorFromPalette(palette["index"].GetInt(),
                                                      str2Color(palette["color"].GetString()));
#if EDITOR_MODE
          UIColorEditor::colorEditor->addColor(palette["index"].GetInt(),
                                               str2Color(palette["color"].GetString()));
#endif
        }
      }
      
#if EDITOR_MODE
      UIColorEditor::colorEditor->updateColorButtonDisplay();
#endif
    }
  }
}
