//
//  SpriteUV.h
//  JumpEdt
//
//  Created by Yanxing Wang.
//
//

#ifndef __SpriteUV_H__
#define __SpriteUV_H__

class SpriteUV : public cocos2d::Sprite {
public:
  CREATE_FUNC(SpriteUV);

  static SpriteUV *create(const std::string &filename);

  bool initWithFile(const char *pszFilename);

  bool initWithSpriteFrameName(const char *framename);

  void setupTexParameters();

  void setUVOffset(cocos2d::Point offset);

  void setUVSize(float val);

  void setUVHeight(float val);

  void setUVWidth(float val);

  void resetUV();

  void flipUVX();

  void flipUVY();

  void rotateUV();
};

#endif
