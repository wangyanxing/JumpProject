//
//  SceneSpriteSerial.inl
//  jumpproj
//
//  Created by Yanxing Wang on 12/31/15.
//
//

void saveSceneSprite(stringstream& ss, const SceneSprite& sprite) {
  WRITE_STR_E(3, "image", sprite.Image);
  WRITE_VEC_E(3, "size", sprite.Size);
  WRITE_VEC_E(3, "position", sprite.Position);
  WRITE_NUM_E(3, "zorder", sprite.ZOrder);
  WRITE_NUM_E(3, "opacity", int(sprite.Opacity));
  WRITE_COL_R(3, "color", sprite.Color);
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
