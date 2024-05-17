/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2007/03/01
 Name   : Config
 Descr  : A config reader class that works in conjuction with your 
          command line arguements
==============================================================================*/

#include <stdlib.h>
#include <stdarg.h>
#include <stdlib.h>

#include "config.h"
#include "xmlcore.h"
#include "xstring.h"
#include "basemacros.h"
#include "logfile.h"
#include "cmdlinearg.h"

#define CONFIG_FILE_ENV             "CONFIG_FILE"
#define DEFAULT_CONFIG_FILE         "config.xml"

#define XML_CONFIG                  "config"
#define XML_GLOBALS                 "globals"
#define XML_COMMON                  "common"
#define XML_SPECIFIC                "specific"
#define XML_DESCR_ATTR              "desc"
#define XML_FLAGS_ATTR              "flags"

#define FLAG_ATTR_CMD_LINE_ONLY     "cmdlineonly"
#define FLAG_ATTR_HIDDEN            "hidden"
#define FLAG_ATTR_READ_ONLY         "readonly"
#define FLAG_ATTR_IS_PATH           "ispath"


/*------------------------------------------------------------------------------
   xConfig Constructor
------------------------------------------------------------------------------*/
xConfig::xConfig(const char *file, const int line, const char *fmt, ...)
        :xCept(file, line, "xConfig", -1)
{
   va_list ap;
   char    jotter[1024];

   va_start(ap, fmt);

   vsnprintf(jotter, sizeof(jotter) - 1, fmt, ap);

   va_end(ap);

   osErr << jotter << std::endl << std::ends;
}


/*------------------------------------------------------------------------------
   Config Constructor
------------------------------------------------------------------------------*/
Config::Config(const char *overRideFile)
{    
   if (overRideFile)
   {
      LoadOverRideFile(overRideFile);
      return;
   }

   LoadDefaultFile();
}


/*------------------------------------------------------------------------------
   Config Destructor
------------------------------------------------------------------------------*/
Config::~Config()
{    
   Destruct();
}


/*------------------------------------------------------------------------------
   Clears the memory used
------------------------------------------------------------------------------*/
void Config::Destruct()
{
   _ConfigFile = 0;
   _Globals    = 0;
   _Common     = 0;
   _Specific   = 0;
   _Section    = 0;

   _Config.Destruct();
}


/*------------------------------------------------------------------------------
   Loads the override file specified
------------------------------------------------------------------------------*/
void Config::LoadOverRideFile(const char *overRideFile)
{
   Destruct();

   _ConfigFile = (char *)overRideFile;

   XmlNode::LoadFile(_Config, overRideFile);

   ValidateConfigFile();
}


/*------------------------------------------------------------------------------
   Loads the default file
------------------------------------------------------------------------------*/
void Config::LoadDefaultFile()
{
   Destruct();

   // Check Command Line Arg Overide
   if (CmdLineArg::isInitialized())
      _ConfigFile = CmdLineArg::ConfigFileOverride();

   // Check Environment variable
   if (_ConfigFile == 0 || *_ConfigFile == 0)
   {
      _ConfigFile = getenv(EnvironmentVariable());

      if (_ConfigFile == 0 || *_ConfigFile == 0)
      {
         SAVECUSTOMSTR(SLOG, "Config->");

         SLOG->lprintf(eLogWarning, "Default Config File not loaded - Environment Variable = '%s' has not been defined", EnvironmentVariable());
      }
   }

   // Set Default if not set yet
   if (_ConfigFile == 0 || *_ConfigFile == 0)
      _ConfigFile = DEFAULT_CONFIG_FILE;

   try
   {
      XmlNode::LoadFile(_Config, _ConfigFile);
   }
   catch (xCept &x)
   {
      throw xConfig(__FILE__, __LINE__, "Could not read config file '%s' because - '%s'", _ConfigFile, x.ErrorStr());
   }

   ValidateConfigFile();
}


