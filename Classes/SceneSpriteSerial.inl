//
//  SceneSpriteSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 12/31/15.
//
//

void saveSceneSprite(stringstream& ss, const SceneSprite& sprite) {
  INDENT_3
  ss << "\"image\": " << "\"" << sprite.Image << "\"";
  RT_LINE
  
  INDENT_3
  ss << "\"size\": " << size2Str(sprite.Size);
  RT_LINE
  
  INDENT_3
  ss << "\"position\": " << vec2Str(sprite.Position);
  RT_LINE
  
  INDENT_3
  ss << "\"zorder\": " << sprite.ZOrder;
  RT_LINE
  
  INDENT_3
  ss << "\"opacity\": " << int(sprite.Opacity);
  RT_LINE
  
  INDENT_3
  ss << "\"color\": " << colorStr(sprite.Color);

  ss << std::endl;
}

SceneSprite loadSceneSprite(Document::ValueType &var) {
  SceneSprite ret;

  if (CHECK_STRING(var, "image")) {
    ret.Image = var["image"].GetString();
  } SHOW_WARNING
  
  if (CHECK_STRING(var, "size")) {
    ret.Size = str2Size(var["size"].GetString());
  } SHOW_WARNING
  
  if (CHECK_STRING(var, "position")) {
    ret.Position = str2Vec(var["position"].GetString());
  } SHOW_WARNING
  
  if (CHECK_INT(var, "zorder")) {
    ret.ZOrder = var["zorder"].GetInt();
  } SHOW_WARNING
  
  if (CHECK_INT(var, "opacity")) {
    ret.Opacity = GLubyte(var["opacity"].GetInt());
  } SHOW_WARNING
  
  if (CHECK_STRING(var, "color")) {
    ret.Color = str2Color(var["color"].GetString());
  } SHOW_WARNING
  
  return ret;
}
