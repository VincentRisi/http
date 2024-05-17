/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2006/11/01
 Name   : XmlCore
 Descr  : Xml classes to parse to simple xml

 Change Date   : 2008-08-15
 Author		   : Rohan van den Berg
 Descr		   : Fixed XML Core. Was skipping characters when finding <! and <? tags.

 Change Date   : 2008-08-18
 Author		   : Rohan van den Berg
 Descr		   : Fixed XML Core - again!!! Overloaded = operator is wrong wrong wrong. 
                 And screwing the instance one the right side of the operator. 
				 Inserted fix code to maintain the parent pointers that was getting screwed. 
				 Removal of overload to follow shortly!

 Change Date   : 2008-08-19
 Author		   : Rohan van den Berg
 Descr		   : Fixed XML Core - again * 3... Added new function to remove a node/tag that will 
                 work with duplicate tags as well. Is simple at the mo but need to improve it to 
				 work for parent tags as well. At the mo it's only used for lowest level tags ie 
				 tag without a child.

 Change Date   : 2008-08-21
 Author        : Rohan van den Berg
 Description   : Added a static method that enables you to strip the XML of empty tags.
                 This was quite a mission since xml core needed fixing before I could achieve this.
				 You can pass 

 Change Date   : 2009-01-19
 Author		   : Rohan van den Berg
 Descr		   : Enhancement... wish I knew why we are parsing XML all by our selves...
                 Are we better than the rest of the people that have done this before???
				 A bit deep down the rabbit hole at this stage and xmlcore part of tooo much.
				 Including code to provide functionality that is suppose to be standard.
				 Namely xi:include... Not sure I'm doing it right in terms of the W3C standard.
				 If fact I'm more sure it's a but wrong ... but will have to do for now.

 Change Date   : 2009-08-04
 Author		     : Dieter Rosch
 Descr		     : Bug fix... in the terrible operator = function, Nic "I don't need SmartPointers, my code doesn't leak" (verbatim quote)
                 forgets to delete the destination target's members. The result is a massive memory leak everytime you re-use an XMLNode
							   instance. 

 Change Date   : 2009-11-11
 Author		     : Dieter Rosch
 Descr		     : Memory leak fix - delete the includeXml in the xiInclude section.

==============================================================================*/


#include "xmlcore.h"
#include "basemacros.h"
#include "machine.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
//#include <xstring.h>

/*-----------------------------------
   xml build defines
-----------------------------------*/
#define XC_IDENTIFIER_MAX_LEN   65536
#define NODE_DELIMETER          '>'
#define ATTR_DELIMETER          '<'
#define STR_COMPARE             strcmp
#define STR_N_COMPARE           strncmp

#define XC_OPEN                 1
#define XC_CLOSE                2
#define XC_FINISHED             3

using namespace std;


/*------------------------------------------------------------------------------
   static methods
------------------------------------------------------------------------------*/
static void xmlCore_xiInclude(char *&pos, XmlNode *&node, char *jotter);

static void xmlCore_PrintNode(FILE          *dest, 
                              const XmlNode *node, 
                              const XmlNode *topNode,
                              const unsigned tab)
{
   if (!node)
      return;

   unsigned i;

   for (i = 0; i < tab; i++)
      fprintf(dest, "\t");

   fprintf(dest, "<%s", node->Data->Name());

   if (node->Attributes)
   {
      unsigned  a;
      XmlData  *data;

      for (a = 0; a < node->Attributes->Count(); a++)
      {
         data = node->Attributes->Data(a);

         fprintf(dest, " %s = \"%s\"", data->Name(), data->Value());
      }
   }

   if (!node->Data->Value()[0] && !node->Child)
   {
      fprintf(dest, "/>\n");
      if (topNode != node)
         xmlCore_PrintNode(dest, node->Sibling, topNode, tab);
      return;
   }

   fprintf(dest, ">");

   if (node->Data->Value()[0])
   {
      fprintf(dest, "%s", node->Data->Value());

      if (!node->Child)
      {
         fprintf(dest, "</%s>\n", node->Data->Name());
         if (topNode != node)
            xmlCore_PrintNode(dest, node->Sibling, topNode, tab);
         return;
      }
   }

   if (node->Child)
   {
      fprintf(dest, "\n");

      xmlCore_PrintNode(dest, node->Child, topNode, tab + 1);
   }

   for (i = 0; i < tab; i++)
      fprintf(dest, "\t");

   fprintf(dest, "</%s>\n", node->Data->Name());

   if (topNode != node)
      xmlCore_PrintNode(dest, node->Sibling, topNode, tab);
}


static void xmlCore_PrintRawNode(FILE          *dest, 
                                 const XmlNode *node,
                                 const XmlNode *topNode,
                                 const unsigned tab)
{
   if (!node)
      return;

   unsigned i;

   for (i = 0; i < tab; i++)
      fprintf(dest, "\t");

   fprintf(dest, "<%s", node->Data->Name());

   if (node->Attributes)
   {
      unsigned  a;
      XmlData  *data;
      for (a = 0; a < node->Attributes->Count(); a++)
      {
         data = node->Attributes->Data(a);
         int len = data->ValueLen()+17;
         char     *jotter = new char[len];
         fprintf(dest, " %s = \"%s\"", data->Name(), data->GetRaw( jotter, len ));
         delete [] jotter;
      }
   }

   if (!node->Data->Value()[0] && !node->Child)
   {
      fprintf(dest, "/>\n");
      if (topNode != node)
         xmlCore_PrintRawNode(dest, node->Sibling, topNode, tab);
      return;
   }

   fprintf(dest, ">");

   if (node->Data->Value()[0])
   {
      {
         int len = node->Data->ValueLen()+17;
         char *jotter = new char[len];

         fprintf(dest, "%s", node->Data->GetRaw( jotter, len ));
         delete []jotter;
      }

      if (!node->Child)
      {
         fprintf(dest, "</%s>\n", node->Data->Name());
         if (topNode != node)
            xmlCore_PrintRawNode(dest, node->Sibling, topNode, tab);
         return;
      }
   }

   if (node->Child)
   {
      fprintf(dest, "\n");

      xmlCore_PrintRawNode(dest, node->Child, topNode, tab + 1);
   }

   for (i = 0; i < tab; i++)
      fprintf(dest, "\t");

   fprintf(dest, "</%s>\n", node->Data->Name());

   if (topNode != node)
      xmlCore_PrintRawNode(dest, node->Sibling, topNode, tab);
}

