//
//  JsonWriter.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 2/1/16.
//
//

#include "JsonWriter.h"
#include "JsonFormat.h"

#include <fstream>
#include <iostream>

using namespace std;
using namespace rapidjson;
USING_NS_CC;

JsonWriter::JsonWriter() {
  mWriter.SetIndent(' ', 2);
  mWriter.StartObject();
}

JsonWriter::~JsonWriter() {
}

void JsonWriter::save(const std::string& name) {
  mWriter.EndObject();

  auto fp = fopen(name.c_str(), "w+");
  if (!fp) {
    CCLOG("Warning: cannot access the file : %s", name.c_str());
    return;
  }
  fprintf(fp, "%s", mBuffer.GetString());
  fclose(fp);
}

std::string JsonWriter::getTimeString() {
  time_t rawtime;
  struct tm *ptm;
  time(&rawtime);
  ptm = gmtime(&rawtime);
  string timestr = asctime(ptm);
  timestr.resize(timestr.size() - 1);
  return timestr;
}

std::string JsonWriter::getAuthorString() {
  string author = "unknown";
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

void JsonWriter::writeTime() {
  mWriter.String(MAP_TIME);
  mWriter.String(getTimeString());
}

void JsonWriter::writeAuthor() {
  mWriter.String(MAP_AUTHOR);
  mWriter.String(getAuthorString());
}
