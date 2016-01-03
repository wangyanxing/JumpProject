//
//  ControlConfigSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 1/1/16.
//
//

void MapSerial::saveControlConfig(const char *file) {
  if (ControlPad::controlPadConfig->mControlConfig.empty()) {
    auto config1 = new ControlPadConfig();
    ControlPad::controlPadConfig->mControlConfig.push_back(config1);
    auto config2 = new ControlPadConfig();
    config2->mRightButtonPos = Vec2(300, 60);
    config2->mScale = 0.3f;
    ControlPad::controlPadConfig->mControlConfig.push_back(config2);
    auto config3 = new ControlPadConfig();
    config2->mRightButtonPos = Vec2(320, 60);
    config2->mScale = 0.4f;
    ControlPad::controlPadConfig->mControlConfig.push_back(config3);
  }
  
  stringstream ss;
  ss << "{\n";
  
  INDENT_1
  ss << "\"ConfigIndex\" : " << ControlPad::controlPadConfig->mSelectedConfig << ",\n ";
  
  INDENT_1
  ss << "\"ConfigArray\": [ \n";
  for (auto it = ControlPad::controlPadConfig->mControlConfig.begin();
       it != ControlPad::controlPadConfig->mControlConfig.end(); ++it) {
    if (it != ControlPad::controlPadConfig->mControlConfig.begin()) {
      ss << ", \n";
    }
    INDENT_2
    ss << "{\n";
    INDENT_3
    ss << "\"desc\": \"" << (*it)->mDescription << "\", \n";
    INDENT_3
    ss << "\"scale\": " << (*it)->mScale << ", \n";
    INDENT_3
    ss << "\"leftButton\": " << vec2Str((*it)->mLeftButtonPos) << ", \n";
    INDENT_3
    ss << "\"rightButton\": " << vec2Str((*it)->mRightButtonPos) << ", \n";
    INDENT_3
    ss << "\"jumpButton\": " << vec2Str((*it)->mJumpButtonPos) << " \n";
    INDENT_2
    ss << "} ";
  }
  
  ss << "\n";
  INDENT_1
  ss << "] \n";
  
  ss << "}";
  
  auto fp = fopen(file, "w+");
  if (!fp) {
    CCLOGWARN("Warning: cannot access the map file : %s", file);
    return;
  }
  fprintf(fp, "%s", ss.str().c_str());
  fclose(fp);
}

void MapSerial::loadControlConfig(const char *file) {
  std::string fullPath = std::string(getConfigDir()) + "/" + file;
  auto buffer = FileUtils::getInstance()->getStringFromFile(fullPath);
  Document d;
  d.Parse<kParseDefaultFlags>(buffer.c_str());
  
  int configIndex = 0;
  if (d["ConfigIndex"].IsInt()) {
    configIndex = d["ConfigIndex"].GetInt();
    ControlPad::controlPadConfig->mSelectedConfig = configIndex;
  }
  
  std::string configKey = "ConfigArray";
  configKey += getLevelSuffix();
  
  if (CHECK_ARRAY(d, configKey.c_str())) {
    auto size = d[configKey.c_str()].Size();
    
    if (size > 0) {
      ControlPad::controlPadConfig->clearConfig();
    }
    
    for (auto i = 0; i < size; ++i) {
      auto config = new ControlPadConfig();
      auto &var = d[configKey.c_str()][i];
      
      config->mDescription = var["desc"].GetString();
      config->mScale = var["scale"].GetDouble();
      config->mLeftButtonPos = str2Vec(var["leftButton"].GetString());
      config->mRightButtonPos = str2Vec(var["rightButton"].GetString());
      config->mJumpButtonPos = str2Vec(var["jumpButton"].GetString());
      
      ControlPad::controlPadConfig->mControlConfig.push_back(config);
    }
  }
}

void MapSerial::loadControlConfig() {
  MapSerial::loadControlConfig("ControlConfig.json");
}
