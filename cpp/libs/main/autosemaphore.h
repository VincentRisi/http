#ifndef __AUTOSEMAPHORE_H_
#define __AUTOSEMAPHORE_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(__AUTOSEMAPHORE_H_);

#include "machine.h"
#include "xcept.h"

#if defined(M_AIX) || defined(M_GNU)

   #include <semaphore.h>
   
#endif   

/*-----------------------------------
   Semaphore Exception Object
-----------------------------------*/
class xAutoSemaphore : public xCept
{
   public:

      enum eError
      { 
         ERROR_NO_ERROR, 
         ERROR_INIT_SEM,
         ERROR_LOCK,
         ERROR_UNLOCK,
         ERROR_RECREATING,
         UNKNOWN_ERROR
      };

      xAutoSemaphore(const char    *Fname, 
                     const int      Line, 
                     const eError   Error,
                     const int      errNo,
                     const char    *errDescr);

      xAutoSemaphore(const xAutoSemaphore &x) : xCept(x) {}
};


/*-----------------------------------
   AutoSemaphore
-----------------------------------*/
class AutoSemaphore
{
   public :

             AutoSemaphore(const char *name);
            ~AutoSemaphore();

      void   Lock();
      void   Unlock();

  private : 

      char  *_Name;

#if defined(M_AIX) || defined(M_GNU)

      sem_t *_Semaphore;

#endif
};


/*-----------------------------------
   AutoSemaphoreLock
-----------------------------------*/
class AutoSemaphoreLock
{
   public :

      AutoSemaphoreLock(AutoSemaphore *semaphore);
     ~AutoSemaphoreLock();

  private :

     AutoSemaphore *_Semaphore;

};

#endif
