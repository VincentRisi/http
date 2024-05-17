/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2007/01/15
 Name   : AutoSemaphore
 Descr  : A group semephore classes that make using semaphores easy
==============================================================================*/

#include "autosemaphore.h"

#include <errno.h>
#include <string.h>

#if defined(M_AIX) || defined(M_GNU)

   #include <unistd.h>
   #include <fcntl.h>
   
#endif
   
#ifdef M_STDLIB
using namespace std;
#endif

/*------------------------------------------------------------------------------
   xAutoSemaphore Constructor
------------------------------------------------------------------------------*/
xAutoSemaphore::xAutoSemaphore(const char    *Fname, 
                               const int      Line, 
                               const eError   Error,
                               const int      errNo,
                               const char    *errDescr)
               :xCept(Fname, Line, "xAutoSemaphore", Error)
{
   static char *eErrMsg[UNKNOWN_ERROR - ERROR_NO_ERROR + 1] =
   {
      /*ERROR_NO_ERROR     */    "No Error."
      /*ERROR_INIT_SEM     */    "Error initializing semaphore.",
      /*ERROR_LOCK         */    "Error locking semaphore.",
      /*ERROR_UNLOCK       */    "Error unlocking semaphore.",
      /*ERROR_RECREATING   */    "Error recreating semaphore.", 
      /*UNKNOWN_ERROR      */    "Unknown Error."
   };

   error   = (Error > UNKNOWN_ERROR || Error < ERROR_NO_ERROR) ? UNKNOWN_ERROR : Error;
   osErr << eErrMsg[Error - ERROR_NO_ERROR] << " - Sys ErrNo (" << errNo << ") Sys ErrDescr (" << errDescr << ")" << ends;
}

#if defined(M_GNU)
#define XSEM(v) throw xAutoSemaphore(__FILE__, __LINE__, xAutoSemaphore::v, errno, strerror(errno))
#else
#define XSEM(v) throw xAutoSemaphore(__FILE__, __LINE__, xAutoSemaphore::v, errno, sys_errlist[errno])
#endif

/*------------------------------------------------------------------------------
   AutoSemaphore Constructor
------------------------------------------------------------------------------*/
AutoSemaphore::AutoSemaphore(const char *name)
{
   _Name = 0;

   try
   {
      _Name = new char[strlen(name) + 1];

      strcpy(_Name, name);

#if defined(M_AIX) || defined(M_GNU)

      sem_unlink(_Name);

      if ((_Semaphore = sem_open(_Name, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
         XSEM(ERROR_INIT_SEM);

      if (sem_post(_Semaphore) != 0)
         XSEM(ERROR_INIT_SEM);

#endif
   }
   catch (...)
   {
      if (_Name)
         delete _Name;
   }
}


/*------------------------------------------------------------------------------
   xAutoSemaphore Destructor
------------------------------------------------------------------------------*/
AutoSemaphore::~AutoSemaphore()
{
   if (_Name)
      delete _Name;

#if defined(M_AIX) || defined(M_GNU)

   if (_Semaphore != SEM_FAILED)
      sem_close(_Semaphore);

#endif
}


/*------------------------------------------------------------------------------
   waits for until the semaphore has been unlocked
------------------------------------------------------------------------------*/
void AutoSemaphore::Lock()
{
#if defined(M_AIX) || defined(M_GNU)

   if (sem_wait(_Semaphore) == 0)
      return;

   // Attempt to recreate a valid semaphor
   if (errno == EINVAL)
   {
      unsigned index;

      for (index = 0; index < 3; index++)
      {
         if ((_Semaphore = sem_open(_Name, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
         {
            sleep(5);
            continue;
         }

         break;
      }

      if (index == 3)
         XSEM(ERROR_RECREATING);

      if (sem_wait(_Semaphore) != 0)
         XSEM(ERROR_LOCK);

      return;
   }
   if (errno == EINTR)
   {
      fprintf(stderr
            , "AutoSemaphore::Lock:\n"
              "  errno: %d\n"
            , errno
            );
      return;
   }

   XSEM(ERROR_LOCK);

#endif
}


/*------------------------------------------------------------------------------
   waits for until the semaphore has been unlocked
------------------------------------------------------------------------------*/
void AutoSemaphore::Unlock()
{
#if defined(M_AIX) || defined(M_GNU)

   if (sem_post(_Semaphore) == 0)
      return;

   XSEM(ERROR_UNLOCK);

#endif
}


/*------------------------------------------------------------------------------
   AutoSemaphoreLock Constructor
------------------------------------------------------------------------------*/
AutoSemaphoreLock::AutoSemaphoreLock(AutoSemaphore *semaphore)
{
   _Semaphore = semaphore;

   if (_Semaphore)
      _Semaphore->Lock();
}


/*------------------------------------------------------------------------------
   AutoSemaphoreLock Destructor
------------------------------------------------------------------------------*/
AutoSemaphoreLock::~AutoSemaphoreLock()
{
   if (_Semaphore)
      _Semaphore->Unlock();
}
