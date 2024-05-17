#ifndef __CONFIG_H_
#define __CONFIG_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(__CONFIG_H_);

#include "bstring.h"
#include "xcept.h"
#include "xmlcore.h"

/*-----------------------------------
   Forward Declerations   
-----------------------------------*/
class XmlNode;


/*-----------------------------------
   Exception object
-----------------------------------*/
class xConfig : public xCept
{
   public:
      xConfig(const char *file, const int line, const char *fmt, ...);
			virtual ~xConfig() {}
      xConfig(const xConfig &x) : xCept(x) { }
};


/*-----------------------------------
   Config object   
-----------------------------------*/
class Config
{
   public :

      struct Flags
      {
         bool    cmdLineOnly;
         bool    hidden;
         bool    readOnly;
         bool    isPath;
      };

               Config(const char *overRideFile = 0);

              ~Config();

      void     LoadOverRideFile(const char *overRideFile);

      void     LoadDefaultFile();

      void     Destruct();

      bool     SetSection(const char *section, const bool throwError = true);

      unsigned SectionValues(bstringlink &values);

      unsigned CommonValues(bstringlink &values, bstringlink *exclusions = 0);

      int      Get(const char *value, int &out);

      long     Get(const char *value, long &out);

      unsigned Get(const char *value, unsigned &out);

      double   Get(const char *value, double &out);

      bstring &Get(const char *value, bstring &out);

      bool     Get(const char *value, bool &out);

      bool     Has(const char *value);

      char    *ConfigFileName() const;

      void     GetForCmdLineDisplay(const char *value, bstring &out, Flags &flags);

   protected:

      virtual char *EnvironmentVariable() const;

   private :

      unsigned ReadValues(bstringlink  &values, 
                         bstringlink   *exclusions,
                         XmlNode       *section);

      void    ValidateConfigFile();

      void    ReadValue(const char *value, bstring &out, const bool envOnly);

      void    ReadFlags(const char *value, const char *flagStr, Flags &flags) const;

      XmlNode  _Config;
      XmlNode *_Globals;
      XmlNode *_Common;
      XmlNode *_Specific;
      XmlNode *_Section;
      char    *_ConfigFile;
};

#endif

#if 0

This is an example of the config file

$() = environment variable
$[] = global variable

Flags = hidden|readonly|cmdlineonly|ispath

   hidden        - value will be hidden from the user when help is shown
   readonly      - value cannot be overriden from the command line
   cmdlineonly   - value can onl be set at the command line
   ispath        - will converts directory slashes to the correct os slash

<config>
   <globals>
      <database>nbpdev</database>
      <logpath>$(PROJDIR)/admin/log</logpath>
   </globals>

   <common>
      <connect desc = "Database Logon">npb/nbpqa@$[database]</connect>
   </common>

   <specific>

      <dealserver>
         <timeout   flags = "readonly" desc = "Server Timeout">300</timeout>
         <host      desc = "Host machine">thingy</host>
         <service   desc = "Port">dealserv<service>
      </dealserver>

      <DealExtract>
         <LogFile flags = "ispath">$[logpath]/dealextract.log</LogFile>
         <OutPath>dealext.out</OutPath>

         <RunDate desc = "Rundate of extract" flags = "cmdlineonly"/>
      </DealExtract>
      
   </specific>
</config>

#endif
