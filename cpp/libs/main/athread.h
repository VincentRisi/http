#ifndef _ATHREAD_H_
#define _ATHREAD_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(_ATHREAD_H_);

#if defined(M_AIX) || defined(M_GNU)

#include <pthread.h>

#endif

#include "amutex.h"

class tAThread
{
public:
  enum      eStatus {Created, Running, Finished};
  eStatus   Status;
  bool      Active() {return active;}
  bool      active;
  bool      SelfDestruct;
#if defined(M_AIX) || defined(M_GNU)
  pthread_t thread;
#endif
  tAThread(bool aSelfDestruct=false);
  void Start();
  void WaitForExit() {cond.Wait();}
  virtual void Run() = 0;
  virtual void Terminate() {active = false;}

  /*void Yield() // I Give up - need more info
  {
    #ifdef _AIX_PTHREADS_D7
    pthread_yield();
    #endif
  } */

  friend void *ThreadEntry(void *aThread);
protected:
  virtual ~tAThread();
  tACond    cond;
};

#endif
