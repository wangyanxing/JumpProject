//
//  Singleton.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef Singleton_h
#define Singleton_h

/**
 * Nice singleton implementation, from http://stackoverflow.com/a/1008289
 */
#define DECL_SINGLETON(S) \
public:\
  static S& instance() { \
    static S ins; \
    return ins; \
  } \
private: \
  S() {}; \
  ~S() {release();}\
public: \
  S(S const&) = delete; \
  void operator=(S const&) = delete;

#endif /* Singleton_h */
