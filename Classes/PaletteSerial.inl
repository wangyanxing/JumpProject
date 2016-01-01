//
//  PaletteSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 1/1/16.
//
//

void MapSerial::savePalette(const char *file) {
  time_t rawtime;
  struct tm *ptm;
  time(&rawtime);
  ptm = gmtime(&rawtime);
  std::string timestr = asctime(ptm);
  timestr.resize(timestr.size() - 1);
  
  std::string author = "unknown";
  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  TCHAR username[UNLEN + 1];
  DWORD size = UNLEN + 1;
  GetUserName((TCHAR *) username, &size);
  author = username;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  author = getlogin();
#endif
  
  stringstream ss;
  ss << "{\n";
  
  INDENT_1
  ss << "\"author\": " << "\"" << author << "\"";
  RT_LINE
  INDENT_1
  ss << "\"time\": " << "\"" << timestr << "\"";
  RT_LINE
  
  INDENT_1
  ss << "\"palette\": [ \n";
  
  const auto &palette = Palette::getInstance()->getPalette();
  for (auto it = palette.begin(); it != palette.end(); ++it) {
    if (it != palette.begin()) {
      ss << ", \n";
    }
    INDENT_2
    ss << "{\n";
    INDENT_3
    ss << "\"index\": " << it->first << ", \n";
    INDENT_3
    ss << "\"color\": " << colorStr(it->second) << " \n";
    INDENT_2
    ss << "}";
  }
  
  INDENT_1
  ss << "] \n";
  
  ss << "}";
  
  auto fp = fopen(file, "w+");
  if (!fp) {
    CCLOGWARN("Warning: cannot write the map file : %s", file);
    return;
  }
  fprintf(fp, "%s", ss.str().c_str());
  fclose(fp);
}

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
