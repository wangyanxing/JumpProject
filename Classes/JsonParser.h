//
//  SerializationUtils.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#ifndef JsonParser_h
#define JsonParser_h

#include "Prerequisites.h"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

typedef rapidjson::SizeType JsonSizeT;
typedef rapidjson::Document::ValueType JsonValueT;
typedef std::function<void(JsonSizeT, JsonValueT &)> ParseCallback;

class JsonParser {
public:
  JsonParser(const std::string& buffer);

  ~JsonParser();

  rapidjson::Document &getCurrentDocument() {
    return *mCurrentJson;
  }

  static std::string getBuffer(const std::string &file);

  static std::string getLevelSuffix();

  bool parseArray(JsonValueT &var, const std::string &key, ParseCallback func);

  bool parseArray(const std::string &key, ParseCallback func);

  operator bool() const {
    return mCurrentJson != nullptr;
  }

private:
  rapidjson::Document* mCurrentJson{nullptr};
};

#endif /* JsonParser_h */
