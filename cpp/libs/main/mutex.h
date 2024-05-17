#ifndef _MUTEX_H_
#define _MUTEX_H_
#include "machine.h"
#include "xcept.h"

/**--------------------------------------------------------------------------
Author: Vince Risi - May 2014
=============================
This is a rework of a number of semaphore classes, libti has tsem which
is the basis for the unix semaphore usage, libmain has autoSemaphore and
sharedSemaphore which are not based on a mutex principle but a post/wait
stategy - which allows for multiple instances depending on how many posts 
are active. They also all do not do a windows implementation. The other
feature added is the semaphores are timed - and on the constructor seconds
and nanoseconds can be provided. If 0 seconds and nano seconds are specified
then the wait will block indefinitely. 


Usage is quite simple:
1)
  #include "mutex.h"

2)
- construct a semaphore (int in unix and a HANDLE in windows) using either
  semaphore = Mutex::makePrivate();   // single process (or fork parent/child)
- or
  semaphore = Mutex::makeNamed(name); // In windows this may be just a unique name        
                                      // In unix this must be an existing file or
                                      // a valid file that can be created.
3)
- where you want to set your critical section, use a stack created resource like
  Mutex wait(semaphore);              // When this goes out of scope the destructor
                                      // will release. The constructor acquires.   
----------------------------------------------------------------------------- */

enum EMutex
{ eMNoError = 0
, eMutexAcquireFailed
, eMutexFtokFailed
, eMutexSemGetFailed
, eMutexSemKeyFailed
, eMutexReleaseFailed
};

#define XMUTEX(err) XMutex(__FILE__, __LINE__, err)
class XMutex : public XCept
{
public:
  static char *getText(long err)
  {
    switch (err)
    {
    case eMNoError:            return "No Error";
    case eMutexAcquireFailed:  return "Mutex Acquire Failure";
    case eMutexFtokFailed:     return "Mutex File does not exist or cannot be opened Failure";
    case eMutexSemGetFailed:   return "Mutex File semget Failure";
    case eMutexSemKeyFailed:   return "Mutex File construction of semkey Failure";
    default:                   return "Unknown exception";
    }
  }
  XMutex(const char* aFname, int aLine, long aError)
  : XCept(aFname, aLine, "XMutex", aError)
  {
    osErr << aFname << "(" << aLine << ")";
    if (aError)
      osErr << "Error(" << aError << ") " << getText(aError);
    osErr << ends;
  }
  XMutex(const XMutex& aX)
  : XCept(aX)
  {}
};

struct Mutex
{
  int secs, nanosecs;
  bool failed;
#if defined(M_AIX) || defined(M_GNU)
  static int makePrivate();
  static int makeNamed(const char* name);
  static void remove(int semaphore);
  int semaphore;
  Mutex(int semaphore, int secs=0, int nanosecs=0)
#else
  static HANDLE makePrivate();
  static HANDLE makeNamed(const char* name);
  static void remove(HANDLE semaphore);
  HANDLE semaphore;
  Mutex(HANDLE semaphore, int secs=0, int nanosecs=0)
#endif
  {
    this->semaphore = semaphore;
    this->secs = secs;
    this->nanosecs = nanosecs;
    this->failed = false;
    acquire();
  }
  ~Mutex()
  {
    release();
  }
  void acquire();
  void release();
};

#endif