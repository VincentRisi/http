#ifndef _KVSTORE_H_
#define _KVSTORE_H_
#include "machine.h"

/**--------------------------------------------------------------------------
Author: Vince Risi - May 2014
=============================
This is a straight forward implementation of a key value store that is to
used as only in process. Bearing in mind that windows use dlls and unix 
uses shared objects we will use shared objects in the further discussion.
Due to the fact that shared objects behave differently across platforms, 
this single shared object allows for a single place to place key value 
data that can be used between other shared objects. One SO can put and
another SO can get. The main executable can also put and get and can also
just be assumed to be another object.
----------------------------------------------------------------------------- */
#ifndef OS_DECLSPEC
#if defined(M_AIX) || defined(M_GNU)
#define OS_DECLSPEC
#else
#define OS_DECLSPEC __declspec(dllexport)
#endif
#endif

extern "C" OS_DECLSPEC void kvstore_put(const char* key, int valueSize, const void* data);
extern "C" OS_DECLSPEC const void* kvstore_get(const char* key, int &valueSize);
extern "C" OS_DECLSPEC void kvstore_free(const char* key);

#if defined(M_AIX) || defined(M_GNU)
#define CLIAPI_QUERY_TIMEOUT "cliapiQueryTimout" 

inline void setCliapiQueryTimeout(unsigned int timeout)
{
  int timeoutSize = sizeof(unsigned int);
  kvstore_put(CLIAPI_QUERY_TIMEOUT, timeoutSize, &timeout);
}

inline unsigned int getCliapiQueryTimeout()
{
  int timeoutSize = sizeof(unsigned int);
  const void* data = kvstore_get(CLIAPI_QUERY_TIMEOUT, timeoutSize);
  if (data != 0)
    return *(unsigned int*)data;
  return 0;
}
#endif

#endif