/*------------------------------------------------------------------------------
   Ensures the xml file is sort of well formed
------------------------------------------------------------------------------*/
void Config::ValidateConfigFile()
{
   if (strcmp(_Config.Data->Name(), XML_CONFIG))
       throw xConfig(__FILE__, __LINE__, "Config file is invalid.  Root tag is not '%s'", XML_CONFIG);

   _Globals  = _Config.ExistsNode(XML_GLOBALS);
   _Common   = _Config.ExistsNode(XML_COMMON);
   _Specific = _Config.ExistsNode(XML_SPECIFIC);

   if (!_Common && !_Specific)
      throw xConfig(__FILE__, __LINE__, "Config file is invalid.  Expected to find a common or specific node");

   if (CmdLineArg::isInitialized())
      SetSection(CmdLineArg::ProgramName().c_str(), false);
}


/*------------------------------------------------------------------------------
   Sets the section
------------------------------------------------------------------------------*/
bool Config::SetSection(const char *section, const bool throwError)
{
   if (_STR_IS_NULL(section))
   {
      _Section = 0;
      return true;
   }

   if (_Specific == 0)
   {
      if (throwError)
         throw xConfig(__FILE__, __LINE__, "Section '%s' cannot be specified because no specific sections exist!", section);

      return false;
   }


   _Section = _Specific->ExistsNode(section);

   if (!_Section)
   {
      if (throwError)
         throw xConfig(__FILE__, __LINE__, "Section '%s' not found!", section);

      return false;
   }

   return true;
}


