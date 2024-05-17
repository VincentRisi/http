#ifndef _AMUTEX_H_
#define _AMUTEX_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(_AMUTEX_H_);

#include "machine.h"
#include <time.h>

class tAMutex
{
public:
#if defined(M_AIX) || defined(M_GNU)
  pthread_mutex_t mutex;
#endif

  tAMutex()         
  {
#if defined(M_AIX) || defined(M_GNU)
     pthread_mutex_init(&mutex, 0);
#endif
  }

  ~tAMutex()        
  {
#if defined(M_AIX) || defined(M_GNU)
     pthread_mutex_destroy(&mutex);
#endif     
  }
  
  void Lock()        
  {
#if defined(M_AIX) || defined(M_GNU)
     pthread_mutex_lock(&mutex);
#endif     
  }

  void Unlock()      
  {
#if defined(M_AIX) || defined(M_GNU)
     pthread_mutex_unlock(&mutex);
#endif     
  }
};

class tACond : public tAMutex
{
  bool rc;
public:

#if defined(M_AIX) || defined(M_GNU)
  pthread_cond_t cond;
#endif     

  tACond()
  {
#if defined(M_AIX) || defined(M_GNU)
    pthread_cond_init(&cond, 0);
#endif     
  }

  ~tACond()
  {
#if defined(M_AIX) || defined(M_GNU)
    pthread_cond_destroy(&cond);
#endif     
  }

  void Wait()
  {
#if defined(M_AIX) || defined(M_GNU)
    Lock();
    pthread_cond_wait(&cond, &mutex);
    Unlock();
#endif     
  }

  bool TimedWait(unsigned long secs)
  {
#if defined(M_AIX) || defined(M_GNU)
    struct timespec timeout;
    time((time_t*)&timeout.tv_sec);
    timeout.tv_sec += secs;
    Lock();
    rc = (pthread_cond_timedwait(&cond, &mutex, &timeout) == 0) ? true : false;
    Unlock();
#endif     
    return rc;
  }

  void Signal()
  {
#if defined(M_AIX) || defined(M_GNU)
    pthread_cond_signal(&cond);
#endif     
  }

  void Broadcast()
  {
#if defined(M_AIX) || defined(M_GNU)
    pthread_cond_broadcast(&cond);
#endif     
  }
};

#endif
