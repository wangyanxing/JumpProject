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

#define WITH_COMMA true
#define NO_COMMA false

#define INDENT_1 ss<<"  ";
#define INDENT_2 ss<<"    ";
#define INDENT_3 ss<<"      ";
#define INDENT_4 ss<<"        ";
#define INDENT_5 ss<<"          ";
#define RT_LINE ss<<","<<std::endl;
#define RT_LINE_NOCOM ss<<std::endl;

#define BEGIN_OBJECT(ind) indent(ss,ind);ss<<"{\n";
#define BEGIN_ARRAY(ind,key) indent(ss,ind);ss<<'"'<<key<<"\": [\n";
#define END_ARRAY(ind,comma) indent(ss,ind);endObject(ss,']',comma);
#define END_OBJECT(ind,comma) indent(ss,ind);endObject(ss,'}',comma);

#define CHECK_DEFAULT(cond,def) if(!checkEqual(cond,def))

/**
 * Write a key value pair.
 */
#define WRITE_STR(ind,key,val) indent(ss,ind);ss<<'"'<<key<<"\": "<<'"'<<val<<'"';
#define WRITE_NUM(ind,key,val) indent(ss,ind);ss<<'"'<<key<<"\": "<<val;
#define WRITE_COL(ind,key,val) indent(ss,ind);ss<<'"'<<key<<"\": "<<colorStr(val);
#define WRITE_VEC(ind,key,val) indent(ss,ind);ss<<'"'<<key<<"\": "<<vec2Str(val);
#define WRITE_SIZ(ind,key,val) indent(ss,ind);ss<<'"'<<key<<"\": "<<size2Str(val);
#define WRITE_BOL(ind,key,val) indent(ss,ind);ss<<'"'<<key<<"\": "<<bool2Str(val);

/**
 * Write a key value pair with ending comma.
 */
#define WRITE_GEN_E(ind,key,val) WRITE_NUM(ind,key,val) RT_LINE
#define WRITE_STR_E(ind,key,val) WRITE_STR(ind,key,val) RT_LINE
#define WRITE_NUM_E(ind,key,val) WRITE_NUM(ind,key,val) RT_LINE
#define WRITE_COL_E(ind,key,val) WRITE_COL(ind,key,val) RT_LINE
#define WRITE_VEC_E(ind,key,val) WRITE_VEC(ind,key,val) RT_LINE
#define WRITE_SIZ_E(ind,key,val) WRITE_VEC(ind,key,val) RT_LINE
#define WRITE_BOL_E(ind,key,val) WRITE_BOL(ind,key,val) RT_LINE

/**
 * Write a key value pair without ending comma.
 */
#define WRITE_GEN_R(ind,key,val) WRITE_NUM(ind,key,val) RT_LINE_NOCOM
#define WRITE_STR_R(ind,key,val) WRITE_STR(ind,key,val) RT_LINE_NOCOM
#define WRITE_NUM_R(ind,key,val) WRITE_NUM(ind,key,val) RT_LINE_NOCOM
#define WRITE_COL_R(ind,key,val) WRITE_COL(ind,key,val) RT_LINE_NOCOM
#define WRITE_VEC_R(ind,key,val) WRITE_VEC(ind,key,val) RT_LINE_NOCOM
#define WRITE_SIZ_R(ind,key,val) WRITE_VEC(ind,key,val) RT_LINE_NOCOM
#define WRITE_BOL_R(ind,key,val) WRITE_BOL(ind,key,val) RT_LINE_NOCOM

/**
 * Wirte as an array element(no key).
 */
#define WRITE_ARR_STR(ind,val,comma) indent(ss,ind);ss<<'"'<<val<<'"'; endObject(ss,0,comma);
#define WRITE_ARR_VEC(ind,val,comma) indent(ss,ind);ss<<vec2Str(val); endObject(ss,0,comma);

/**
 * Check a given key name is existed in the Json document.
 */
#define CHECK_ARRAY(doc, member) (doc.HasMember(member) && doc[member].IsArray())
#define CHECK_NUMBER(doc, member) (doc.HasMember(member) && doc[member].IsNumber())
#define CHECK_BOOL(doc, member) (doc.HasMember(member) && doc[member].IsBool())
#define CHECK_INT(doc, member) (doc.HasMember(member) && doc[member].IsInt())
#define CHECK_STRING(doc, member) (doc.HasMember(member) && doc[member].IsString())

/**
 * If can't locate the key, show a warning messasge.
 */
#define SHOW_WARNING else {CCLOGWARN("Invalid map file!");}

using namespace std;
using namespace rapidjson;

#include <regex>
#include <iostream>

USING_NS_CC;

template<typename T>
bool checkEqual(const T &a, const T &b) {
  return a == b;
}

template<>
bool checkEqual(const float &a, const float &b) {
  return fabs(a - b) < FLT_EPSILON;
}

static void indent(stringstream &ss, int level) {
  if (level > 0) {
    ss << std::string(level * 2, ' ');
  }
}

static void endObject(stringstream &ss, char c, bool comma) {
  if (c != 0) {
    ss << c;
  }
  if (comma) {
    ss << ",";
  }
  ss << "\n";
}

static void saveToFile(const char *file, const stringstream &ss) {
  auto fp = fopen(file, "w+");
  if (!fp) {
    CCLOG("Warning: cannot access the file : %s", file);
    return;
  }
  fprintf(fp, "%s", ss.str().c_str());
  fclose(fp);
}

template<typename T>
std::string toJsonArray(const std::vector<T> &array) {
  std::ostringstream stream;
  stream << "[";
  for (size_t i = 0; i < array.size(); ++i) {
    if (i != 0) {
      stream << ',';
    }
    stream << '\"' << array[i] << '\"';
  }
  stream << "]";
  return stream.str();
}

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

/**
 * Convert to hex color string.
 */
std::string colorStr(const Color3B &color) {
  char temp[100];
  sprintf(temp, "\"#%02X%02X%02X\"", color.r, color.g, color.b);
  return temp;
}

/**
 * Convert a hex color string back to Color3B object.
 */
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

/**
 * Convert a vec2 to string.
 */
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

/**
 * Get the user name.
 */
std::string getComputerUser() {
  std::string author = "unknown";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  TCHAR username[UNLEN + 1];
  DWORD size = UNLEN + 1;
  GetUserName((TCHAR *) username, &size);
  author = username;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
  author = getlogin();
#endif
  return author;
}

/**
 * Get current time as a string.
 */
std::string getTimeStr() {
  time_t rawtime;
  struct tm *ptm;
  time(&rawtime);
  ptm = gmtime(&rawtime);
  std::string timestr = asctime(ptm);
  timestr.resize(timestr.size() - 1);
  return timestr;
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

std::string lightType2Str(ShadowManager::LightType v) {
  static std::string names[] = {
    "\"POINT\"",
    "\"DIR\""
  };
  return names[v];
}

ShadowManager::LightType str2lightType(const string &v) {
  static std::map<std::string, ShadowManager::LightType> kinds = {
    {"POINT", ShadowManager::LIGHT_POINT},
    {"DIR", ShadowManager::LIGHT_DIR}
  };

  if (!kinds.count(v)) {
    CCLOGWARN("Invalid light type: %s", v.c_str());
    return ShadowManager::LIGHT_POINT;
  }
  return kinds[v];
}

#endif /* MapSerialUtils_inl */
