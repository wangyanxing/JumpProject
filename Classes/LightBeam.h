//
//  LightBeam.h
//  JumpEdt
//
//  Created by Yanxing Wang on 11/3/14.
//
//

#ifndef __JumpEdt__LightBeam__
#define __JumpEdt__LightBeam__

#include "DrawNodeEx.h"
#include "cocos2d.h"

struct LightBeamDesc {
    int zorder{ 50 };
    float segSizeUpper{ 13 };
    float segSizeLower{ 8 };
    int segNums{ 8 };
    float height{ 700 };
    float uvScale{ 1 };
    float uvUpper{ 0.089 };
    float uvLower{ 0.961 };
    float uvStart{ 0 };
    float uvSpeed{ 0.1 };
    cocos2d::Color4B color{255,255,255,255};
    std::string texture{ "images/lightbeam.png" };
    
    static LightBeamDesc DEFAULT;
};

////////////////////////////////////////////////////////////////////////

class LightBeam {
public:
    
    LightBeam(const LightBeamDesc& desc = LightBeamDesc::DEFAULT) {
        init(desc);
    }
    
    ~LightBeam() {
        destroy();
    }
    
    void init(const LightBeamDesc& desc);
    
    void update(float dt);
    
    void destroy();
    
    cocos2d::DrawNodeEx* getNode() {return mNode;}

    LightBeamDesc mDesc;
    
private:
    
    float mUVAnimTimer{ 0 };
    
    cocos2d::DrawNodeEx* mNode{ nullptr };
};

#endif /* defined(__JumpEdt__LightBeam__) */
