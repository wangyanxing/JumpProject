//
//  SerializationUtils.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#include "JsonParser.h"
#include "VisibleRect.h"

#include <sstream>
#include <fstream>
#include <streambuf>

using namespace std;
using namespace rapidjson;
USING_NS_CC;

JsonParser::JsonParser(const std::string& buffer) {
  CC_ASSERT(!mCurrentJson);
  mCurrentJson = new Document();
  ParseResult ok = mCurrentJson->Parse<kParseDefaultFlags>(buffer.c_str());
  if (!ok) {
    printf("JSON parse error: %d (%lu)\n", ok.Code(), ok.Offset());
    CC_SAFE_DELETE(mCurrentJson);
  }
}

JsonParser::~JsonParser() {
  CC_SAFE_DELETE(mCurrentJson);
}

std::string JsonParser::getBuffer(const std::string &file) {
  return FileUtils::getInstance()->getStringFromFile(file);
}

std::string JsonParser::getLevelSuffix() {
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

bool JsonParser::parseArray(const std::string &key, ParseCallback func) {
  CC_ASSERT(mCurrentJson);
  return parseArray(*mCurrentJson, key, func);
}

bool JsonParser::parseArray(JsonValueT &var,
                                    const std::string &key,
                                    ParseCallback func) {
  if (!var.HasMember(key.c_str())) {
    return false;
  }
  SizeType size = var[key.c_str()].Size();
  for (SizeType i = 0; i < size; ++i) {
    func(i, var[key.c_str()][i]);
  }
  return true;
}
