//
//  LevelScene.h
//  JumpEdt
//
//
//

#ifndef __JumpEdt__LevelScene__
#define __JumpEdt__LevelScene__

#include <iostream>
#include "cocos2d.h"

class LevelScene : public cocos2d::Scene
{
public:  

    virtual bool init();  
    CREATE_FUNC(LevelScene);
    static LevelScene *getInstance();
    
private:
    static LevelScene *instance;
}; 
#endif
