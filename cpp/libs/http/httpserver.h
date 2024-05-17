#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_
#include "machine.h"
#include "ini.h"
#include "logfile.h"
#include "tbuffer.h"
//#include "popgen.h"
//#include "dbportal.h"
#include "cbsocket.h"
#include "xstring.h"
#include "httpxcept.h"
#include "httprequest.h"
#include "json/json.h"
#include "splits.h"
#if defined(M_AIX) || defined(M_GNU)
#include <sys/resource.h>
#include <sys/wait.h>
#include "mutex.h"
#endif

class HTTPServer
{
public:
    static bool restful;
    static bool shutdown;
    TCBSockServer *sockServer;
    char* ConfigFile;
    char* LogFileName;
    eLevel logLevel;
    char* service;
    char* binFileName;
    char* connectString;
    int32 timeout;
    int32 noProcs;
    int32 restartCount;
    bool debug;
    bool logReceive;
    bool logTransmit;
    bool metrics;
    bool waitForChildren;
    HTTPServer(char* configFile, char* logFileName);
    ~HTTPServer();
    void OpenSocket();
    void LoadConfig();
    void WaitForCall(TLogFile &logFile);
    void (*ServiceClient)(const char* function, string input, string &output);
    int (*ServiceStart)(TLogFile &logFile);
    void Interpret(string& input, string& path);
#if defined(USE_OPENSSL)
    bool usessl;
    char* certfile;
    char* cafile;
    char* capath;
    char* keyfile;
    char* keypassword;
#endif    
#if defined(M_AIX) || defined(M_GNU)
    int semaphore;
    int ServiceStartPid(tLogFile &logFile, int pid)
    {
        char work[32];
        snprintf(work, 32, "(%d)", pid);
        logFile.CustomString = work;
        return ServiceStart(logFile);
    }
#endif
    int CreateHandlers(TLogFile &logFile);
};

extern "C" void ShutdownHandler(int arg);

#endif
