/**
 *  System : Generic
 *  $Source: /cvsroot/libs/libti/src/blackboard.h,v $
 *  $Author: vince $
 *  $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $
 *  $Revision: 92 $
 */

#ifndef _BLACKBOARD_H_
#define _BLACKBOARD_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(_BLACKBOARD_H_);
#include "ti.h"
#include "tbuffer.h"

enum EXBlackBoard
{ eBBNoError = 0
, eBBMappingfailed
, eBBMutexfailed
, eBBCircled
};

#define XBLACKBOARD(err) XBlackBoard(__FILE__, __LINE__, err)
class XBlackBoard : public xCept
{
public:
  static char *GetText(long err)
  {
    switch (err)
    {
    case eBBNoError:        return "No Error";
    case eBBMappingfailed:  return "Cannot Access Shared Memory Map Failure";
    case eBBMutexfailed:    return "Cannot Access Shared Memory Mutex Failure";
    case eBBCircled:        return "Circular Buffer Store Failed, size exceeded";
    default:                return "Unknown exception";
    }
  }
  XBlackBoard(pchar aFname, int aLine, long aError)
  : xCept(aFname, aLine, "XBlackBoard", aError)
  {
    osErr << aFname << "(" << aLine << ")";
    if (aError)
      osErr << "Error(" << aError << ") " << GetText(aError);
    osErr << std::ends;
  }
  XBlackBoard(const XBlackBoard& aX)
  : xCept(aX)
  {}
};

template <class TE>
struct TBlackBoard
{
  struct TStore
  {
    long Head;
    long Tail;
    long Size;
    long Count;
  };
  struct Wait
  {
    HANDLE mutex;
    Wait(HANDLE amutex)
    {
      mutex = amutex;
      DWORD Result = WaitForSingleObject(mutex, 5000);
      if (Result != WAIT_OBJECT_0)
        throw XBLACKBOARD(eBBMutexfailed);
    }
    ~Wait()
    {
      ReleaseMutex(mutex);
    }
  };
  SECURITY_ATTRIBUTES sa;
  SECURITY_DESCRIPTOR sd;
  HANDLE fmh;
  HANDLE mutex;
  long RollOver;
  pchar Area;
  TStore *Store;
  TBChar Name;
  TBlackBoard(pchar InName, long InCount=5000, long InRollOver=0)
  {
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, true, NULL, false);
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = &sd;
    sa.bInheritHandle = 1;
    RollOver = InRollOver;
    Name.set(InName);
    long FileSize = sizeof(Store)+sizeof(TE)*InCount;
    fmh = CreateFileMapping(HANDLE(0xFFFFFFFF), &sa, PAGE_READWRITE, 0, FileSize, Name);
    Area = (pchar)MapViewOfFile(fmh, FILE_MAP_ALL_ACCESS, 0, 0, FileSize);
    if (Area == 0)
      throw XBLACKBOARD(eBBMappingfailed);
    Store = (TStore*)Area;
    if (Store->Size != sizeof(TE) || Store->Count != InCount)
    {
      Store->Size = sizeof(TE);
      Store->Count = InCount;
      Store->Head = 0;
      Store->Tail = 0;
    }
    TBChar MutexName;
    MutexName.set(Name.data);
    MutexName.append("__Mutex");
    mutex = CreateMutex(&sa, FALSE, MutexName);
  }
  virtual ~TBlackBoard()
  {
    UnmapViewOfFile(Area);
    CloseHandle(fmh);
    CloseHandle(mutex);
  }
  void Insert(TE &Element)
  {
    Wait Here(mutex);
    TE *New = (TE*)(Area+sizeof(TStore)+Store->Size*Store->Tail);
    if (New == 0)
      throw XBLACKBOARD(eBBMappingfailed);
    *New = Element;
    long NewTail = Store->Tail+1;
    if (NewTail >= Store->Count)
      NewTail = 0;
    if (NewTail == Store->Head)
    {
      if (RollOver == 0)
        throw XBLACKBOARD(eBBCircled);
      else
      {
        long NewHead = Store->Head+1;
        if (NewHead >= Store->Count)
          NewHead = 0;
        Store->Head = NewHead;
      }
    }
    Store->Tail = NewTail;
  }
  bool Retrieve(TE &Element)
  {
    Wait Here(mutex);
    if (Store->Tail == Store->Head)
    {
      Store->Tail = Store->Head = 0;
      return false;
    }
    TE *New = (TE*)(Area+sizeof(TStore)+Store->Size*Store->Head);
    if (New == 0)
      throw XBLACKBOARD(eBBMappingfailed);
    Element = *New;
    Store->Head++;
    if (Store->Head >= Store->Count)
      Store->Head = 0;
    return true;
  }
  void Clear()
  {
    Wait Here(mutex);
    Store->Tail = Store->Head = 0;
  }
};

#endif

