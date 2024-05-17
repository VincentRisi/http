#include "versions.h"
#define _VERSION_NO "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
COMPILED_VERSION(_VERSION_NO);

#if defined(M_AIX) || defined(M_GNU)

#include <pthread.h>

#endif

#include "athread.h"

tAThread::tAThread(bool aSelfDestruct)
: SelfDestruct(aSelfDestruct)
, active(false)
, Status(Created)
{
}

tAThread::~tAThread()
{
  Terminate();
  cond.Signal();
}

void *ThreadEntry(void *aThread)
{
  tAThread* Thread = (tAThread*)aThread;

  Thread->Status = tAThread::Running;
  Thread->Run();
  Thread->Status = tAThread::Finished;
#if defined(M_AIX) || defined(M_GNU)
  pthread_detach(pthread_self());// geoff 17-10-2000
  pthread_exit(0);
#endif
  if (Thread->SelfDestruct == true)
    delete Thread;
  return 0;
}

void tAThread::Start()
{
#if defined(M_AIX) || defined(M_GNU)
  if (Status != Running)
  {
    active = true;
    int rc = pthread_create(&thread, 0, ThreadEntry, this);
  }
#endif
}



