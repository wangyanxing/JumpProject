//
//  MapSerialUtils.inl
//  jumpproj
//
//  Created by Yanxing Wang on 12/23/15.
//
//

#ifndef MapSerialUtils_inl
#define MapSerialUtils_inl

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#   include <windows.h>
#   include <Lmcons.h>
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#   include <unistd.h>
#   include <sys/types.h>
#endif

#include <sstream>
#include <fstream>
#include <streambuf>

#define INDENT_1 ss<<"  ";
#define INDENT_2 ss<<"    ";
#define INDENT_3 ss<<"      ";
#define INDENT_4 ss<<"        ";
#define INDENT_5 ss<<"          ";
#define RT_LINE ss<<","<<std::endl;

#define CHECK_ARRAY(doc, member) (doc.HasMember(member) && doc[member].IsArray())
#define CHECK_NUMBER(doc, member) (doc.HasMember(member) && doc[member].IsNumber())
#define CHECK_BOOL(doc, member) (doc.HasMember(member) && doc[member].IsBool())
#define CHECK_INT(doc, member) (doc.HasMember(member) && doc[member].IsInt())
#define CHECK_STRING(doc, member) (doc.HasMember(member) && doc[member].IsString())

static void pushwarning() {
  CCLOGWARN("Invalid map file!");
}

#define SHOW_WARNING else{pushwarning();}

using namespace std;
using namespace rapidjson;

#include <regex>
#include <iostream>

using namespace cocos2d;

std::string MapSerial::CurrentEditingFile;

static const char *getLevelSuffix() {
  static std::string suffix;
  auto framesize = VisibleRect::getFrameSize();
  float ratio = framesize.width / framesize.height;
  
  if (ratio > 1.7) { // wide
    // ok, do nothing
    suffix = "";
  } else if (ratio < 1.4) { // ipad
    suffix = "_pad";
  } else { //ip4
    suffix = "_ip4";
  }
  return suffix.c_str();
}

std::string colorStr(const Color3B &color) {
  char temp[100];
  sprintf(temp, "\"#%02X%02X%02X\"", color.r, color.g, color.b);
  return temp;
}

Color3B str2Color(std::string hex) {
  int rgb[3];
  stringstream ss;
  string str;
  
  // Drop a hash if the value has one
  if (hex[0] == '#') {
    hex.erase(0, 1);
  }
  
  int size = (int) hex.size();
  for (int i = 0; i < 3; i++) {
    // Determine 3 or 6 character format.
    if (size == 3) {
      str = string(2, hex[i]);
    } else if (size == 6) {
      str = hex.substr(i * 2, 2);
    } else {
      break;
    }
    ss << std::hex << str;
    ss >> rgb[i];
    ss.clear();
  }
  return Color3B(rgb[0], rgb[1], rgb[2]);
}

std::string vec2Str(const Vec2 &v) {
  char temp[100];
  sprintf(temp, "\"%g,%g\"", v.x, v.y);
  return temp;
}

Vec2 str2Vec(const std::string &str) {
  auto v = PathLib::stringSplit(str, ",");
  if (v.size() != 2) {
    CCLOG("Invalid vec2: %s", str.c_str());
    return Vec2::ZERO;
  }
  return Vec2(atof(v[0].c_str()), atof(v[1].c_str()));
}

std::string size2Str(const Size &v) {
  char temp[100];
  sprintf(temp, "\"%g,%g\"", v.width, v.height);
  return temp;
}

Size str2Size(const std::string &str) {
  auto v = PathLib::stringSplit(str, ",");
  if (v.size() != 2) {
    CCLOGWARN("Invalid vec2: %s", str.c_str());
    return Size::ZERO;
  }
  return Size(atof(v[0].c_str()), atof(v[1].c_str()));
}

std::string bool2Str(bool v) {
  return v ? "true" : "false";
}

std::string followMode2Str(FollowMode v) {
  static std::string names[] = {
    "\"CENTER\"",
    "\"UP\"",
    "\"DOWN\"",
    "\"LEFT\"",
    "\"RIGHT\""
  };
  return names[v];
}

FollowMode str2FollowMode(const string &v) {
  static std::map<std::string, FollowMode> modes = {
    {"CENTER", F_CENTER},
    {"UP",     F_UP},
    {"DOWN",   F_DOWN},
    {"LEFT",   F_LEFT},
    {"RIGHT",  F_RIGHT}
  };
  
  if (!modes.count(v)) {
    CCLOGWARN("Invalid kind: %s", v.c_str());
    return F_CENTER;
  }
  return modes[v];
}

std::string kind2Str(BlockKind v) {
  static std::string names[] = {
    "\"HERO\"",
    "\"BLOCK\"",
    "\"DEATH\"",
    "\"DEATH_CIRCLE\"",
    "\"BUTTON\"",
    "\"PUSHABLE\"",
    "\"FORCEFIELD\""
  };
  return names[v];
}

BlockKind str2Kind(const string &v) {
  static std::map<std::string, BlockKind> kinds = {
    {"HERO",         KIND_HERO},
    {"BLOCK",        KIND_BLOCK},
    {"DEATH",        KIND_DEATH},
    {"DEATH_CIRCLE", KIND_DEATH_CIRCLE},
    {"BUTTON",       KIND_BUTTON},
    {"PUSHABLE",     KIND_PUSHABLE}
  };
  
  if (!kinds.count(v)) {
    CCLOGWARN("Invalid kind: %s", v.c_str());
    return KIND_BLOCK;
  }
  return kinds[v];
}

std::string direction2Str(Button::PushDir v) {
  static std::string names[] = {
    "\"UP\"",
    "\"DOWN\"",
    "\"LEFT\"",
    "\"RIGHT\""
  };
  return names[v];
}

Button::PushDir str2Direction(const string &v) {
  static std::map<std::string, Button::PushDir> kinds = {
    {"UP",    Button::DIR_UP},
    {"DOWN",  Button::DIR_DOWN},
    {"LEFT",  Button::DIR_LEFT},
    {"RIGHT", Button::DIR_RIGHT}
  };
  
  if (!kinds.count(v)) {
    CCLOGWARN("Invalid push direction: %s", v.c_str());
    return Button::DIR_DOWN;
  }
  return kinds[v];
}


#endif /* MapSerialUtils_inl */
