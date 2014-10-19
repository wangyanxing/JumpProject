//
//  MapSerial.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/16/14.
//
//

#ifndef __JumpEdt__MapSerial__
#define __JumpEdt__MapSerial__


class MapSerial {
public:
    
    static void saveMap();
    
    static void saveMap(const char* file);
    
    static void loadMap(bool local = true);
    
    static void loadMap(const char* file);
    
    static void loadLastEdit();
    
    static void saveRemoteMaps();
    
    static void afterLoadRemoteMaps();
    
    static const char* getMapDir();
};

#endif /* defined(__JumpEdt__MapSerial__) */