#define ADD_STR(str, strSize) if (strSize + outUsed < outLen) \
{ \
   outUsed += strSize; \
   strcat(out, str); \
} \
else \
   throw xXmlCore(__FILE__, __LINE__, "xmlCore_ToString - String buffer of size '%u' is to small for the xml", outLen)



static void xmlCore_ToString(char           *out, 
                             const unsigned  outLen,
                             unsigned       &outUsed,
                             const XmlNode  *node,
                             const XmlNode  *topNode)
{
   if (!node)
      return;

   ADD_STR("<",                1);
   ADD_STR(node->Data->Name(), strlen(node->Data->Name()));

   if (node->Attributes)
   {
      unsigned  a;
      XmlData  *data;
      
      for (a = 0; a < node->Attributes->Count(); a++)
      {
         data = node->Attributes->Data(a);
         int len = data->ValueLen()+17;
         char *jotter = new char[len];
         data->GetRaw(jotter, len);

         ADD_STR(" ",          1);
         ADD_STR(data->Name(), strlen(data->Name()));
         ADD_STR(" = \"",      4);
         ADD_STR(jotter,       strlen(jotter));
         ADD_STR("\"",         1);
         delete [] jotter;
      }
   }

   if (!node->Data->Value()[0] && !node->Child)
   {
      ADD_STR("/>", 2);

      if (topNode != node)
         xmlCore_ToString(out, outLen, outUsed, node->Sibling, topNode);
      return;
   }

   ADD_STR(">", 1);

   if (node->Data->Value()[0])
   {
      {
         int len = node->Data->ValueLen()+17;
         char  *jotter = new char[len];
         node->Data->GetRaw(jotter, len);
         ADD_STR(jotter, strlen(jotter));

         delete [] jotter;
      }

      if (!node->Child)
      {
         ADD_STR("</",               2);
         ADD_STR(node->Data->Name(), strlen(node->Data->Name()));
         ADD_STR(">",                1);

         if (topNode != node)
            xmlCore_ToString(out, outLen, outUsed, node->Sibling, topNode);
         return;
      }
   }

   if (node->Child)
      xmlCore_ToString(out, outLen, outUsed, node->Child, topNode);

   ADD_STR("</",               2);
   ADD_STR(node->Data->Name(), strlen(node->Data->Name()));
   ADD_STR(">",                1);

   if (topNode != node)
      xmlCore_ToString(out, outLen, outUsed, node->Sibling, topNode);
}

/*
Changed, as top is runnig out of stack space quite fast :-(
*/
static void xmlCore_c_str( char           *out, 
                           const unsigned outLen,
                           unsigned       &outUsed,
                           XmlNode  *inNode,
                           const XmlNode  *topNode)
{
   if (!inNode)
      return;

   for ( XmlNode *node = inNode; node; node = node->Sibling )
   {
     ADD_STR("<",                1);
     ADD_STR(node->Data->Name(), strlen(node->Data->Name()));
  
     if (node->Attributes)
     {
        unsigned  a;
        XmlData  *data;
        
        for (a = 0; a < node->Attributes->Count(); a++)
        {
           data = node->Attributes->Data(a);
           int len = data->ValueLen()+17;
           char *jotter = new char[len];
           data->GetRaw(jotter, len);
  
           ADD_STR(" ",          1);
           ADD_STR(data->Name(), strlen(data->Name()));
           ADD_STR(" = \"",      4);
           ADD_STR(jotter,       strlen(jotter));
           ADD_STR("\"",         1);
           delete [] jotter;
        }
     }
  
     if (!node->Data->Value()[0] && !node->Child)
     {
        ADD_STR("/>", 2);
  
        //if (topNode != node)
        //   xmlCore_c_str(out, outLen, outUsed, node->Sibling, topNode);
        continue;
     }
  
     ADD_STR(">", 1);
  
     if (node->Data->Value()[0])
     {
        if ( node->Data->GetCData() )
        {
          ADD_STR( "\n<![CDATA[", strlen("\n<![CDATA[") );
          ADD_STR(node->Data->Value(), node->Data->ValueLen());
          ADD_STR( " ]]>\n", strlen(" ]]>\n") );
        }
        else
        {
           int len = node->Data->ValueLen()+17;
           char  *jotter = new char[len];
  
           node->Data->GetRaw(jotter, len);
           
           ADD_STR(jotter, strlen(jotter));
  
           delete [] jotter;
        }
  
        if (!node->Child)
        {
           ADD_STR("</",               2);
           ADD_STR(node->Data->Name(), strlen(node->Data->Name()));
           ADD_STR(">",                1);
  
           //if (topNode != node)
           //   xmlCore_ToString(out, outLen, outUsed, node->Sibling, topNode);
           continue;
        }
     }
  
     if (node->Child)
        xmlCore_c_str(out, outLen, outUsed, node->Child, topNode);
  
     ADD_STR("</",               2);
     ADD_STR(node->Data->Name(), strlen(node->Data->Name()));
     ADD_STR(">",                1);
  
     //if (topNode != node)
     //   xmlCore_ToString(out, outLen, outUsed, node->Sibling, topNode);
   }
}


#undef ADD_STR


static inline bool xmlCore_WhiteSpace(const char ch)
{
   if (ch == ' '  ||
       ch == '\n' ||
       ch == '\r' ||
       ch == '\t' ||
       ch == '\f' ||
       ch == '\v')
      return true;

   return false;
}


static inline bool xmlCore_XmlValueChar(const char ch)
{
   if (ch == '<' ||
       ch == '>')
      return true;

   return false;
}


static inline bool xmlCore_XmlAttributeChar(const char ch)
{
   if (ch == '<' ||
       ch == '/' ||
       ch == '=' ||
       ch == '>')
      return true;

   return false;
}


