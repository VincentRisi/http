/**
 *  System : Generic min heap data structure
 *  This is a windows and unix implementation using shared memory
 *  The numbers are inserted independent of retrievals.
 *  The smallest number is returned on retrieval, this allows
 *  for negative numbers to used as flags. 
 *  If there are no more numbers in the heap we return the smallest
 *  negative number (-INT32_MAX).
 *  Use shared memory on other operating systems
 *  is most likely pretty simple. Other languages that can access 
 *  shared memory would most likely be easy to port to.
 *  Author: vince
 */
#ifndef _HEAPQUEUE_H_
#define _HEAPQUEUE_H_
#include "machine.h"
#include <limits.h>
#include "xcept.h"
#include "mutex.h"
#include "tbuffer.h"
#if defined (M_AIX) || defined (M_GNU)
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#endif

enum EXHeapQueue
{
    eHQNoError = 0
  , eHQFailed
  , eHQMutexfailed
};

#define XHEAPQUEUE(err) XHeapQueue(__FILE__, __LINE__, err)
class XHeapQueue : public xCept
{
public:
    static char *getText(long err)
    {
        switch (err)
        {
        case eHQNoError:        return "No Error";
        case eHQFailed:         return "Cannot Access Shared Memory Failure";
        case eHQMutexfailed:    return "Cannot Access Shared Memory Mutex Failure";
        default:                return "Unknown exception";
        }
    }
    XHeapQueue(pchar aFname, int aLine, long aError)
        : xCept(aFname, aLine, "XHeapQueue", aError)
    {
        osErr << aFname << "(" << aLine << ")";
        if (aError)
            osErr << "Error(" << aError << ") " << getText(aError);
        osErr << std::ends;
    }
    XHeapQueue(const XHeapQueue& aX)
        : xCept(aX)
    {}
};

template <typename T, size_t N>
struct HeapQueue
{
    struct Store
    {
        T heap[N];
        int heapSize;
        void init(T minimum)
        {
            if (heap[0].element != minimum.element)
            {
                heapSize = 0;
                heap[0] = minimum;
            }
        }
        /*Insert an element into the heap */
        void insert(T element)
        {
            heapSize++;
            heap[heapSize] = element; /*Insert in the last place*/
            /*Adjust its position*/
            int now = heapSize;
            while (heap[now / 2].element > element.element)
            {
                heap[now] = heap[now / 2];
                now /= 2;
            }
            heap[now] = element;
        }
        T retrieve()
        {
            /* heap[1] is the minimum element. So we remove heap[1]. Size of the heap is decreased.
             Now heap[1] has to be filled. We put the last element in its place and see if it fits.
             If it does not fit, take minimum element among both its children and replaces parent with it.
             Again See if the last element fits in that place.*/
            T minElement = heap[1], lastElement = heap[heapSize--];
            int child, now;
            //minElement = heap[1];
            //lastElement = heap[heapSize--];
            /* now refers to the index at which we are now */
            for (now = 1; now * 2 <= heapSize; now = child)
            {
                /* child is the index of the element which is minimum among both the children */
                /* Indexes of children are i*2 and i*2 + 1*/
                child = now * 2;
                /*child!=heapSize because heap[heapSize+1] does not exist, which means it has only one
                 child */
                if (child != heapSize && heap[child + 1].element < heap[child].element)
                    child++;
                /* To check if the last element fits ot not it suffices to check if the last element
                 is less than the minimum element among both the children*/
                if (lastElement.element > heap[child].element)
                    heap[now] = heap[child];
                else /* It fits there */
                    break;
            }
            heap[now] = lastElement;
            return minElement;
        }
    } *store;
#if defined (M_W32)
    SECURITY_ATTRIBUTES sa;
    SECURITY_DESCRIPTOR sd;
    HANDLE fmh;
    HANDLE mutex;
    TBChar name;
    char* area;
    HeapQueue(char* inName, T minimum, bool initialize=false)
    {
        InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&sd, true, NULL, false);
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = &sd;
        sa.bInheritHandle = 1;
        name.set(inName);
        int FileSize = sizeof(Store);
        fmh = CreateFileMapping(HANDLE(0xFFFFFFFF), &sa, PAGE_READWRITE, 0, FileSize, name.data);
        area = (char*)MapViewOfFile(fmh, FILE_MAP_ALL_ACCESS, 0, 0, FileSize);
        if (area == 0)
            throw XHEAPQUEUE(eHQFailed);
        store = (Store*)area;
        TBChar mutexName;
        mutexName.set(name.data);
        mutexName.append("__MUTEX");
        mutex = Mutex::makeNamed(mutexName);
        if (initialize == true)
        {
            Mutex here(mutex);
            store->init(minimum);
        }
    }
    virtual ~HeapQueue()
    {
        UnmapViewOfFile(area);
        CloseHandle(fmh);
        CloseHandle(mutex);
    }
