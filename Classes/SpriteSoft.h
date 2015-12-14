#ifndef __SpriteSoft_H__
#define __SpriteSoft_H__

#include "SpriteUV.h"

class SpriteSoft : public SpriteUV
{
public:

  static SpriteSoft* create(const std::string& filename);

  virtual bool initWithFile(const char *pszFilename);
  virtual bool initWithSpriteFrameName(const char *framename);

  CREATE_FUNC(SpriteSoft);

  void initPhysics(void);

  void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);

  virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

private:
  cocos2d::CustomCommand _customCommand;
};

#endif
