//
//  SceneSprite.h
//  jumpproj
//
//  Created by Yanxing Wang on 12/31/15.
//
//

#ifndef SceneSprite_h
#define SceneSprite_h

#include "SpriteUV.h"

class SceneSprite {
public:
  std::string Image;
  
  int ZOrder{3};
  
  cocos2d::Size Size;
  
  cocos2d::Vec2 Position;
  
  GLubyte Opacity{255};
  
  cocos2d::Color3B Color;
  
  void create();
  
  SpriteUV* getSprite() {
    return mSprite;
  }
  
  void clean();
  
private:
  SpriteUV *mSprite{nullptr};
};

#endif /* SceneSprite_h */