/*------------------------------------------------------------------------------
   Gets the value, ensure the value is an integer   
------------------------------------------------------------------------------*/
int Config::Get(const char *value, int &out)
{
   bstring val;

   Get(value, val);

   if (!val.isInt())
      throw xConfig(__FILE__, __LINE__, "Value '%s/%s' is not an integer!", value, val.c_str());

   out = val.toInt();

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value, ensure the value is a long
------------------------------------------------------------------------------*/
long Config::Get(const char *value, long &out)
{
   bstring val;

   Get(value, val);

   if (!val.isLong())
      throw xConfig(__FILE__, __LINE__, "Value '%s/%s' is not a long!", value, val.c_str());

   out = val.toLong();

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value, ensure the value is an unsigned
------------------------------------------------------------------------------*/
unsigned Config::Get(const char *value, unsigned &out)
{
   bstring val;

   Get(value, val);

   if (!val.isUnsigned())
      throw xConfig(__FILE__, __LINE__, "Value '%s/%s' is not an unsigned integer!", value, val.c_str());

   out = val.toUnsigned();

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value, ensure the value is a double
------------------------------------------------------------------------------*/
double Config::Get(const char *value, double &out)
{
   bstring val;

   Get(value, val);

   if (!val.isDouble())
      throw xConfig(__FILE__, __LINE__, "Value '%s/%s' is not a double!", value, val.c_str());

   out = val.toDouble();

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value
------------------------------------------------------------------------------*/
bstring &Config::Get(const char *value, bstring &out)
{
   bool     cmdLine     = false;
   XmlNode *sectionNode = 0;
   XmlNode *commonNode  = 0;
   XmlNode *node        = 0;
   char    *flagsStr    = 0;
   Flags    flags;

   memset(&flags, 0, sizeof(flags));

   if (CmdLineArg::isInitialized())
   {
      if (CmdLineArg::Has(value, &out))
         cmdLine = true;
   }
      
   if (_Section)
      node = sectionNode = _Section->ExistsNode(value);

   if (_Common)
   {
      commonNode = _Common->ExistsNode(value);

      if (!node)
         node = commonNode;
   }

   if (!node)
   {
      if (cmdLine)
      {
         ReadValue(value, out, false);
         return out;
      }

      throw xConfig(__FILE__, __LINE__, "Value '%s' is not found in the config file - Section '%s'", value, _Section ? _Section->Name() : "");
   }
   
   if (sectionNode             &&
       sectionNode->Attributes && 
       sectionNode->Attributes->Has(XML_FLAGS_ATTR, &flagsStr))
   {
      ReadFlags(value, flagsStr, flags);
   }
   else if (commonNode &&
            commonNode->Attributes && 
            commonNode->Attributes->Has(XML_FLAGS_ATTR, &flagsStr))
   {
      ReadFlags(value, flagsStr, flags);
   }

   if (cmdLine)
   {
      if (flags.readOnly)
         throw xConfig(__FILE__, __LINE__, "Value '%s' is read only any may not be overriden at the command line - Section '%s'", value, _Section ? _Section->Name() : "");
   }
   else
   {
      if (flags.cmdLineOnly)
         throw xConfig(__FILE__, __LINE__, "Value '%s' is a mandatory command line arguement - Section '%s'", value, _Section ? _Section->Name() : "");

      out = node->Value();
   }

   ReadValue(value, out, false);

   if (flags.isPath)
      out.replace(bstring::dirSep() == '\\' ? '/' : '\\', bstring::dirSep());

   return out;
}


/*------------------------------------------------------------------------------
   Gets the value, ensure the value is a bool
------------------------------------------------------------------------------*/
bool Config::Get(const char *value, bool &out)
{
   bstring val;

   Get(value, val);

   if (!val.isBool())
      throw xConfig(__FILE__, __LINE__, "Value '%s/%s' is not a boolean!", value, val.c_str());

   out = val.toBool();

   return out;
}


/*------------------------------------------------------------------------------
   returns true of false if the value exists
------------------------------------------------------------------------------*/
bool Config::Has(const char *value)
{
   if (_Section && _Section->Exists(value) != 0)
      return true;

   if (_Common && _Common->Exists(value) != 0)
      return true;

   return false;
}


/*------------------------------------------------------------------------------
   reads the value, checks for environment and global variables
------------------------------------------------------------------------------*/
void Config::ReadValue(const char  *value, 
                       bstring     &out, 
                       const bool   envOnly)
{
   out.strip();

   int found;
   int end;

   for (;;)
   {
      found = out.find("$(");

      if (found == -1)
         break;

      end = out.find(")", found + 1);

      if (end == -1)
         throw xConfig(__FILE__, __LINE__, "Environment variable is missing a closing bracket - Value '%s'", value);

      bstring  envname  = out.splice(found + 2, end);
      char    *envvalue = getenv(envname.c_str());

      if (_STR_IS_NULL(envvalue))
         throw xConfig(__FILE__, __LINE__, "Environment variable '%s' has not been set for value '%s'", envname.c_str(), value);

      out = out.splice(0, found) + envvalue + out.splice(end + 1);
   }

   if (envOnly)
      return;

   for (;;)
   {
      found = out.find("$[");

      if (found == -1)
         break;

      if (_Globals == 0)
         throw xConfig(__FILE__, __LINE__, "Globals variables have been used, but not defined - Value '%s'", value);

      end = out.find("]", found + 1);

      if (end == -1)
         throw xConfig(__FILE__, __LINE__, "Global variable is missing a closing bracket - Value '%s'", value);

      bstring   globvalue;
      bstring   globname  = out.splice(found + 2, end);
      XmlData  *data      = _Globals->Exists(globname.c_str());

      if (!data)
         throw xConfig(__FILE__, __LINE__, "Global variable '%s' has not been set for value '%s'", globname.c_str(), value);

      globvalue = data->Value();

      ReadValue(globname.c_str(), globvalue, true);

      out = out.splice(0, found) + globvalue + out.splice(end + 1);
   }
}


/*------------------------------------------------------------------------------
   returns the list of sectional config values
------------------------------------------------------------------------------*/
unsigned Config::SectionValues(bstringlink &values)
{
   return ReadValues(values, 0, _Section);
}


/*------------------------------------------------------------------------------
   returns the list of common values, excludes values from the exclusions list
------------------------------------------------------------------------------*/
unsigned Config::CommonValues(bstringlink  &values, 
                              bstringlink  *exclusions)
{
   return ReadValues(values, exclusions, _Common);
}


/*------------------------------------------------------------------------------
   ReadValues from a node
------------------------------------------------------------------------------*/
unsigned Config::ReadValues(bstringlink    &values, 
                            bstringlink    *exclusions,
                            XmlNode        *section)
{
   values.clear();

   if (!section)
      return 0;

   XmlNode  *node = section->Child;
   char     *desc;
   bstring  *excl;


   for (; node; node = node->Sibling)
   {
      desc  = 0;
      excl  = 0;

      if (exclusions)
         excl = exclusions->get(node->Data->Name());

      if (node->Attributes)
         node->Attributes->Has(XML_DESCR_ATTR, &desc);

      if (!excl)
      {
         values.set(node->Data->Name(), desc);
         continue;
      }

      if (desc && excl->isEmpty())
         exclusions->set(node->Data->Name(), desc);
   }

   return values.count();
}


//static char __empty[1];
/*------------------------------------------------------------------------------
   Returns the current config file name
------------------------------------------------------------------------------*/
char *Config::ConfigFileName() const
{
   return _ConfigFile ? _ConfigFile : __empty;
}


/*------------------------------------------------------------------------------
   Gets the value
------------------------------------------------------------------------------*/
void Config::GetForCmdLineDisplay(const char *value, bstring &out, Flags &flags)
{
   XmlNode *node     = 0;
   char    *flagsStr = 0;
   bool     cmdLine  = false;

   memset(&flags, 0, sizeof(flags));
      
   if (CmdLineArg::isInitialized())
   {
      if (CmdLineArg::Has(value, &out))
         cmdLine = true;
   }

   if (_Section)
   {
      node = _Section->ExistsNode(value);
   }

   if (!node)
   {
      node = _Common->ExistsNode(value);

      if (!node)
      {
         if (cmdLine)
         {
            ReadValue(value, out, false);
            flags.cmdLineOnly = true;
            return;
         }

         throw xConfig(__FILE__, __LINE__, "Value '%s' is not found in the config file.", value);
      }
   }

   out = node->Value();

   if (node->Attributes)
      node->Attributes->Has(XML_FLAGS_ATTR, &flagsStr);

   if (flagsStr && *flagsStr)
      ReadFlags(value, flagsStr, flags);
}


      
/*------------------------------------------------------------------------------
   returns true if this flags contain the cmd line only attribute  
------------------------------------------------------------------------------*/
void Config::ReadFlags(const char *value, 
                       const char *flagStr, 
                       Flags      &flags) const
{
   if (flagStr == 0 || *flagStr == 0)
      return;

   bstrings  farr;
   bstring   f = flagStr;
   unsigned  index;

   memset(&flags, 0, sizeof(flags));

   f.split('|', farr);

   for (index = 0; index < farr.Count(); index++)
   {
      if (farr[index].lower() == FLAG_ATTR_CMD_LINE_ONLY)
      {
         flags.cmdLineOnly = true;
         continue;
      }

      if (farr[index].lower() == FLAG_ATTR_HIDDEN)
      {
         flags.hidden = true;
         continue;
      }

      if (farr[index].lower() == FLAG_ATTR_READ_ONLY)
      {
         flags.readOnly = true;
         continue;
      }

      if (farr[index].lower() == FLAG_ATTR_IS_PATH)
      {
         flags.isPath = true;
         continue;
      }

      throw xConfig(__FILE__, __LINE__, "Invalid flag property '%s' found for value '%s', section '%s'", farr[index].c_str(), value, _Section ? _Section->Name() : "");
   }
}


/*------------------------------------------------------------------------------
   returns the environment variable that stores a default config filename
------------------------------------------------------------------------------*/
char *Config::EnvironmentVariable() const
{
   return CONFIG_FILE_ENV;
}
