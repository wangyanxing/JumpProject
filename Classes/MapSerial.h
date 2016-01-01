//
//  MapSerial.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/16/14.
//
//

#ifndef __JumpEdt__MapSerial__
#define __JumpEdt__MapSerial__

#include "Defines.h"

class MapSerial {
public:
#if EDITOR_MODE
  static void saveMap();
#endif

  static void saveMap(const char *file);

  static void savePalette(const char *file);

#if EDITOR_MODE
  static void loadMap(bool local = true);
#endif

  static void loadMap(const char *file);
  
  static void loadPalette(const char *file);

  static void loadLastEdit();

  static void saveRemoteMaps();

  static void afterLoadRemoteMaps();

  static const char *getMapDir();

  static const char *getConfigDir();

  static void saveControlConfig(const char *file);

  static void loadControlConfig(const char *file);

  static void loadControlConfig();

  static std::string CurrentEditingFile;
};

#endif /* defined(__JumpEdt__MapSerial__) */
