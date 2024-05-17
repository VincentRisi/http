#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "xcept.h"
#include "xstring.h"

#ifdef M_STDLIB
using namespace std;
#endif

XCept::XCept(const char* aFname,
	const int         aLine,
	const char* aName,
	const int         aError,
	const ESeverity   aSeverity)
	:line(aLine)
	, error(aError)
	, severity(aSeverity)
	, osErr("")
{
	osErr.clear();
	errorStr[0] = 0;
	__buffer[0] = 0;
	name[0] = 0;
	memset(errorStr, 0, sizeof(errorStr));
	strncpyz(fname, aFname, sizeof(fname) - 1);
	strncpyz(name, aName, sizeof(name) - 1);
}

XCept::XCept(const XCept& x)
	:line(x.line)
	, error(x.error)
	, severity(x.severity)
	, osErr(x.osErr.str())
{
	osErr.clear();
	__buffer[0] = 0;
	name[0] = 0;
	memset(errorStr, 0, sizeof(errorStr));;
	strcpy(fname, x.fname);
	strcpy(name, x.name);
}

void XCept::Display(std::ostream& os, const EMode mode)
{
	if (mode == modeVerbose)
	{
		if (severity == sevFatal)
			os << "Fatal Error ";
		else
			os << "Error ";
		os << name << " in " << fname << "(" << line << ") " << error << ": ";
	}
	os << ErrorStr() << std::endl << ends;
}


XBase::XBase(const char* file, const int line, const char* fmt, ...)
	:XCept(file, line, "XBase", -1)
{
	va_list ap;
	char    jotter[1024];
	va_start(ap, fmt);
	vsnprintf(jotter, sizeof(jotter) - 1, fmt, ap);
	va_end(ap);
	osErr << jotter << std::endl << ends;
}



