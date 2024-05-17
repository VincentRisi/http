#ifndef __AQUEUE__H__
#define __AQUEUE__H__ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(__AQUEUE__H__);

#include "ti.h"
#include <sys/msg.h>
#include <errno.h>

#include "xcept.h"

static char* QueueErrors[] =
{ /* MSGXXX_OK*/      "OK ???"
, /* MSGCTL_EINVAL */ "The Command or MessageQueueID parameter is not valid."
, /* MSGCTL_EACCES */ "The Command parameter is equal to the IPC_STAT value, and the calling\n"
                      "process was denied read permission."
, /* MSGCTL_EPERM */  "The Command parameter is equal to the IPC_RMID value and the effective\n"
                      "user ID of the calling process does not have root user authority. Or, the\n"
                      "Command parameter is equal to the IPC_SET value, and the effective user ID of\n"
                      "the calling process is not equal to the value of the msg_perm.uid field or the\n"
                      "msg_perm.cuid field in the data structure associated with the MessageQueueID\n"
                      "parameter.\n\n"
                      "The Command parameter is equal to the IPC_SET value, an attempt was made\n"
                      "to increase the value of the msg_qbytes field, and the effective user ID of the\n"
                      "calling process does not have root user authority."
, /* MSGCTL_EFAULT */ "The Buffer parameter points outside of the process address space."
, /* MSGGET_EACCES */ "A message queue identifier exists for the Key parameter, but operation\n"
                      "permission as specified by the low-order 9 bits of the MessageFlag parameter is\n"
                      "not granted."
, /* MSGGET_ENOENT */ "A message queue identifier does not exist for the Key parameter and the\n"
                      "IPC_CREAT value is not set."
, /* MSGGET_ENOSPC */ "A message queue identifier is to be created, but the system-imposed limit\n"
                      "on the maximum number of allowed message queue identifiers system-wide would be\n"
                      "exceeded."
, /* MSGGET_EEXIST */ "A message queue identifier exists for the Key parameter, and both\n"
                      "IPC_CREAT and IPC_EXCL values are set."
, /* MSGRCV_EINVAL */ "The MessageQueueID parameter is not a valid message queue identifier.\n"
                      "The MessageSize parameter is less than 0."
, /* MSGRCV_EACCES */ "The calling process is denied permission for the specified operation."
, /* MSGRCV_E2BIG */  "The mtext field is greater than the MessageSize parameter, and the\n"
                      "MSG_NOERROR value is not set."
, /* MSGRCV_ENOMSG */ "The queue does not contain a message of the desired type and the\n"
                      "IPC_NOWAIT value is set."
, /* MSGRCV_EFAULT */ "The MessagePointer parameter points outside of the allocated address\n"
                      "space of the process."
, /* MSGRCV_EINTR */  "The msgrcv subroutine is interrupted by a signal."
, /* MSGRCV_EIDRM */  "The message queue identifier specified by the MessageQueueID parameter has\n"
                      "been removed from the system."
, /* MSGSND_EINVAL */ "The MessageQueueID parameter is not a valid message queue identifier.\n"
                      "The mtype field is less than 1.\n"
                      "or The MessageSize parameter is less than 0 or greater than the system-imposed limit.\n"
                      "or The upper 32-bits of the 64-bit mtype field for a 64-bit process is not 0."
, /* MSGSND_EACCES */ "The calling process is denied permission for the specified operation."
, /* MSGSND_EAGAIN */ "The message cannot be sent for one of the reasons stated previously, and\n"
                      "the MessageFlag parameter is set to the IPC_NOWAIT value."
, /* MSGSND_EFAULT */ "The MessagePointer parameter points outside of the address space of the process."
, /* MSGSND_EINTR */  "The msgsnd subroutine received a signal."
, /* MSGSND_EIDRM */  "The message queue identifier specified by the MessageQueueID parameter has\n"
                      "been removed from the system."
, /* MSGSND_ENOMEM */ "The message could not be sent because not enough storage space was available."
};

