//
//  HttpHelper.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/16/14.
//
//

#ifndef __JumpEdt__HttpHelper__
#define __JumpEdt__HttpHelper__

#include <stdio.h>
#include <string>
#include <vector>

#include "HttpClient.h"

struct MapResource {
  int id{0};
  std::string name;
  std::string author;
  std::string time;
  std::string content;
};

class HttpHelper {
public:
  static void getAllMaps();

  static void uploadMap(const std::string &name,
                        const std::string &author,
                        const std::string &time,
                        const std::string &content);

  static std::vector<MapResource> sAllMaps;
};

#endif /* defined(__JumpEdt__HttpHelper__) */
