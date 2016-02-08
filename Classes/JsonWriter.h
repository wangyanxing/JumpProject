//
//  JsonWriter.h
//  jumpproj
//
//  Created by Yanxing Wang on 2/1/16.
//
//

#ifndef JsonWriter_h
#define JsonWriter_h

#include "Prerequisites.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> JsWriter;

class JsonWriter {
public:
  JsonWriter();

  ~JsonWriter();

  JsWriter &getWriter() {
    return mWriter;
  }

  void save(const std::string& fileName);

  void writeTime();

  void writeAuthor();

  static std::string getTimeString();

  static std::string getAuthorString();

private:
  rapidjson::StringBuffer mBuffer;

  JsWriter mWriter{mBuffer};
};

#endif /* JsonWriter_h */
