/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2007/02/01
 Name   : cmdlinearg
 Descr  : A command line arg object, works together with the config object
==============================================================================*/

#include <stdlib.h>
#include <stdarg.h>

#include "cmdlinearg.h"
#include "config.h"
#include "xstring.h"

#ifdef M_STDLIB
using namespace std;
#endif

/*------------------------------------------------------------------------------
   Static method, Gets the current value of the speicified value
------------------------------------------------------------------------------*/
static char *CurrentValue(Config *cfg, const char *value, bstring &out)
{
   if (!cfg->Has(value))
      return "";

   Config::Flags flags;

   cfg->GetForCmdLineDisplay(value, out, flags);

   if (flags.hidden)
      out = "???";

   if (flags.readOnly)
      out += "~";

   if (flags.cmdLineOnly)
      out += "*";

   if (flags.isPath)
      out += " (r)";

   return (char *) out;
}

/*------------------------------------------------------------------------------
   xCmdLineArg Constructor
------------------------------------------------------------------------------*/
xCmdLineArg::xCmdLineArg(const char *file, const int line, const char *fmt, ...)
            :xCept(file, line, "xCmdLineArg", -1)
{
   va_list ap;
   char    jotter[1024];

   va_start(ap, fmt);

   vsnprintf(jotter, sizeof(jotter) - 1, fmt, ap);

   va_end(ap);

   osErr << jotter << std::endl << ends;
}


/*------------------------------------------------------------------------------
   SingletonLogFile Memembers
------------------------------------------------------------------------------*/
int    CmdLineArg::_argc = 0;
char **CmdLineArg::_argv = 0;


/*------------------------------------------------------------------------------
   CmdLineArg Constructor
------------------------------------------------------------------------------*/
CmdLineArg::CmdLineArg()
{
}


/*------------------------------------------------------------------------------
   Returns false if singleton has not been initialized
------------------------------------------------------------------------------*/
bool CmdLineArg::isInitialized()
{
   if (_argc == 0 || _argv == 0)
      return false;

   return true;
}


/*------------------------------------------------------------------------------
   Initialize, this be called in main, right up front before anything else
------------------------------------------------------------------------------*/
bool CmdLineArg::Initialize(const int argc, char **argv)
{
   _argc = argc;
   _argv = argv;

   return ShowHelp();
}


/*------------------------------------------------------------------------------
   ProgramName
------------------------------------------------------------------------------*/
bstring CmdLineArg::ProgramName()
{
   if (!isInitialized())
      throw xCmdLineArg(__FILE__, __LINE__, "CmdLineArg::ProgramName not initialized");

   bstring name(_argv[0]);

   if (name.rfind(bstring::dirSep()) != -1)
       name = name.splice(name.rfind(bstring::dirSep()) + 1, 0);

   if (name.find('.') != -1)
      name = name.splice(0, name.find('.'));

   return name.lower();
}


/*------------------------------------------------------------------------------
   ConfigFileOverride
------------------------------------------------------------------------------*/
char *CmdLineArg::ConfigFileOverride()
{
   if (!isInitialized())
      throw xCmdLineArg(__FILE__, __LINE__, "CmdLineArg::ConfigFileOverride not initialized");

   int index;

   for (index = 1; index < _argc; index++)
      if (!strcmp(_argv[index], "-c") && 
          index + 1 < _argc          && 
          _argv[index + 1][0] != '0')
         return _argv[index + 1];

   return 0;
}


