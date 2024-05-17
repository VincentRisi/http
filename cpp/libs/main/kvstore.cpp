#include "kvstore.h"
#include "addlist.h"
#include "mutex.h"
#include "xcept.h"
#include "tbuffer.h"

enum EXKVStore
{ eKVNoError = 0
, eKVMappingfailed
, eKVMutexfailed
};

#define KVSTORE_XCEPT(err) KVStoreXCept(__FILE__, __LINE__, err)
class KVStoreXCept : public xCept
{
public:
  static char *GetText(long err)
  {
    switch (err)
    {
    case eKVNoError:        return "No Error";
    case eKVMappingfailed:  return "Cannot Access Shared Memory Map Failure";
    case eKVMutexfailed:    return "Cannot Access Shared Memory Mutex Failure";
    default:                return "Unknown exception";
    }
  }
  KVStoreXCept(pchar aFname, int aLine, long aError)
  : xCept(aFname, aLine, "KVStoreXCept", aError)
  {
    osErr << aFname << "(" << aLine << ")";
    if (aError)
      osErr << "Error(" << aError << ") " << GetText(aError);
    osErr << ends;
  }
  KVStoreXCept(const KVStoreXCept& aX)
  : xCept(aX)
  {}
};

struct KVStore
{
  const char* key;
  int valueSize;
  const void* data;
};

static TAddList<KVStore, int>* Store;

static int _compare(KVStore* a, KVStore* b)
{
  return strcmp(a->key, b->key);
}

#if defined(M_AIX) || defined(M_GNU)
static int semaphore;
#else
static HANDLE semaphore;
#endif

static void _init()
{
  if (semaphore != 0)
    return;
  Store = new TAddList<KVStore, int>();
  Store->compare = _compare;
  semaphore = Mutex::makePrivate();
}

void kvstore_put(const char* key, int valueSize=0, const void* data=0)
{
  _init();
  Mutex wait(semaphore);
  KVStore entry;
  entry.key = key;
  int element = Store->search(&entry);
  if (element == -1)
  {
    if (valueSize > 0)
    {
      entry.valueSize = valueSize;
      entry.data = malloc(valueSize);
      memcpy((char*)entry.data, (char*)data, valueSize);
      Store->add(entry);
      Store->sort();
    }
    return;
  }
  KVStore* list = Store->getList();
  free((char*)list[element].data);
  if (valueSize > 0)
  {
    list[element].valueSize = valueSize;
    list[element].data = malloc(valueSize);
    memcpy((char*)list[element].data, (char*)data, valueSize);
  }
  else
    Store->remove(element);
  return;
}

const void* kvstore_get(const char* key, int &valueSize)
{
  _init();
  Mutex wait(semaphore);
  KVStore entry;
  entry.key = key;
  int element = Store->search(&entry);
  if (element == -1)
    return 0;
  KVStore* list = Store->getList();
  valueSize = list[element].valueSize;
  return list[element].data;
}

void kvstore_free(const char* key)
{
  _init();
  Mutex wait(semaphore);
  KVStore entry;
  entry.key = key;
  int element = Store->search(&entry);
  if (element != -1)
  {
    KVStore* list = Store->getList();
    free((char*)list[element].data);
    Store->remove(element);
  }
}
