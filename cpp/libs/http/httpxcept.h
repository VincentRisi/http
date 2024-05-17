#ifndef _HTTPXCEPT_H_
#define _HTTPXCEPT_H_
#include "machine.h"
#include "xcept.h"

class XHTTPError : public XCept
{
public:
    enum eError
    {
        ERR_HTTP_OK
        , ERR_HTTP_UNHANDLED
        , ERR_HTTP_NOT_POST
        , ERR_HTTP_NOT_JSON
        , ERR_HTTP_READWAIT_FAILED
        , ERR_HTTP_ZERODATA_READ
        , ERR_HTTP_POST_INPUT_INCORRECT
        , ERR_HTTP_ELLIPSIS_UNHANDLED
    };
    XHTTPError(const char* aFname, int aLine, eError aError, int aNo = 0, const char* aMsg = 0);
    XHTTPError(const XHTTPError& aX)
        : XCept(aX) {}
};
#define XHTTP_ERROR(err, msg) XHTTPError(__FILE__, __LINE__, XHTTPError::err, 0, msg)

#endif