XCLASSDEF(QueueCept);
class xQueueCept : public xCept
{
public:
  enum eError
  { MSGXXX_OK
  , MSGCTL_EINVAL
  , MSGCTL_EACCES
  , MSGCTL_EPERM
  , MSGCTL_EFAULT
  , MSGGET_EACCES
  , MSGGET_ENOENT
  , MSGGET_ENOSPC
  , MSGGET_EEXIST
  , MSGRCV_EINVAL
  , MSGRCV_EACCES
  , MSGRCV_E2BIG
  , MSGRCV_ENOMSG
  , MSGRCV_EFAULT
  , MSGRCV_EINTR
  , MSGRCV_EIDRM
  , MSGSND_EINVAL
  , MSGSND_EACCES
  , MSGSND_EAGAIN
  , MSGSND_EFAULT
  , MSGSND_EINTR
  , MSGSND_EIDRM
  , MSGSND_ENOMEM
  };
  xQueueCept(char* aFname, int aLine, eError aError)
  : xCept(aFname, aLine, "xQueueCept", aError)
  {
    osErr << "TQueue: " << QueueErrors[aError] << std::endl;
    osErr << std::ends;
  }
  xQueueCept(const xQueueCept& aX)
  : xCept(aX)
  {
  }
};
#define XQUEUECEPT(ERR) throw xQueueCept(__FILE__, __LINE__, xQueueCept::ERR)

#if defined(M_GNU)
typedef int mtyp_t;
#endif

template <class TYPE>
class TQueue
{
public:
  int queueId;
  struct {mtyp_t type; TYPE data;} entry;
  TQueue(char *queueName)
  {
    char fileName[256];
    strcpy(fileName, "/tmp/");
    strncat(fileName, queueName, 200);
    strcat(fileName, ".MSGFTOK");
    if (access(fileName, 0) != 0)
    {
      FILE* x=fopen(fileName, "w");
      if (x)
        fclose(x);
    }
    entry.type = 1;
    int token = ftok(fileName, 11);
    queueId = msgget(token, IPC_CREAT|0666); // User Access Onlys
    if (queueId == -1)
    {
      switch (errno)
      {
      case EACCES: XQUEUECEPT(MSGGET_EACCES);
      case ENOENT: XQUEUECEPT(MSGGET_ENOENT);
      case ENOSPC: XQUEUECEPT(MSGGET_ENOSPC);
      case EEXIST: XQUEUECEPT(MSGGET_EEXIST);
      }
    }
  }
  bool receive(TYPE &data)
  {
    int rc = msgrcv(queueId, &entry, sizeof(TYPE), entry.type, IPC_NOWAIT);
    if (rc == -1)
    {
      switch (errno)
      {
      case EINVAL: XQUEUECEPT(MSGRCV_EINVAL);
      case EACCES: XQUEUECEPT(MSGRCV_EACCES);
      case E2BIG:  XQUEUECEPT(MSGRCV_E2BIG);
      case ENOMSG: return false;
      case EFAULT: XQUEUECEPT(MSGRCV_EFAULT);
      case EINTR:  XQUEUECEPT(MSGRCV_EINTR);
      case EIDRM:  XQUEUECEPT(MSGRCV_EIDRM);
      }
    }
    data = entry.data;
    return true;
  }
  void insert(TYPE &data)
  {
    entry.data = data;
    int rc = msgsnd(queueId, &entry, sizeof(TYPE), IPC_NOWAIT);
    if (rc == -1)
    {
      switch (errno)
      {
      case EINVAL: XQUEUECEPT(MSGSND_EINVAL);
      case EACCES: XQUEUECEPT(MSGSND_EACCES);
      case EAGAIN: XQUEUECEPT(MSGSND_EAGAIN);
      case EFAULT: XQUEUECEPT(MSGSND_EFAULT);
      case EINTR:  XQUEUECEPT(MSGSND_EINTR);
      case EIDRM:  XQUEUECEPT(MSGSND_EIDRM);
      case ENOMEM: XQUEUECEPT(MSGSND_ENOMEM);
      }
    }
  }
  void stats(struct msqid_ds &stats)
  {
    int rc = msgctl(queueId, IPC_STAT, &stats);
    if (rc == -1)
    {
      switch (errno)
      {
      case EINVAL: XQUEUECEPT(MSGCTL_EINVAL);
      case EACCES: XQUEUECEPT(MSGCTL_EACCES);
      case EPERM:  XQUEUECEPT(MSGCTL_EPERM);
      case EFAULT: XQUEUECEPT(MSGCTL_EFAULT);
      }
    }
  }
  void removeId()
  {
    int   rc = msgctl(queueId, IPC_RMID, 0);
    if (rc == -1)
    {
      switch (errno)
      {
      case EINVAL: XQUEUECEPT(MSGCTL_EINVAL);
      case EACCES: XQUEUECEPT(MSGCTL_EACCES);
      case EPERM:  XQUEUECEPT(MSGCTL_EPERM);
      case EFAULT: XQUEUECEPT(MSGCTL_EFAULT);
      }
    }
  }
};


#endif