static inline void xmlCore_SaveValue(char *&pos, char *&dest)
{
   if (STR_N_COMPARE(pos, "&gt;", 4) == 0)
   {
      *dest = '>';
      pos += 3;
      return;
   }

   if (STR_N_COMPARE(pos, "&lt;", 4) == 0)
   {
      *dest = '<';
      pos += 3;
      return;
   }

   if (STR_N_COMPARE(pos, "&amp;", 5) == 0)
   {
      *dest = '&';
      pos += 4;
      return;
   }

   if (STR_N_COMPARE(pos, "&apos;", 6) == 0)
   {
      *dest = '\'';
      pos += 5;
      return;
   }

   if (STR_N_COMPARE(pos, "&quot;", 6) == 0)
   {
      *dest = '"';
      pos += 5;
      return;
   }

   *dest = *pos;
}


static int xmlCore_SetValue(char *&pos, char *&dest)
{
   if (*pos == '>')
   {
      memcpy(dest, "&gt;", 4);
      dest += 3;
      return 4;
   }

   if (*pos == '<')
   {
      memcpy(dest, "&lt;", 4);
      dest += 3;
      return 4;
   }

   if (*pos == '&')
   {
      memcpy(dest, "&amp;", 5);
      dest += 4;
      return 5;
   }

   if (*pos == '\'')
   {
      memcpy(dest, "&apos;", 6);
      dest += 5;
      return 6;
   }

   if (*pos == '"')
   {
      memcpy(dest, "&quot;", 6);
      dest += 5;
      return 6;
   }

   *dest = *pos;
   return 1;
}


static void xmlCore_MakeValueRaw(const char *value, char *dest, unsigned dest_size)
{
   int   len    = strlen(value);
   int   newlen = 0;
   char *ptr    = (char *) value;
   char *jptr   = dest;

   // Robert ?? Why
   //dest_size -= 16; // give space for expansion due to raw characters

   if (len >= (int) dest_size)
      throw xXmlCore(__FILE__, __LINE__, "XmlData::Set - Value larger that dest buffer");

   for (; *ptr && newlen < (int) dest_size; ptr++, jptr++)
      newlen += xmlCore_SetValue(ptr, jptr);

   *jptr = 0;

   //if (newlen >= XC_IDENTIFIER_MAX_LEN)
   //   throw xXmlCore(__FILE__, __LINE__, "XmlData::Set - Value larger that dest buffer - xml tranformation");
}


/*
Note you have to delete
*/
//static void xmlCore_ReadCData(char *&pos, char *&value, unsigned &size)
static char* xmlCore_ReadCData(char *&pos, unsigned &size, int &len )
{
   if (strncmp(pos + 2, "[CDATA[", 7) != 0)
      return 0;

   pos += 9;
   char *start = pos;

   for (; *pos; pos++, size++)
   {
     if (strncmp(pos, "]]>", 3) == 0)
     {
        pos += 3;
        break;
     }
   }
   len = (int)(pos-start)-3;
   if ( len < 0 )
     len = 0;

   char *retvalue = new char[len + 1];
   memcpy( retvalue, start, len );
   retvalue[len] = 0;
   return retvalue;

   /*
   for (; *pos; pos++, value++, size++)
   {
      if (size >= XC_IDENTIFIER_MAX_LEN)
         throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadCData - Max value size reached!");

      if (strncmp(pos, "]]>", 3) == 0)
      {
         pos += 3;
         return;
      }

      *value = *pos;
   }
   */

}


static void xmlCore_ReadValue(char *&pos, XmlNode *&node)
{
   for (; *pos; pos++)
   {
      if (xmlCore_WhiteSpace(*pos))
      {
         continue;
      }
      else if (*pos == '<' && 
               *(pos + 1) == '!' && 
               *(pos + 2) == '[' &&
               *(pos + 3) == 'C' &&
               *(pos + 4) == 'D' &&
               *(pos + 5) == 'A' &&
               *(pos + 6) == 'T' &&
               *(pos + 7) == 'A' &&
               *(pos + 8) == '[')
      {
         break;
      }
      else if (xmlCore_XmlValueChar(*pos))
      {
         return;
      }
      else
      {
         break;
      }
   }

   char     *jotter = new char[XC_IDENTIFIER_MAX_LEN];
   char     *value = jotter;
   unsigned  size  = 1;

   for (; *pos; pos++, value++, size++)
   {
      if (size >= XC_IDENTIFIER_MAX_LEN)
         throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadValue - Max value size reached!");

      if (pos[0] == '<' && pos[1] == '!')
      {
         int len = 0;
         char *res = xmlCore_ReadCData(pos,size,len);
         if ( res )
         {
            node->Data->Set(res);
            node->Data->SetCData(true);
        
            // incase there is any white spaces.
            while ( !xmlCore_XmlValueChar(*pos) )
               pos++;

            delete [] jotter;
            delete [] res;

            return;
         }
      }

      if (xmlCore_XmlValueChar(*pos))
      {
         *value = 0;
         node->Data->Set(jotter);
         delete [] jotter;
         return;
      }

      if (*pos == '&')
         xmlCore_SaveValue(pos, value);
      else
         *value = *pos;
   }

   delete [] jotter;

   throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadValue - Could not read xml value - end of file unexpected");
}


static void xmlCore_ReadAttribute(char *&pos, char *dest)
{
   for (; *pos; pos++)
   {
      if (xmlCore_WhiteSpace(*pos))
         continue;

      break;
   }

   char *begin = pos;

   for (; *pos; pos++)
      if (xmlCore_WhiteSpace(*pos) || xmlCore_XmlAttributeChar(*pos))
      {
         memcpy(dest, begin, pos - begin);
         dest[pos - begin] = 0;
         return;
      }

   throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadAttribute - Could not read xml attribute - end of file unexpected");
}


static void xmlCore_ReadAttributeValue(char *&pos, char *dest)
{
   char id;

   for (; *pos; pos++)
      if (*pos == '"')
      {
         id = '"';
         break;
      }
      else if (*pos == '\'')
      {
         id = '\'';
         break;
      }
         

   if (!pos)
      throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadAttributeValue - Could not read xml attribute value - end of file unexpected");

   pos++;

   char     *value = dest;
   unsigned  size  = 1;

   for (; *pos; pos++, value++, size++)
   {
      if (size >= XC_IDENTIFIER_MAX_LEN)
         throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadAttributeValue - Max value size reached!");

      if (*pos == id)
      {
         *value = 0;
         return;
      }

      if (*pos == '&')
         xmlCore_SaveValue(pos, value);
      else
         *value = *pos;
   }

   throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadAttributeValue - Could not read value - end of file unexpected");
}


