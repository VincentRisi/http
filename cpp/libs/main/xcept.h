#ifndef _XCEPT_H_
#define _XCEPT_H_

#include "machine.h"
using namespace std;
#include <string.h>
#include <sstream>

class XCept
{

public:
  enum ESeverity
  {
    sevFatal,
    sevNonFatal
  };

  enum EMode
  {
    modeVerbose,
    modeUser
  };
  XCept(const XCept &x);
  virtual ~XCept() {}
  const char *FileName() const { return fname; }
  int Line() const { return line; }
  const char *Name() const { return name; }
  int Error() const { return error; }
  ESeverity Severity() const { return severity; }
  //pchar ErrorStr()     {return errorStr;}
  char *ErrorStr()
  {
    memset(__buffer, 0, sizeof(__buffer));
    strncpy(__buffer, osErr.str().c_str(), sizeof(__buffer) - 1);
    int len = (int)strlen(__buffer);
    strncat(__buffer, errorStr, sizeof(__buffer) - len - 1);
    __buffer[sizeof(__buffer) - 1] = 0;
    return __buffer;
  }
  void Display(std::ostream &os, const EMode mode = modeVerbose);

protected:
  enum
  {
    ErrorStrLen = 4096,
    NameLen = 20,
    FileLen = 256
  };

  char name[NameLen];  // name of exception class
  int error;           // error no. within exception class
  char fname[FileLen]; // file name
  int line;            // line number
  ESeverity severity;
  char errorStr[ErrorStrLen]; // error string - contains all relevant info

  XCept(const char *aFname,
        const int aLine,
        const char *aName,
        const int aError,
        const ESeverity aSeverity = sevNonFatal);
  std::stringstream osErr;

private:
  char __buffer[ErrorStrLen];
};

class XBase : public XCept
{
public:
  XBase(const char *file, const int line, const char *fmt, ...);
  XBase(const XBase &x)
      : XCept(x) {}
};

///*-----------------------------------
//   xDBException
//-----------------------------------*/
//class xDBException : public XCept
//{
//public:
//  xDBException(const char *file, const int line, const char *typeOf, int errorNo, char *error)
//      : XCept(file, line, typeOf, errorNo)
//  {
//    osErr << "Database Error: " << std::endl
//          << error << std::endl
//          << std::ends;
//  }
//  xDBException(const xDBException &copy)
//      : XCept(copy)
//  {
//  }
//};

//class xIdlException : public XCept
//{
//public:
//  xIdlException(const char *file, const int line, const char *typeOf, int errorNo, const char *error)
//      : XCept(file, line, typeOf, errorNo)
//  {
//    osErr << "RPC IDL Error: " << std::endl
//          << error << std::endl
//          << std::ends;
//  }
//  xIdlException(const xIdlException &copy)
//      : XCept(copy)
//  {
//  }
//};

///*-----------------------------------
//   Marcos for backwards compatibility
//-----------------------------------*/
//#define DisplayVerbose(x, os)                              \
//  do                                                       \
//  {                                                        \
//    os << "Caught exception at "                           \
//       << __FILE__ << "(" << __LINE__ << ")" << std::endl; \
//    x.Display(os);                                         \
//  } while (0)
//
//#define UndefineDisplayVerbose(x, os)                      \
//  do                                                       \
//  {                                                        \
//    os << "Caught Undefine exception at "                  \
//       << __FILE__ << "(" << __LINE__ << ")" << std::endl; \
//  } while (0)
//
//#define DisplayUser(x, os)          \
//  do                                \
//  {                                 \
//    x.Display(os, XCept::modeUser); \
//  } while (0)
//
//// logging to syslog on Unix
//
//#ifdef M_AIX
//
//#define LogVerbose(x, severity, logstream)                   \
//  do                                                         \
//  {                                                          \
//    logstream << severity << "Caught exception at "          \
//              << __FILE__ << "(" << __LINE__ << ")" << '\n'; \
//    x.Display(logstream);                                    \
//  } while (0)
//
//#define LogUser(x, severity, logstream)    \
//  do                                       \
//  {                                        \
//    logstream << severity;                 \
//    x.Display(logstream, XCept::modeUser); \
//  } while (0)
//
//#else
//
//#define LogVerbose(x, severity, logstream)
//#define LogUser(x, severity, logstream)
//
//#endif

#endif
