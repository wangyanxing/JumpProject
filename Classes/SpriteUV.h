#ifndef __SpriteUV_H__
#define __SpriteUV_H__

#include "cocos2d.h"

//Sprite class where you can set a texture offset. Helps if you do UV animation on CCSprite's
//by @hermanjakobi, 2013 Jan

//Note:: Might not work with Texture Atlases or fliped sprites yet

class SpriteUV : public cocos2d::Sprite {
public:
  void setupTexParameters();

public:
  static SpriteUV *create(const std::string &filename);

  virtual bool initWithFile(const char *pszFilename);

  virtual bool initWithSpriteFrameName(const char *framename);

  void setUVOffset(cocos2d::Point offset);

  void setUVSize(float val);

  void setUVHeight(float val);

  void setUVWidth(float val);

  void resetUV();

  void flipUVX();

  void flipUVY();

  void rotateUV();

  CREATE_FUNC(SpriteUV);
};

#endif
