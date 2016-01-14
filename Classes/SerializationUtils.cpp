//
//  SerializationUtils.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#include "SerializationUtils.h"

#include <sstream>
#include <fstream>
#include <streambuf>

using namespace std;
using namespace rapidjson;
USING_NS_CC;

Document* SerializationUtils::beginJson(const std::string& fileName) {
  std::ifstream fileStream(fileName);
  if (!fileStream) {
    return nullptr;
  }

  std::string paletteBuffer((std::istreambuf_iterator<char>(fileStream)),
                            std::istreambuf_iterator<char>());

  CC_ASSERT(!mCurrentJson);
  mCurrentJson = new Document();
  ParseResult ok = mCurrentJson->Parse<kParseDefaultFlags>(paletteBuffer.c_str());
  if (!ok) {
    printf("JSON parse error: %d (%lu)\n", ok.Code(), ok.Offset());
    CC_SAFE_DELETE(mCurrentJson);
    return nullptr;
  }
  return mCurrentJson;
}

void SerializationUtils::endJson() {
  CC_SAFE_DELETE(mCurrentJson);
}

bool SerializationUtils::parseArray(const std::string &key, ParseCallback func) {
  CC_ASSERT(mCurrentJson);
  return parseArray(*mCurrentJson, key, func);
}

bool SerializationUtils::parseArray(JsonValueT &var,
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

cocos2d::Color3B SerializationUtils::parseColor(JsonValueT& val) {
  return parseColor(val.GetString());
}

cocos2d::Color3B SerializationUtils::parseColor(const char *hex) {
  int rgb[3];
  stringstream ss;
  string str;

  // Drop a hash if the value has one
  std::string hexVal = hex;
  if (hexVal[0] == '#') {
    hexVal.erase(0, 1);
  }

  int size = (int) hexVal.size();
  for (int i = 0; i < 3; i++) {
    // Determine 3 or 6 character format.
    if (size == 3) {
      str = string(2, hexVal[i]);
    } else if (size == 6) {
      str = hexVal.substr(i * 2, 2);
    } else {
      break;
    }
    ss << std::hex << str;
    ss >> rgb[i];
    ss.clear();
  }
  return Color3B(rgb[0], rgb[1], rgb[2]);
}
