//
//  ControlConfigSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 1/1/16.
//
//

#if EDITOR_MODE

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

  BEGIN_OBJECT(0);
  WRITE_NUM_E(1, "ConfigIndex", ControlPad::controlPadConfig->mSelectedConfig);
  BEGIN_ARRAY(1, "ConfigArray");

  for (auto it = ControlPad::controlPadConfig->mControlConfig.begin();
       it != ControlPad::controlPadConfig->mControlConfig.end(); ++it) {
    BEGIN_OBJECT(2)

    WRITE_STR_E(3, "desc", (*it)->mDescription);
    WRITE_NUM_E(3, "scale", (*it)->mScale);
    WRITE_VEC_E(3, "leftButton", (*it)->mLeftButtonPos);
    WRITE_VEC_E(3, "rightButton", (*it)->mRightButtonPos);
    WRITE_VEC_E(3, "jumpButton", (*it)->mJumpButtonPos);

    END_OBJECT(2, next(it) != ControlPad::controlPadConfig->mControlConfig.end());
  }

  END_ARRAY(1, NO_COMMA);
  END_OBJECT(0, NO_COMMA);

  saveToFile(file, ss);
}
#endif

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
