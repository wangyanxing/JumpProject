//
//  SerializationUtils.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/14/16.
//
//

#ifndef SerializationUtils_h
#define SerializationUtils_h

#include "Prerequisites.h"
#include "Singleton.h"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

typedef rapidjson::SizeType JsonSizeT;
typedef rapidjson::Document::ValueType JsonValueT;
typedef std::function<void(JsonSizeT, JsonValueT &)> ParseCallback;

class SerializationUtils {
public:
  DECL_SIMPLE_SINGLETON(SerializationUtils)

  rapidjson::Document* beginJson(const std::string& fileName);

  void endJson();

  rapidjson::Document &getCurrentDocument() {
    return *mCurrentJson;
  }

  bool parseArray(JsonValueT &var, const std::string &key, ParseCallback func);

  bool parseArray(const std::string &key, ParseCallback func);

  cocos2d::Color3B parseColor(const char *hex);

  cocos2d::Color3B parseColor(JsonValueT& val);

private:
  rapidjson::Document* mCurrentJson{nullptr};
};

#endif /* SerializationUtils_h */