static void xmlCore_FindEquals(char *&pos)
{
   for (; *pos; pos++)
      if (*pos == '=')
      {
         pos++;
         return;
      }

   throw xXmlCore(__FILE__, __LINE__, "xmlCore_FindEquals - Could not find equals - end of file unexpected");
}


static short xmlCore_ReadAttributes(char *&pos, XmlNode *&node)
{
   char identifier[XC_IDENTIFIER_MAX_LEN];
   char value[XC_IDENTIFIER_MAX_LEN];

   for (; *pos; pos++)
   {
      if (*pos == '/' && *(pos + 1) == '>')
      {
         pos += 2;
         return XC_CLOSE;
      }

      if (*pos == '>')
      {
         pos++;
         return XC_FINISHED;
      }

      if (xmlCore_WhiteSpace(*pos))
         continue;

      xmlCore_ReadAttribute(pos, identifier);

      xmlCore_FindEquals(pos);

      xmlCore_ReadAttributeValue(pos, value);

      node->AddAttribute(new XmlData(identifier, value));
   }

   throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadAttributes - Could not read attributes - end of file unexpected");
}


static void xmlCore_ReadTag(char *&pos, char *dest, const bool open_tag)
{
   char *orig = pos;
   char *end;

   for (; *pos; pos++)
      if (open_tag)
      {
         if (xmlCore_WhiteSpace(*pos) || *pos == '>' || *pos == '/')
         {
            end = pos;
            break;
         }
      }
      else
      {
         if (*pos == '>')
         {
            end = pos;
            break;
         }
      }

   if (end <= orig)
      throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadTag - Invalid xml tag - end pos <= start pos");

   if (end - orig >= XC_IDENTIFIER_MAX_LEN)
      throw xXmlCore(__FILE__, __LINE__, "xmlCore_ReadTag - Identifier exceeds max len of %d", XC_IDENTIFIER_MAX_LEN);

   memcpy(dest, orig, end - orig);

   dest[end - orig] = 0;
}

static char *righttrim(char *s)
{
   int len = strlen(s);

   while (len && (s[len - 1] == ' ' || s[len - 1] == '\t' || s[len - 1] == '\n' ))
      s[--len] = 0;
   return s;
}

static short xmlCore_OpenClose(char *&pos, XmlNode *&node)
{
   char jotter[XC_IDENTIFIER_MAX_LEN];

   for (; *pos; pos++)
      if (*pos == '<')
      {
         pos++;

         // skip comments
         if (*pos       == '!' && 
             *(pos + 1) == '-' &&
             *(pos + 2) == '-')
         {
            for (pos += 3; *pos; pos++)
               if (*pos       == '-' &&
                   *(pos + 1) == '-' &&
                   *(pos + 2) == '>')
               {
                  pos += 2;
                  break;
               }

            continue;
         }

         // skip infomation
         if (*pos == '?')
         {
            for (pos += 1; *pos; pos++)
               if (*pos       == '?' &&
                   *(pos + 1) == '>')
               {
                  pos += 1;
                  break;
               }

            continue;
         }

         // check close tag
         if (*pos == '/')
         {
            pos++;
            xmlCore_ReadTag(pos, jotter, false);
            righttrim(jotter);
            if (STR_COMPARE(jotter, node->Data->Name()))   
                throw xXmlCore(__FILE__, __LINE__, "xmlCore_OpenClose - Close tag '%s' does not match open tag '%s'", jotter, node->Data->Name());

            return XC_CLOSE;
         }

         //Check open tag
         xmlCore_ReadTag(pos, jotter, true);

         if (!node)
            node = new XmlNode(new XmlData(jotter)); 
         else
            node = node->AppendChild(new XmlData(jotter)); 

         return XC_OPEN;
      }

   return XC_FINISHED;
}

static void xmlCore_xiInclude(char *&pos, XmlNode *&node, char *jotter)
{
	char* attributeValue;
	XmlNode* includeXml;

	includeXml = new XmlNode();

	if ( !node->Attributes->Has("href", &attributeValue) )
		throw xXmlCore(__FILE__, __LINE__, "xmlCore_xiInclude - xi:include tag '%s' does not have a href property", jotter);

	//Have to cater for the use of an environment variable in the href path
	//Code taken from config class and replace bstring with std::string
	string envVarStr = attributeValue; 
    string::size_type found;
    string::size_type end;
    for (;;)
    {
	  found = envVarStr.find("$(");

	  if (found == string::npos)
		 break;

	  end = envVarStr.find(")", found+1);

	  if (end == string::npos)
		 throw xXmlCore(__FILE__, __LINE__, "Environment variable is missing a closing bracket - Value '%s'", attributeValue);

	  string  envname  = envVarStr.substr(found+2, end-(found+2));
	  char    *envvalue = getenv(envname.c_str());

	  if (_STR_IS_NULL(envvalue))
		 throw xXmlCore(__FILE__, __LINE__, "Environment variable '%s' has not been set for value '%s'", envname.c_str(), attributeValue);

	  envVarStr = envVarStr.substr(0, found) + envvalue + envVarStr.substr(end + 1);
	  attributeValue = (char*)envVarStr.c_str();
    }
	
	XmlNode::LoadFile(*includeXml, attributeValue);

	//Setup new links
	node->Sibling = includeXml->Child;
	for(XmlNode* workNode = node->Sibling;workNode;workNode=workNode->Sibling)
	{
		workNode->Parent = node->Parent;
	}

	//Don't delete sibling and parent
	includeXml->Sibling    = 0;
	includeXml->Parent     = 0;
	includeXml->Child      = 0;

	//Dieter - delete this to avoid memory leak
	delete includeXml;

}
/*------------------------------------------------------------------------------
   xXmlCore Constructor
------------------------------------------------------------------------------*/
xXmlCore::xXmlCore(const char *file, const int line, const char *fmt, ...)
         :xCept(file, line, "xXmlCore", -1)
{
   va_list ap;
   char    jotter[1024];
	 memset(jotter, 0, sizeof(jotter));

   va_start(ap, fmt);

   vsnprintf(jotter, sizeof(jotter) - 1, fmt, ap);

   va_end(ap);

   osErr << jotter << std::endl;// << ends;
}


