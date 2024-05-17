#include "mutex.h"

#if defined(M_GNU)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

void Mutex::acquire()
{
  struct timespec waitFor = {secs, nanosecs};
  static struct sembuf acquireOp[] =  // The possible semaphore
  { 0, 0, 0                           // Wait for semaphore == 0
  , 0, 1, SEM_UNDO                    // inc semaphore if crash undo
  };
  int rc = semtimedop(semaphore, acquireOp, 2, secs != 0 || nanosecs != 0 ? &waitFor : 0);
  if (rc == -1)
  {
    failed = true;
    if (errno == EINTR)
      return;
    throw XMUTEX(eMutexAcquireFailed);
  }
}

void Mutex::release()
{
  struct timespec waitFor = {secs, nanosecs};
  static struct sembuf releaseOp[] =
  { 0, -1, SEM_UNDO  // dec semaphore
  };
  int rc = semtimedop(semaphore, releaseOp, 1, secs != 0 || nanosecs != 0 ? &waitFor : 0);
  if (rc == -1)
  {
    failed = true;
    throw XMUTEX(eMutexReleaseFailed);
  }
}

int Mutex::makePrivate()
{
  int semaphore = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
  return semaphore;
}

int Mutex::makeNamed(const char* name)
{
  FILE* temp = fopen(name, "rb");
  if (temp == 0)
  {
    temp = fopen(name, "wb");
    if (temp == 0)
      throw XMUTEX(eMutexFtokFailed);
  }
  fclose(temp);
  key_t semKey = ftok(name, 1);
  if (semKey == -1)
    throw XMUTEX(eMutexSemKeyFailed);
  int semaphore = semget(semKey, 1, 0666 | IPC_CREAT);
  if (semaphore == -1)
    throw XMUTEX(eMutexSemGetFailed);
  return semaphore;
}

void Mutex::remove(int semaphore)
{
  semctl(semaphore, 0, IPC_RMID);
}

#else
void Mutex::acquire()
{
  DWORD waitFor = (DWORD) (secs * 1000 + nanosecs / 1000000);
  if (waitFor == 0) 
    waitFor = INFINITE;
  DWORD result = WaitForSingleObject(semaphore, waitFor);
  if (result != WAIT_OBJECT_0)
  {
    failed = true;
    throw XMUTEX(eMutexAcquireFailed);
  }
}

void Mutex::release()
{
  ReleaseMutex(semaphore);
}

static SECURITY_ATTRIBUTES sa;
static SECURITY_DESCRIPTOR sd;

static void _setup()
{
  if (sa.nLength == sizeof(sa))
    return;
  InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
  SetSecurityDescriptorDacl(&sd, true, NULL, false);
  sa.nLength = sizeof(sa);
  sa.lpSecurityDescriptor = &sd;
  sa.bInheritHandle = 1;
}

HANDLE Mutex::makePrivate()
{
  _setup();
  srand((unsigned int)time(0));
  char mutexName[128];
  snprintf(mutexName, sizeof(mutexName), "MUTEX_%04d", rand()%10000); 
  HANDLE semaphore = CreateMutex(&sa, FALSE, mutexName);
  return semaphore;
}

HANDLE Mutex::makeNamed(const char* name)
{
  _setup();
  HANDLE semaphore = CreateMutex(&sa, FALSE, name);
  return semaphore;
}

void Mutex::remove(HANDLE semaphore)
{
  CloseHandle(semaphore);
}

#endif