#elif defined (M_AIX) || defined (M_GNU)
    int fmh;
    int mutex;
    TBChar name;
    char* area;
    HeapQueue(char* inName, T minimum, bool initialize=false)
    {
        name.set(inName);
        int FileSize = sizeof(Store);
        fmh = shm_open(name.data, O_RDWR|O_CREAT, 0660);
        int rc = ftruncate(fmh, (off_t) FileSize);
        area = (char*)mmap(0, FileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fmh, 0);
        if (area == 0)
            throw XHEAPQUEUE(eHQFailed);
        store = (Store*)area;
        TBChar mutexName;
        mutexName.set(name.data);
        mutexName.append("__MUTEX");
        mutex = Mutex::makeNamed(mutexName);
        if (initialize == true)
        {
            Mutex here(mutex);
            store->init(minimum);
        }
    }
    virtual ~HeapQueue()
    {
        int FileSize = sizeof(Store);
        munmap(area, FileSize);
        shm_unlink(name.data);
        close(fmh);
        close(mutex);
    }
#endif
    void insert(T element)
    {
        Mutex here(mutex);
        store->insert(element);
    }
    T retrieve()
    {
        Mutex here(mutex);
        if (store->heapSize == 0)
            return store->heap[0];
        return store->retrieve();
    }
};

struct Int32
{
    int32 element;
    Int32(int32 element)
    {
        this->element = element;
    }
};

struct Queue  // use of 8 48 8 for allows for bigsequence
{
    // This form make lower priority messages are retrieved first, then lower msgNo and then type. (kind of reversed)
    int64 type : 8;
    int64 msgNo : 48;
    int64 priority : 8;
};

inline int64 makeElement(int8 priority, int8 queueType, int64 msgNo)
{
    union
    {
        int64 element;
        Queue queue;
    };
    queue.priority = priority;
    queue.type = queueType;
    queue.msgNo = msgNo;
    return element;
}

inline Queue makeQueue(int64 value)
{
    union
    {
        int64 element;
        Queue queue;
    };
    element = value;
    return queue;
}

enum PriorityQEnums
{
    PriorityQTypeEmpty = 0,
    PriorityQTypeControl = 1,
    PriorityQTypeMessage,
    PriorityQTypeStream,
    PriorityQTypeResponse,
    PriorityQTypeReply,
    PriorityQTypeMessageTime,
    PriorityQTypeEvent,
    PriorityQControl = 1,
    PriorityQMsgHigh = 2,
    PriorityQMsgLow = 17,
    PriorityQResponse = 17,
    PriorityQReadPriority = -20,
    PriorityQReadAny = 0,
    PriorityQSrvRouteReq = 1,
    PriorityQSrvShutdownReq,
    PriorityQSrvShutdownDrvReq,
    PriorityQSrvReloadDrvReq,
    PriorityQSrvResendReq,
    PriorityQSrvShutdownDriversReq,
    PriorityQSrvRouteEventReq,
    PriorityQSrvRouteTimeReq,
    PriorityQSrvLShutdownReq = 99
};

struct PriorityQ
{
    union
    {
        int64 element;
        Queue queue;
    };
    PriorityQ(int64 element)
    {
        this->element = element;
    }
    PriorityQ(int8 priority, int8 type, int64 msgNo)
    {
        this->queue.priority = priority;
        this->queue.type = type;
        this->queue.msgNo = msgNo;
    }
};

#ifndef INT8_MAX
#define INT8_MAX  127
#endif
#ifndef INT16_MAX
#define INT16_MAX 32767
#endif
#ifndef INT32_MAX
#define INT32_MAX 2147483647
#endif
#ifndef INT64_MAX
#define INT64_MAX 9223372036854775807
#endif

const Int32 Int32min(-INT32_MAX);
const PriorityQ PriorityQmin(-INT64_MAX);
typedef HeapQueue<Int32, 100000> Int32MHeapQueue;
typedef HeapQueue<PriorityQ, 100000> PriorityQMHeapQueue;

#endif