/*------------------------------------------------------------------------------
   XmlData Constructors
------------------------------------------------------------------------------*/
XmlData::XmlData(const char *name)
{
   _Value = 0;
   _Name  = 0;
   _CData = false;

   SetName(name);
}


XmlData::XmlData(const char *name, const char *value)
{
   _Value = 0;
   _Name  = 0;
   _CData = false;

   SetName(name);
   Set(value);
}

XmlData::XmlData(const char *name, const char *value, const bool CData)
{
   _Value = 0;
   _Name  = 0;
   _CData = CData;

   SetName(name);
   Set(value);
}


XmlData::XmlData(const char *name, const int value)
{
   _Value = 0;
   _Name  = 0;
   _CData = false;

   SetName(name);
   Set(value);
}


XmlData::XmlData(const char *name, const double value)
{
   _Value = 0;
   _Name  = 0;
   _CData = false;

   SetName(name);
   Set(value);
}


XmlData::XmlData(const char *name, const bool value)
{
   _Value = 0;
   _Name  = 0;
   _CData = false;

   SetName(name);
   Set(value);
}


/*------------------------------------------------------------------------------
   XmlData Destructor
------------------------------------------------------------------------------*/
XmlData::~XmlData()
{
   if (_Value)
      free(_Value);

   if (_Name)
      free(_Name);
}


/*------------------------------------------------------------------------------
   XmlData SetName
------------------------------------------------------------------------------*/
void XmlData::SetName(const char *name)
{
   if ((_Name = (char *) malloc(strlen(name) + 1)) == 0)
      throw xXmlCore(__FILE__, __LINE__, "XmlData::SetName - Error allocating memory for name");

   strcpy(_Name, name);
}


/*------------------------------------------------------------------------------
   XmlData Set - like Set, except it does no validation
------------------------------------------------------------------------------*/
void XmlData::Set(const char *value)
{
   if (_Value)
   {
      free(_Value);
      _Value = 0;
      _ValueLen = 0;
   }

   if (!value)
      return;

   _ValueLen = strlen(value);
   if ((_Value = (char *) malloc(_ValueLen+1)) == 0)
       throw xXmlCore(__FILE__, __LINE__, "XmlData::Set - Error allocating memory for value");

   strcpy(_Value, value);
}


void XmlData::Set(const int value)
{
   char jotter[64];

   sprintf(jotter, "%d", value);

   Set(jotter);
}


void XmlData::Set(const double value)
{
   char jotter[64];

   sprintf(jotter, "%.2lf", value);

   Set(jotter);
}


void XmlData::Set(const bool value)
{
   Set(value ? "True" : "False");
}


/*------------------------------------------------------------------------------
   Converts name to uppercase
------------------------------------------------------------------------------*/
void XmlData::NameToUpper()
{
   char *p = _Name;

   for (; *p; p++)
      *p = toupper(*p);
}


/*------------------------------------------------------------------------------
   Converts name to uppercase
------------------------------------------------------------------------------*/
void XmlData::NameToLower()
{
   char *p = _Name;

   for (; *p; p++)
      *p = tolower(*p);
}


/*------------------------------------------------------------------------------
   Gets the raw version of a xml value
------------------------------------------------------------------------------*/
char *XmlData::GetRaw(char *dest, const unsigned dest_size) const
{
   if (!_Value)
      return "";

   if ( _CData && 
        dest_size > (_ValueLen + 16)  )
   {
     sprintf( dest, "\n<![CDATA[ %*s ]]>\n", this->_ValueLen, this->_Value );
   }
   else
   {
      xmlCore_MakeValueRaw(_Value, dest, dest_size);
   }
   

   return dest;
}


/*------------------------------------------------------------------------------
   XmlAttributes Constructor
------------------------------------------------------------------------------*/
XmlAttributes::XmlAttributes()
{
}


/*------------------------------------------------------------------------------
   XmlAttributes Destructor
------------------------------------------------------------------------------*/
XmlAttributes::~XmlAttributes()
{
   _Attributes.Purge();
}


/*------------------------------------------------------------------------------
   Add
------------------------------------------------------------------------------*/
void XmlAttributes::Add(XmlData *data)
{
   _Attributes.Append(data);
}


/*------------------------------------------------------------------------------
   Remove
------------------------------------------------------------------------------*/
void XmlAttributes::Remove(const char *name)
{
   unsigned index;

   for (index = 0; index < _Attributes.Count(); index++)
      if (STR_COMPARE(name, _Attributes[index].Name()) == 0)
      {
         _Attributes.Remove(index);
         return;
      }
}


void XmlAttributes::Remove(const unsigned index)
{
   _Attributes.Remove(index);
}


/*------------------------------------------------------------------------------
   Clear
------------------------------------------------------------------------------*/
void XmlAttributes::Clear()
{
   _Attributes.Purge();
}


/*------------------------------------------------------------------------------
   Has, also fills the value if you pass in a valid char **
------------------------------------------------------------------------------*/
bool XmlAttributes::Has(const char *name, char **value) const
{
   unsigned index;

   if (Has(name, &index))
   {
      if (value)
         *value = _Attributes[index].Value();

      return true;
   }

   return false;
}


/*------------------------------------------------------------------------------
   Has, returns the index if you specify a valid index pointer
------------------------------------------------------------------------------*/
bool XmlAttributes::Has(const char *name, unsigned *outIndex) const
{
   unsigned index;

   for (index = 0; index < _Attributes.Count(); index++)
      if (STR_COMPARE(name, _Attributes[index].Name()) == 0)
      {
         if (outIndex)
            *outIndex = index;
         
         return true;
      }

   return false;
}


/*------------------------------------------------------------------------------
   [] operators
------------------------------------------------------------------------------*/
XmlData &XmlAttributes::operator [] (const char *name) const
{
   unsigned index;

   for (index = 0; index < _Attributes.Count(); index++)
      if (STR_COMPARE(name, _Attributes[index].Name()) == 0)
      {
         break;
      }

   return _Attributes[index];
}


XmlData &XmlAttributes::operator [] (const unsigned index) const
{
   return _Attributes[index];
}



