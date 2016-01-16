//
//  SerializationUtils.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#include "JsonParser.h"

#include <sstream>
#include <fstream>
#include <streambuf>

using namespace std;
using namespace rapidjson;
USING_NS_CC;

JsonParser::JsonParser(const std::string& fileName) {
  std::ifstream fileStream(fileName);
  if (!fileStream) {
    return;
  }

  std::string paletteBuffer((std::istreambuf_iterator<char>(fileStream)),
                            std::istreambuf_iterator<char>());

  CC_ASSERT(!mCurrentJson);
  mCurrentJson = new Document();
  ParseResult ok = mCurrentJson->Parse<kParseDefaultFlags>(paletteBuffer.c_str());
  if (!ok) {
    printf("JSON parse error: %d (%lu)\n", ok.Code(), ok.Offset());
    CC_SAFE_DELETE(mCurrentJson);
  }
}

JsonParser::~JsonParser() {
  CC_SAFE_DELETE(mCurrentJson);
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