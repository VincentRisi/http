#ifndef _CBSOCKET_H_
#define _CBSOCKET_H_

#include "machine.h"
#include "xcept.h"
#include "obsocket.h"


/*-----------------------------------
   xSockErr
-----------------------------------*/
class XSockErr : public XCept
{
public:
   enum eError
   {
      errSockOK
      , errSockInit
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

   XSockErr(const char   *aFname, 
            const int     aLine, 
            const eError  aError, 
            const tSockCB *fSockCB,
            const int     aNo, 
            const char   *aMsg,
            const char   *aMsg2 = 0, 
            const char   *aMsg3 = 0);

   XSockErr(const XSockErr& aX): XCept(aX) {}
};

struct TCBSockBuffer
{
   unsigned int  BytesToRead;
   unsigned int  BytesRead;
   unsigned int  BytesToWrite;
   unsigned int  BytesWritten;
   char          *p;
};

/*-----------------------------------
   TCBSockClient
-----------------------------------*/
class TCBSockClient
{
protected:
   TCBSockClient() {}
   static int ParseName(char* name, const char sep, char* names[]);

public:
   tSockCB *fSockCB;
   // This form of the constructor will parse the Name and pull out the
   // Host and Service. This is present to cater for the older interfaces
   // like cliserv and cs. the form of this name is :socket:<host>:<service>
   TCBSockClient(const char *aName);
   // This is the preferred form of constructor.
   // aHost is the name of the server providing the service, defined either in
   // the hosts file (/etc/hosts or \winnt\system32\drivers\etc\hosts) or
   // resolved by a domain name server.
   // aService is a service name defined in the services file // (/etc/services
   // or \winnt\system32\drivers\etc\services). This must resolve to the same
   // number on both the local and remote machines.
   TCBSockClient(const char *aHost, const char *aService);
   // Construct Client from an existing socket number
   TCBSockClient(const int iSocket, const int Retries = 5, const int TimeOut = 10000);
#if defined(USE_OPENSSL)
   // Construct SSL Client. CertFile and KeyFile can be passed in as null(0). If used the other 3 can be used or passed in as null(0).
   TCBSockClient(const char *aHost, const char  *aService, const char *CertFile, const char *CAfile, const char *CApath, const char *KeyFile, const char *KeyPassword);
#endif
   // Standard destructor
   virtual ~TCBSockClient();

   // Open the socket for the client.
   void   Open();

   // Close the socket.
   void   Close();

   //Gets the client's IP Address
   //Changed to return a char *, as tString has been removed
   char *ClientIPAddress() const;

   // ReadLength is used to read an unsigned long as data and convert it
   // to host from network byte order. This is the counterpart for
   // WriteLength;
   unsigned int ReadLength();

   // Read data into aBuffer for the exact length aLength, else raise
   // an exception. This is used directly after ReadLength;
   void  Read(void *aBuffer, const unsigned int aLength);

   // Read data into aBuffer for the length aToRead throwing away
   // any excess and return the actual no read in aRead.
   // This is arguably a bad form and should be avoided.
   void   Read(void *aBuffer, const unsigned int aToRead, unsigned int &aRead);

   // Old form read that uses a tBuffer construct for the lengths and data.
   // Will resolve into new form.
   void   Read(TCBSockBuffer *aBuffer);

   // Read data into aBuffer up to the length aLength
   // returns the length read.
   unsigned int ReadStream(void *aBuffer, const unsigned int aLength);

   // WriteLength writes an unsigned int as data after converting it
   // to network byte order.
   void   WriteLength(const unsigned int aLength);

   // Write data for exactly the length of aLength. This is normally
   // used after WriteLength, but can also be used to write stream
   // data.
   void   Write(void *aBuffer, const unsigned int aLength);

   // Old form write that uses a tBuffer construct for the lengths and
   // data. Will resolve into the new form.
   void   Write(TCBSockBuffer *aBuffer);

   // ================================================================
   // A fix for the existing problem of multiple write resolved to one
   // ================================================================

   //void   Write(TCBSockBuffer **aBuffer, ulong aCount);

   // WaitRead will not read data, but will wait until there is data
   // to read or timeout. Returns true if there is data or returns
   // false if it times out.
   bool   WaitRead(const unsigned int aTimeout);

   // WaitWrite will not write data, but will wait until the
   // counterpart is ready to read or timeout. Returns true if the
   // counterpart is ready or returns false if it times out.
   bool   WaitWrite(const unsigned int aTimeout);

   // This only really makes sense from a client point of view
   // Will do an old style write followed by a read.
   void   Transact(TCBSockBuffer *bufOut, TCBSockBuffer *bufIn); // WR

   // This only really makes sense from a client point of view
   // Will do an open, old style write followed by a read, then a close.
   void   Call(TCBSockBuffer *bufOut, TCBSockBuffer *bufIn);     // OWRC

   // Sets the timeout parameter is millisecs
   void   SetTimeout(const unsigned int aTimeout) { fSockCB->TimeOut = aTimeout; }

   // gets the timeout value
   unsigned int GetTimeout() const {return fSockCB->TimeOut;}

   // Sets the number of times to retry
   void   SetRetries(const unsigned int aRetries) { fSockCB->Retries = (unsigned short)aRetries;}

   // Gets the number of retries
   unsigned short GetRetries() const {return fSockCB->Retries;}

   // Query the open status
   bool   IsOpen() const { return fSockCB->CommsPort == -1 ? false : true; }

   // Return the IP address
   sockaddr_in* SockAddr() const {return &fSockCB->Addr;}

   // Return the socket
   int Socket() const {return fSockCB->CommsPort;}

   unsigned short PortNo() const {return (unsigned short)htons(fSockCB->Addr.sin_port);}

   int CommsPort() const {return fSockCB->CommsPort;}
};

class AutoSemaphore;

class TCBSockServer : public TCBSockClient
{
private:
   // This form of constructor is illegal now.
   TCBSockServer() 
   { 
      AutoSem = 0;
   }

public:
    // This form of constructor expects a service defined in a services file.
    // see above. It will also honour the old form name :socket:<host>:<service>
    // and extract the service.
    TCBSockServer(const char *aService);
    // Construct a socket class from an existing socket number
    TCBSockServer(const int iSocket, const int Retries = 5, const int TimeOut = 10000);
    // Construct a socket class from an existing socket number
    TCBSockServer(const TCBSockServer &Sock);
#if defined(USE_OPENSSL)
    // Construct an OpenSSL server - required CertFile and KeyFile - the others may be left as null(0) or used.
    TCBSockServer(const char  *aService, const char *CertFile, const char *CAfile, const char *CApath, const char *KeyFile, const char *KeyPassword);
#endif
    // Open the socket for the server.
    void Open();
    // Waits until a client is ready to connect or times out.
    // Does not do the Open. Returns true if a client is
    // present else false if times out.
    bool WaitConnect(const unsigned int aTimeout);

    int ListenPort() const { return fSockCB->ListenPort; }

    AutoSemaphore  *AutoSem;

};

#endif