/*------------------------------------------------------------------------------
   Data objects
------------------------------------------------------------------------------*/
XmlData *XmlAttributes::Data(const char *name) const
{
   unsigned index;

   for (index = 0; index < _Attributes.Count(); index++)
      if (STR_COMPARE(name, _Attributes[index].Name()) == 0)
      {
         break;
      }

   return _Attributes.Get(index);
}


XmlData *XmlAttributes::Data(const unsigned index) const
{
   return _Attributes.Get(index);
}



/*------------------------------------------------------------------------------
   XmlNode Constructors
------------------------------------------------------------------------------*/
XmlNode::XmlNode()
{
   Data       = 0;
   Attributes = 0;
   Sibling    = 0;
   Parent     = 0;
   Child      = 0;
}


XmlNode::XmlNode(XmlData *data, XmlNode *parent)
{
   if (data == 0)
      throw xXmlCore(__FILE__, __LINE__, "XmlNode::XmlNode - Cannot init XmlNode with a null object");

   Data       = data;
   Attributes = 0;
   Sibling    = 0;
   Parent     = parent;
   Child      = 0;
}


/*------------------------------------------------------------------------------
   XmlNode Destructor
------------------------------------------------------------------------------*/
XmlNode::~XmlNode()
{
   Destruct();
}


/*------------------------------------------------------------------------------
   Clears the node entirely
------------------------------------------------------------------------------*/
void XmlNode::Destruct()
{
   _DELETE(Data)
   _DELETE(Attributes)
   _DELETE(Child)
   _DELETE(Sibling)
}


/*------------------------------------------------------------------------------
   Append Sibling
------------------------------------------------------------------------------*/
XmlNode &XmlNode::operator = (XmlNode &node)
{
  //Dieter Rosch - Fix another one of Nic "My code doesn't leak" Milicovic's stupid FUCKING bugs.
	//      Arrogant prick...

	_DELETE(Data);
	_DELETE(Attributes);
  _DELETE(Sibling);
	_DELETE(Parent);
	_DELETE(Child);

   Data       = node.Data;
   Attributes = node.Attributes;
   Sibling    = node.Sibling;
   Parent     = node.Parent;
   Child      = node.Child;

   //This is NOT a proper = operator. !!!! ARGH
   //Inserted this code to fix bug where 1st level tags lose their parent pointer ( the root node )
   //Will REMOVE this in future!
   if(node.Child && (node.Child->Parent == &node))   
   {
     //Dest node becomes new parent for this nodes child and child nodes siblings
     node.Child->Parent = this;
	 for(XmlNode* siblingNode = node.Child->Sibling; siblingNode; siblingNode = siblingNode->Sibling)
		 siblingNode->Parent = this;
   }

   node.Data       = 0;
   node.Attributes = 0;
   node.Sibling    = 0;
   node.Parent     = 0;
   node.Child      = 0;

   return *this;
}


/*------------------------------------------------------------------------------
   Append Sibling
------------------------------------------------------------------------------*/
XmlNode *XmlNode::AppendSibling(XmlData *data)
{
   XmlNode *node;

   for (node = this; node->Sibling; node = node->Sibling);

   node->Sibling = new XmlNode(data, this->Parent);

   return node->Sibling;
}


/*------------------------------------------------------------------------------
   Append Child
------------------------------------------------------------------------------*/
XmlNode *XmlNode::AppendChild(XmlData *data)
{
   if (!Child)
   {
      Child = new XmlNode(data, this);
      return Child;
   }

   return Child->AppendSibling(data);
}


/*------------------------------------------------------------------------------
   Remove Sibling
------------------------------------------------------------------------------*/
void XmlNode::RemoveSibling(const char *name)
{
   if (!name)
   {
      delete Sibling;
      Sibling = 0;
      return;
   }

   XmlNode *node;
   XmlNode *prior = 0;

   for (node = this; node; prior = node, node = node->Sibling)
      if (STR_COMPARE(node->Data->Name(), name) == 0)
      {
         if (prior)
            prior->Sibling = node->Sibling;
         else
            node->Parent->Child = node->Sibling;

         if (node->Sibling)
            node->Sibling = 0;

         delete node;
         return;
      }
}


/*------------------------------------------------------------------------------
   Remove Child
------------------------------------------------------------------------------*/
void XmlNode::RemoveChild(const char *name)
{
   if (!name)
   {
      delete Child;
      Child = 0;
      return;
   }

   Child->RemoveSibling(name);
}

/*------------------------------------------------------------------------------
   Remove Current Node
------------------------------------------------------------------------------*/
void XmlNode::Remove()
{
   //Quick function to remove list since this functionality is not
   //provided for with RemoveChild and RemoveSibling because of duplicate tag names in XML
   //Will only work for lowest level tags ie tags with no children
   //Will improve later to work for parent nodes as well to delete all children and sibling etc under node to delete
   XmlNode *currNode;
   XmlNode *prevNode = 0;

   for (currNode = this->Parent->Child; currNode; prevNode = currNode, currNode = currNode->Sibling)
   {
      if (currNode == this)
      {
         if (prevNode)
            prevNode->Sibling = currNode->Sibling;
         else
            currNode->Parent->Child = currNode->Sibling;

		 //Set Sibling* to 0 in order for delete to NOT destroy it
         currNode->Sibling = 0;
         delete currNode;
         return;
      }
   }
}

/*------------------------------------------------------------------------------
   Adds an attribute
------------------------------------------------------------------------------*/
void XmlNode::AddAttribute(XmlData *data)
{
   if (!Attributes)
      Attributes = new XmlAttributes();

   Attributes->Add(data);
}


/*------------------------------------------------------------------------------
   Sets new data for this node
------------------------------------------------------------------------------*/
void XmlNode::NewData(const char *name, const char *value)
{
   if (Data)
   {
      delete Data;
      Data = 0;
   }

   Data = new XmlData(name,  value);
}


