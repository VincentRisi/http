#ifndef __CMD_LINE_ARG_H_
#define __CMD_LINE_ARG_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(__CMD_LINE_ARG_H_);

#include "xcept.h"
#include "bstring.h"

/*-----------------------------------
   xCmdLineArg
-----------------------------------*/
class xCmdLineArg : public xCept
{
   public:

      xCmdLineArg(const char *file, const int line, const char *fmt, ...);

      xCmdLineArg(const xCmdLineArg &x) : xCept(x) { }
};


/*-----------------------------------
   CmdLineArg 
-----------------------------------*/
class CmdLineArg
{
   public :

      static bool     isInitialized();

      static bool     Initialize(const int argc, char **argv);

      static int      Get(const char *value, int &out);

      static unsigned Get(const char *value, unsigned &out);

      static double   Get(const char *value, double &out);

      static bstring  Get(const char *value, bstring &out);

      static bool     Get(const char *value, bool &out);

      static bool     Has(const char *value, bstring *out = 0);

      static bstring  ProgramName();

      static char    *ConfigFileOverride();

      static bool     ShowHelp(const bool mustShow = false);

   private :

      CmdLineArg();

      static    int    _argc;
      static    char **_argv;
};

#endif
