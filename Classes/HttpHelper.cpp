//
//  HttpHelper.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/16/14.
//
//

#include "HttpHelper.h"
#include "PathLib.h"

#include "external/json/document.h"
#include "external/json/rapidjson.h"

#include <time.h>
#include <regex>
#include <fstream>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#   include <windows.h>
#   include <Lmcons.h>

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#   include <unistd.h>
#   include <sys/types.h>
#endif

#define RUN_LOCAL 0
#if RUN_LOCAL
#   define GET_URL "https://0.0.0.0:3000/maps.json"
#   define PUSH_URL "https://0.0.0.0:3000/maps.json"
#else
#   define GET_URL "https://jump-editor.herokuapp.com/maps.json"
#   define PUSH_URL "https://jump-editor.herokuapp.com/maps.json"
#endif

using namespace rapidjson;
USING_NS_CC;

std::vector<MapResource> HttpHelper::sAllMaps;

void HttpHelper::getAllMaps() {
  cocos2d::network::HttpRequest *request = new cocos2d::network::HttpRequest();
  request->setUrl(GET_URL);
  request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
  request->setResponseCallback([&](cocos2d::network::HttpClient *client,
                                   cocos2d::network::HttpResponse *response) {
      if (response->isSucceed() && response->getResponseCode() == 200) {
        CCLOG("[HTTP] Got reponse from server: HTTP 200 OK");

        std::vector<char> *buf = response->getResponseData();
        char *buffer = new char[buf->size() + 1];
        memcpy(buffer, &buf->at(0), buf->size());
        buffer[buf->size()] = '\0';

        Document d;
        d.Parse<kParseDefaultFlags>(buffer);

        sAllMaps.clear();
        for (SizeType i = 0; i < d.Size(); ++i) {
          auto &v = d[i];
          sAllMaps.push_back(MapResource());

          if (v["id"].IsInt()) {
            sAllMaps.back().id = v["id"].GetInt();
          }
          if (v["name"].IsString()) {
            sAllMaps.back().name = v["name"].GetString();
          }
          if (v["author"].IsString()) {
            sAllMaps.back().author = v["author"].GetString();
          }
          if (v["time"].IsString()) {
            sAllMaps.back().time = v["time"].GetString();
          }
          if (v["content"].IsString()) {
            sAllMaps.back().content = v["content"].GetString();
          }
        }

        delete[] buffer;
        afterLoadRemoteMaps();
      } else {
        CCLOG("[HTTP] GET ERROR(code %ld): %s",
              response->getResponseCode(),
              response->getErrorBuffer());
      }
  });

  request->setTag("GET get_maps");
  cocos2d::network::HttpClient::getInstance()->send(request);
  CCLOG("[HTTP] Downloading all level files from server..");
  request->release();
}

void HttpHelper::uploadMap(const std::string &name,
                           const std::string &author,
                           const std::string &time,
                           const std::string &content) {
  cocos2d::network::HttpRequest *request = new cocos2d::network::HttpRequest();
  request->setUrl(PUSH_URL);
  request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
  request->setResponseCallback([&](cocos2d::network::HttpClient *client,
                                   cocos2d::network::HttpResponse *response) {
    if (response->isSucceed()) {
      CCLOG("[HTTP] Map uploaded");
    } else {
      CCLOG("[HTTP] POST ERROR(code %ld): %s",
            response->getResponseCode(),
            response->getErrorBuffer());
    }
  });

  std::string t = "map[name]=";
  t += name;
  t += "&map[author]=";
  t += author;
  t += "&map[time]=";
  t += time;
  t += "&map[content]=";
  t += content;
  t += "&commit=Create+Map";

  request->setRequestData(t.c_str(), t.size());
  request->setTag("POST upload_map");
  CCLOG("[HTTP] Uploading level file (%s) to server..", name.c_str());
  cocos2d::network::HttpClient::getInstance()->send(request);
  request->release();
}

void HttpHelper::afterLoadRemoteMaps() {
  std::smatch base_match;
  std::regex rx("^(\\S)*\\.json$");

  for (auto &m : HttpHelper::sAllMaps) {
    auto ret = std::regex_match(m.name, base_match, rx);
    if (!ret) {
      m.name += ".json";
    }

    std::string fullpath = PathLib::getMapDir();
    if (fullpath.size() == 0) {
      CCLOGWARN("Warning: cannot locate the maps folder!");
      continue;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    fullpath = FileUtils::getInstance()->getWritablePath() + m.name;
#else
    fullpath += "/remote/" + m.name;
#endif

    std::ofstream fileStream(fullpath);
    fileStream << m.content;
  }
}