/*------------------------------------------------------------------------------
   Loads the xml from a file
------------------------------------------------------------------------------*/
void XmlNode::LoadFile(XmlNode &root, const char *fileName)
{
   long         len;
   char        *xml  = 0;
   FILE        *fp   = 0;

   try
   { 
      if ((fp = fopen(fileName, "rb")) == 0)
         throw xXmlCore(__FILE__, __LINE__, "XmlNode::LoadFile - Error opening file '%s'", fileName);

      if (fseek(fp, 0L, SEEK_END) == -1 ||
          (len = ftell(fp))       == -1 ||
          fseek(fp, 0L, SEEK_SET) == -1)
         throw xXmlCore(__FILE__, __LINE__, "XmlNode::LoadFile - Error accessing file '%s'", fileName);

      if ((xml = (char *) malloc(len + 1)) == 0)
         throw xXmlCore(__FILE__, __LINE__, "XmlNode::LoadFile - Error allocating memory for file '%s'/%ld", fileName, len);

      if (fread(xml, len, 1, fp) != 1)
         throw xXmlCore(__FILE__, __LINE__, "XmlNode::LoadFile - Error reading file '%s'", fileName);

      fclose(fp);
      
      fp = 0;

      XmlNode::Load(root, xml);

      free(xml);
   }
   catch (...)
   {
      if (fp)
         fclose(fp);

      if (xml)
         free(xml);

      throw;
   }
}


/*------------------------------------------------------------------------------
   Loads the xml from memory
------------------------------------------------------------------------------*/
void XmlNode::Load(XmlNode &root, const char *xml)
{
   XmlNode *local_root = 0;
   XmlNode *node;
   char    *pos  = (char *) xml;
   short    rc;

   try
   {
		 if (xmlCore_OpenClose(pos, local_root) != XC_OPEN)
			 throw xXmlCore(__FILE__, __LINE__, "XmlNode::Load - Cannot open root tag");

		 node = local_root;

		 //Added by: Dieter
		 //Stupid piece of shit doesn't correctly handle attributes on root element.
		 xmlCore_ReadAttributes(pos, node);

     for (;;)
      {
         rc = xmlCore_OpenClose(pos, node);

         if (rc == XC_FINISHED)
            break;

         if (rc == XC_CLOSE)
         {
            if (node == local_root)
               break;

            node = node->Parent;
         }

				 if (rc == XC_OPEN)
				 {
					 if (xmlCore_ReadAttributes(pos, node) == XC_CLOSE)
					 {
						 //If child tag is an include tag replace section with file.
						 //Will improve this later...maybe.
						 if (STR_COMPARE(node->Data->Name(), "xi:include") == 0)
							 xmlCore_xiInclude(pos, node, node->Data->Name());

						 node = node->Parent;
					 }
					 else
					 {
						 xmlCore_ReadValue(pos, node);
					 }
				 }
			}

      if (node != local_root)
         throw xXmlCore(__FILE__, __LINE__, "XmlNode::Load - Final close tag does not match root tag");
   }
   catch (...)
   {
      if (local_root)
         delete local_root;

	  throw;
   }

   root = *local_root;
   
   delete local_root;
}


/*------------------------------------------------------------------------------
   Converts all the elements in this node to lower case
------------------------------------------------------------------------------*/
void XmlNode::UpperCaseElements(XmlNode &node)
{
   ChangeElementCase(&node, true);
}


/*------------------------------------------------------------------------------
   Converts all the elements in this node to lower case
------------------------------------------------------------------------------*/
void XmlNode::LowerCaseElements(XmlNode &node)
{
   ChangeElementCase(&node, false);
}

/*------------------------------------------------------------------------------
   Function to strip the xml of empty tags and still maintain the link list integrity
   Was written outside xmlCore for the itsfilebuilder - Rohan
------------------------------------------------------------------------------*/
void XmlNode::StripXML(XmlNode* BaseNode, XmlNode* StartNode)
{
  XmlNode* nextStartNode = NULL;
  XmlNode* currentNode = NULL;
  XmlNode* currentNodeSibling = NULL;
  XmlNode* currentNodeParent = NULL;

  bool firstTime = true;
  while ( (BaseNode != NULL && StartNode != NULL) && (BaseNode != StartNode || firstTime) )
  {
    firstTime = false;
    //Look from start node tag until lowest tag (tag without child)
    for (currentNode = (XmlNode*)StartNode; currentNode->Child; currentNode = currentNode->Child)
    {
      //SLOG->lprintf(eLogDebug, "Current Tag %s", currentNode->Data->Name() );
    }
    //SLOG->lprintf(eLogDebug, "Lowest Tag %s", currentNode->Data->Name() );
    currentNodeParent  = currentNode->Parent;
    currentNodeSibling = currentNode->Sibling;

    //Remove if empty
    if ( strlen(currentNode->Value()) == 0 )
    {
      //SLOG->lprintf(eLogDebug, "Remove Emty Tag %s", currentNode->Data->Name() );
      currentNode->Remove();
      currentNode = NULL;
    }
    //Check siblings next if there is
    if (currentNodeSibling)
    {
      StartNode = currentNodeSibling;
      continue;
    }
    //Look for next Parent with Sibling until Base tag is found
    for (currentNode = currentNodeParent; currentNode; currentNode = currentNodeParent)
    {
      currentNodeParent  = currentNode->Parent;
      currentNodeSibling = currentNode->Sibling;

      //Found BaseNode/Root tag - finish
      if (currentNode == BaseNode)
      {
        //SLOG->lprintf(eLogDebug, "BaseNode %s reached - finished stripping", BaseNode->Data->Name());
        StartNode = currentNode;
        break;
      }

      //Remove parent node with no child or value
      if (currentNode->Child == NULL && strlen(currentNode->Value()) == 0)
      {
        //SLOG->lprintf(eLogDebug, "Remove Emty Parent Tag %s", currentNode->Data->Name() );
        currentNode->Remove();
        currentNode = NULL;
      }
      //Check
      if (currentNodeSibling)
      {
        //SLOG->lprintf(eLogDebug, "Found Next Sibling %s", currentNodeSibling->Data->Name() );
        StartNode = currentNodeSibling;
        break;
      }
    }
  }
  //SLOG->lprintf(eLogDebug, "Finished stripping XML");
}

