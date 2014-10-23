//
//  Shadows.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#ifndef __JumpEdt__Shadows__
#define __JumpEdt__Shadows__

#include "cocos2d.h"

class BlockBase;
namespace cocos2d {
    class DrawNodeEx;
}

class ShadowManager {
public:
    
    ShadowManager(cocos2d::Node* parentNode);
    ~ShadowManager();
    
    void update(const std::map<int,BlockBase*>& blocks);
    
    cocos2d::Vec2 mLightPos;
    cocos2d::DrawNodeEx* mRenderer{ nullptr };
};


#endif /* defined(__JumpEdt__Shadows__) */
