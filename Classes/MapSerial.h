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
    
    static void saveMap(const char* file);
	static void savePalette(const char* file);

#if EDITOR_MODE
    static void loadMap(bool local = true);
#endif
    
    static void loadMap(const char* file);
    
    static void loadLastEdit();
    
    static void saveRemoteMaps();
    
    static void afterLoadRemoteMaps();
    
    static const char* getMapDir();
};

#endif /* defined(__JumpEdt__MapSerial__) */