/*------------------------------------------------------------------------------
   recursive change element case function
------------------------------------------------------------------------------*/
void XmlNode::ChangeElementCase(XmlNode *node, const bool upper)
{
   upper ? node->Data->NameToUpper() : node->Data->NameToLower();

   if (node->Attributes)
   {
      unsigned index;

      for (index = 0; index < node->Attributes->Count(); index++)
         upper ? node->Attributes->Data(index)->NameToUpper() : node->Attributes->Data(index)->NameToLower();
   }
   
   if (node->Child)
      ChangeElementCase(node->Child, upper);

   if (node->Sibling)
      ChangeElementCase(node->Sibling, upper);
}


/*------------------------------------------------------------------------------
   Returns the xml data object of the associated path
------------------------------------------------------------------------------*/
char *XmlNode::Get(const char *path) const
{
   XmlData *data = Exists(path);

   if (!data)
      throw xXmlCore(__FILE__, __LINE__, "XmlNode::Value - the path (%s) is not valid", path);

   return data->Value();
}


/*------------------------------------------------------------------------------
   Returns the xml node object via the path
------------------------------------------------------------------------------*/
XmlNode *XmlNode::GetNode(const char *path) const
{
   XmlNode *node = ExistsNode(path);

   if (!node)
      throw xXmlCore(__FILE__, __LINE__, "XmlNode::GetNode - the path (%s) is not valid", path);

   return node;
}


/*------------------------------------------------------------------------------
   Returns the xml data object if it exists
------------------------------------------------------------------------------*/
XmlData *XmlNode::Exists(const char *path) const
{
   char      jotter[XC_IDENTIFIER_MAX_LEN + XC_IDENTIFIER_MAX_LEN];
   char      attr[XC_IDENTIFIER_MAX_LEN];
   bool      last = false;
   char     *ptr  = (char *) path;
   XmlNode  *node = (XmlNode *) this;

   attr[0] = 0;
   
   while (!last)
   {
      last = GetSubStr(ptr, jotter);

      if (last)
         SplitAttr(jotter, attr);

      if (jotter[0])
      {
         for (node = node->Child; node; node = node->Sibling)
            if (STR_COMPARE(node->Data->Name(), jotter) == 0)
               break;

         if (!node)
            break;
      }

      if (last)
      {
         if (attr[0])
         {
            if (!node->Attributes)
               return 0;

            unsigned index;

            if (!node->Attributes->Has(attr, &index))
               return 0;

            return node->Attributes->Data(index);
         }

         return node->Data;
      }
   }

   return 0;
}


/*------------------------------------------------------------------------------
   Returns the xml data object if it exists
------------------------------------------------------------------------------*/
XmlNode *XmlNode::ExistsNode(const char *path) const
{
   char      jotter[XC_IDENTIFIER_MAX_LEN + XC_IDENTIFIER_MAX_LEN];
   bool      last = false;
   char     *ptr  = (char *) path;
   XmlNode  *node = (XmlNode *) this;

   while (!last)
   {
      last = GetSubStr(ptr, jotter);   

      for (node = node->Child; node; node = node->Sibling)
         if (STR_COMPARE(node->Data->Name(), jotter) == 0)
            break;

      if (!node)
         break;

      if (last)
         return node;
   }

   return 0;
}


/*------------------------------------------------------------------------------
   returns true if this is the last string 
------------------------------------------------------------------------------*/
bool XmlNode::GetSubStr(char *&ptr, char *dest) const
{
   char *start = ptr;

   for (; *ptr; ptr++)
      if (*ptr == NODE_DELIMETER)
      {
         memcpy(dest, start, ptr - start);
         dest[ptr - start] = 0;
         ptr++;
         return false;
      }

   strcpy(dest, start);
   return true;
}


/*------------------------------------------------------------------------------
   splits the identifier up from the attribute if one was specified
------------------------------------------------------------------------------*/
void XmlNode::SplitAttr(char *identifier, char *attr) const
{
   char *ptr;

   for (ptr = identifier; *ptr; ptr++)
      if (*ptr == ATTR_DELIMETER)
      {
         *ptr = 0;
         ptr++;
         strcpy(attr, ptr);
         return;
      }
}


/*------------------------------------------------------------------------------
   prints the xml tree to file poiner 
------------------------------------------------------------------------------*/
void XmlNode::Print(FILE *dest, const XmlNode *top)
{
   xmlCore_PrintNode(dest, top, top , 0);
}


/*------------------------------------------------------------------------------
   prints the xml tree to file poiner 
------------------------------------------------------------------------------*/
void XmlNode::PrintRaw(FILE *dest, const XmlNode *top)
{
   xmlCore_PrintRawNode(dest, top, top, 0);
}


/*------------------------------------------------------------------------------
   prints the xml tree to file poiner 
------------------------------------------------------------------------------*/
void XmlNode::ToString(char *out, const unsigned outLen, const XmlNode *top)
{
   memset(out, 0, outLen);

   unsigned currLen = 0;

   xmlCore_ToString(out, outLen - 1, currLen, top, top);
}

/*------------------------------------------------------------------------------
   prints the xml tree to file poiner 
------------------------------------------------------------------------------*/
void XmlNode::Tocstr(char *out, const unsigned outLen, XmlNode *top)
{
   memset(out, 0, outLen);

   unsigned currLen = 0;

   xmlCore_c_str(out, outLen - 1, currLen, top, top);
}


/*------------------------------------------------------------------------------
   finds a node, first call, node should be null
------------------------------------------------------------------------------*/
bool XmlNode::FindNode(XmlNode *&node, const char *name) const
{
   if (!node)
      node = this->Child;
   else
      node = node->Sibling;

   for (; node; node = node->Sibling)
      if (STR_COMPARE(node->Data->Name(), name) == 0)
         return true;

   return false;
}

      
/*------------------------------------------------------------------------------
   Just searches for a child from the current node, returns null if not found
------------------------------------------------------------------------------*/
XmlNode *XmlNode::HasChild(const char *name) const
{
   XmlNode *node;

   for (node = this->Child; node; node = node->Sibling)
      if (STR_COMPARE(node->Data->Name(), name) == 0)
         return node;

   return 0;
}


/*------------------------------------------------------------------------------
   Just searches for a sibling from the current node, returns null if not found
------------------------------------------------------------------------------*/
XmlNode *XmlNode::HasSibling(const char *name) const
{
   XmlNode *node;

   for (node = this->Sibling; node; node = node->Sibling)
      if (STR_COMPARE(node->Data->Name(), name) == 0)
         return node;

   return 0;
}

