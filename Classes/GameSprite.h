//
//  GameSprite.h
//  jumpproj
//
//  Created by Yanxing Wang on 2/1/16.
//
//

#ifndef GameSprite_h
#define GameSprite_h

#include "Prerequisites.h"
#include "SpriteUV.h"
#include "JsonParser.h"
#include "JsonWriter.h"

class GameSprite {
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

  void load(JsonValueT &json);

  void save(JsWriter &writer);

private:
  SpriteUV *mSprite{nullptr};
};

#endif /* GameSprite_h */