/*------------------------------------------------------------------------------
   Gets the value, ensures values is an int
------------------------------------------------------------------------------*/
int CmdLineArg::Get(const char *value, int &out)
{
   bstring val;

   Get(value, val);

   if (!val.isInt())
      throw xCmdLineArg(__FILE__, __LINE__, "Value '%s/%s' is not an integer!", value, val.c_str());

   out = val.toInt();

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value, ensures values is an unsigned
------------------------------------------------------------------------------*/
unsigned CmdLineArg::Get(const char *value, unsigned &out)
{
   bstring val;

   Get(value, val);

   if (!val.isUnsigned())
      throw xCmdLineArg(__FILE__, __LINE__, "Value '%s/%s' is not an unsigned integer!", value, val.c_str());

   out = val.toUnsigned();

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value, ensures values is a double
------------------------------------------------------------------------------*/
double CmdLineArg::Get(const char *value, double &out)
{
   bstring val;

   Get(value, val);

   if (!val.isDouble())
      throw xCmdLineArg(__FILE__, __LINE__, "Value '%s/%s' is not a double!", value, val.c_str());

   out = val.toDouble();

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value, ensure the value is a bool
------------------------------------------------------------------------------*/
bool CmdLineArg::Get(const char *value, bool &out)
{
   bstring val;

   Get(value, val);

   if (!val.isBool())
      throw xCmdLineArg(__FILE__, __LINE__, "Value '%s/%s' is not a boolean!", value, val.c_str());

   out = val.toBool();

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value
------------------------------------------------------------------------------*/
bstring CmdLineArg::Get(const char *value, bstring &out)
{
   if (!isInitialized())
      throw xCmdLineArg(__FILE__, __LINE__, "CmdLineArg::Get not initialized");

   int index;

   for (index = 1; index < _argc; index++)
      if (_argv[index][0] == '-' && strcmp(&_argv[index][1], value) == 0)
      {
         out = "";

         if (index + 1 >= _argc)
            return out;

         if (_argv[index + 1][0] == '-')
            return out;

         out = _argv[index + 1];
            return out;
      }
      
   throw xCmdLineArg(__FILE__, __LINE__, "Command Line Arg '-%s' has not been set", value);
}


/*------------------------------------------------------------------------------
   returns true if the specified command line exists
------------------------------------------------------------------------------*/
bool CmdLineArg::Has(const char *value, bstring *out)
{
   if (!isInitialized())
      throw xCmdLineArg(__FILE__, __LINE__, "CmdLineArg::Has not initialized");

   int index;

   for (index = 1; index < _argc; index++)
      if (_argv[index][0] == '-' && !strcmp(&_argv[index][1], value))
      {
         if (out)
         {
            if (index + 1 >= _argc)
               return false;

            if (_argv[index + 1][0] == '-')
               return false;

            if (out)
               (*out) = _argv[index + 1];
         }

         return true;
      }

   return false;
}


/*------------------------------------------------------------------------------
   Shows the help, returns false if user wants to see the help
------------------------------------------------------------------------------*/
bool CmdLineArg::ShowHelp(const bool mustShow)
{
   if (!isInitialized())
      throw xCmdLineArg(__FILE__, __LINE__, "CmdLineArg::ShowHelp not initialized");

   if (!Has("?"))
   {
      if (!mustShow)
         return true;
   }
   
   Config            cfg;
   bstring           progname = ProgramName();
   bstring           currValue;
   bstringlink       sectlinks;
   bstringlink       commlinks;
   unsigned          index;

   printf("Program Usage '%s'\n", progname.c_str());
   printf("--------------------------------------------------------------------------------------\n");
   printf(" %-18.18s : %-30.30s : %-18.18s\n", "Command", "Description", "Current Value");
   printf("--------------------------------------------------------------------------------------\n");
   printf("-%-18.18s : %-30.30s : %s\n", "?", "This Help", "");
   printf("-%-18.18s : %-30.30s : %s\n", "c", "Config file override", cfg.ConfigFileName());

   cfg.SetSection(progname.c_str(), false);

   cfg.SectionValues(sectlinks);
   cfg.CommonValues(commlinks, &sectlinks);

   if (sectlinks.count() > 0)
   {
      for (index = 0; index < sectlinks.count(); index++)
         printf("-%-18.18s : %-30.30s : %s\n", sectlinks.getLink(index)->c_str(),
                                               sectlinks.get(index)->c_str(),
                                               CurrentValue(&cfg, sectlinks.getLink(index)->c_str(), currValue));
   }

   if (commlinks.count() > 0)
   {
      printf("\n Common Overrides \n\n");

      for (index = 0; index < commlinks.count(); index++)
         printf("-%-18.18s : %-30.30s : %s\n", commlinks.getLink(index)->c_str(),
                                               commlinks.get(index)->c_str(),
                                               CurrentValue(&cfg, commlinks.getLink(index)->c_str(), currValue));
   }

   printf("\n * = cmd line only, ~ = read only, ??? = hidden, (r) = is a path\n\n");
   
   return false;
}
