#ifndef _OBSOCKET_H_
#define _OBSOCKET_H_

#include "machine.h"

#if defined(M_AIX)
extern "C" {
#  include <macros.h>
#  include <pwd.h>
#  include <sys/shm.h>
#  include <sys/sem.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/select.h>
#  include <sys/ioctl.h>
#  include <sys/tiuser.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <errno.h>
}
#elif defined(M_GNU)
#  include <pwd.h>
#  include <sys/shm.h>
#  include <sys/sem.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/select.h>
#  include <sys/ioctl.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <errno.h>
#elif defined(M_W32)
#  define  DESIRED_WINSOCK_VERSION 0x0101
#  define  MINIMUM_WINSOCK_VERSION 1
#  define  ioctl   ioctlsocket
typedef int socklen_t;
#endif

#define PROTOCOL "tcp"
#ifndef SOCKET_QSIZE
#  define SOCKET_QSIZE 1024
#endif

#if defined(USE_OPENSSL)
#  ifdef boolean
#    define keep_boolean boolean
#    undef boolean
#  endif
#  include <openssl/ssl.h>
#  include <openssl/err.h>
#  ifdef keep_boolean
#    define boolean keep_boolean
#    undef keep_boolean
#  endif
#endif

typedef struct
{
    int ListenPort;            // TCP socket Descriptor for Server
    int CommsPort;             // TCP socket Descriptor for active connection
    struct sockaddr_in Addr;   // Socket address
    socklen_t AddrLen;
    unsigned short Retries;     // For Wait
    int TimeOut;                // For Wait
    int ErrorNo;                // Error Code
#if defined(USE_OPENSSL)
    SSL_CTX *ctx;
    SSL     *ssl;
    char    SSL_Error[512];
    bool    hasCAFile;
    bool    hasCertFile;
#endif
} tSockCB;

extern "C"
{
    enum eError
    {
        errSockInit = 1
        , errSockHost
        , errSockService
        , errSockSocket
        , errSockBind
        , errSockListen
        , errSockConnect
        , errSockClose
        , errSockRead
        , errSockWrite
        , errSockVersion
        , errSockIOCtl
        , errSockLinger
        , errSockKeepAlive
        , errSockTimeout
        , errSockDebug
        , errSockGetOpt
        , errSockSetOpt
        , errSockParse
#if defined(USE_OPENSSL)
        , errSockSSLContext
#endif
    };

#if defined(USE_OPENSSL)
    // CAfile, CApath, KeyPassword can be supplied as 0 (NULL) for server
    DLLEXPORT_STDCALL(int) SSLServerInit(tSockCB *SockCB,
        const char *CertFile,
        const char *CAfile,
        const char *CApath,
        const char *KeyFile,
        const char *KeyPassword);

    // CertFile, CAfile, CApath, KeyFile, KeyPassword can be supplied as 0 (NULL) for client
    DLLEXPORT_STDCALL(int) SSLClientInit(tSockCB *SockCB,
        const char *CertFile,
        const char *CAfile,
        const char *CApath,
        const char *KeyFile,
        const char *KeyPassword);

#endif

    DLLEXPORT_STDCALL(int) SockDuplicate(tSockCB   **SockCB,
        int   aCommsPort,
        int   aRetries,
        int   aTimeOut);

    DLLEXPORT_STDCALL(int) SockDupServer(tSockCB    **SockCB,
        int    aCommsPort,
        int    aRetries,
        int    aTimeOut);

    DLLEXPORT_STDCALL(int) SockServerInit(tSockCB    **SockCB,
        const char  *aService);

    DLLEXPORT_STDCALL(int) SockServerOpen(tSockCB *SockCB);

    DLLEXPORT_STDCALL(int) SockClientInit(tSockCB    **SockCB,
        const char  *aHost,
        const char  *aService);

    DLLEXPORT_STDCALL(int) SockClientOpen(tSockCB *SockCB);

    DLLEXPORT_STDCALL(int) SockDone(tSockCB *SockCB);

    DLLEXPORT_STDCALL(int) SockClose(tSockCB *SockCB);

    DLLEXPORT_STDCALL(int) SockReadLength(tSockCB *SockCB,
        unsigned int *aSize);

    DLLEXPORT_STDCALL(int) SockRead(tSockCB       *SockCB,
        void          *aBuffer,
        unsigned int  aBufferLen);

    DLLEXPORT_STDCALL(int) SockStreamRead(tSockCB      *SockCB,
        void         *aBuffer,
        unsigned int aBufferLen,
        unsigned int *aSize);

    DLLEXPORT_STDCALL(int) SockWaitRead(tSockCB        *SockCB,
        unsigned int   aTimeOut,
        bool           *aResult);

    DLLEXPORT_STDCALL(int) SockWaitServer(tSockCB        *SockCB,
        unsigned int   aTimeOut,
        bool           *aResult);

    DLLEXPORT_STDCALL(int) SockWaitWrite(tSockCB       *SockCB,
        unsigned int  aTimeOut,
        bool          *aResult);

    DLLEXPORT_STDCALL(int) SockWriteLength(tSockCB       *SockCB,
        unsigned int  aLength);

    DLLEXPORT_STDCALL(int) SockWrite(tSockCB       *SockCB,
        void          *aBuffer,
        unsigned int  aBufferLen);

    DLLEXPORT_STDCALL(int) SockSetTimeOut(tSockCB     *SockCB,
        int         aTimeOut);

    DLLEXPORT_STDCALL(int) SockSetRetries(tSockCB         *SockCB,
        unsigned short  aRetries);

    DLLEXPORT_STDCALL(int) SockError(tSockCB *SockCB);

    DLLEXPORT_STDCALL(pchar) SockErrorText(int rc);

    DLLEXPORT_STDCALL(pchar) SockErrorMsg(tSockCB *SockCB);

    DLLEXPORT_STDCALL(int) SockErrorMsgNet(tSockCB *SockCB,
        char    *Buffer,
        int      BufferLen);

}

#endif



