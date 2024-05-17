#ifndef __XML_CORE_H_
#define __XML_CORE_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(__XML_CORE_H_);

#include "xcept.h"
#include "collection.h"

#include <stdio.h>

/*-----------------------------------
   Exception object
-----------------------------------*/
class xXmlCore : public xCept
{
   public:
      xXmlCore(const char *file, const int line, const char *fmt, ...);

      xXmlCore(const xXmlCore &x) : xCept(x) { }
};

static char __empty[1];   
/*-----------------------------------
   XmlData object
-----------------------------------*/
class XmlData
{
   public :

                  XmlData(const char *name);
                  XmlData(const char *name, const char   *value);
                  XmlData(const char *name, const char   *value, const bool CData);
                  XmlData(const char *name, const int     value);
                  XmlData(const char *name, const double  value);
                  XmlData(const char *name, const bool    value);
                 ~XmlData();

      void        Set(const char   *value);
      void        Set(const int     value);
      void        Set(const double  value);
      void        Set(const bool    value);

      void        NameToUpper();
      void        NameToLower();

      char       *GetRaw(char *dest, const unsigned dest_size) const;

      char       *Value() const
      {
         return _Value == 0 ? __empty : _Value;
      }

      char       *Name() const
      {
         return _Name;
      }

      int        ValueLen()
      {
         return _ValueLen;
      }

      void        SetCData(const bool    value)
      {
        this->_CData = value;
      }

      bool        GetCData()
      {
        return _CData;
      }


      
   private :

      void        SetName(const char *name);

      char       *_Name;
      char       *_Value;
      int        _ValueLen;
      bool       _CData;
};


/*-----------------------------------
   XmlAttributes object
-----------------------------------*/
class XmlAttributes
{
   public :

                  XmlAttributes();
                 ~XmlAttributes();

      void        Add(XmlData *data);

      void        Remove(const char *name = 0);

      void        Remove(const unsigned index);

      void        Clear();

      bool        Has(const char *name, char **value = 0) const;

      bool        Has(const char *name, unsigned *outIndex = 0) const;

      unsigned    Count() const
      {
         return _Attributes.Count();
      }
      
      XmlData    &operator [] (const char *name) const;

      XmlData    &operator [] (const unsigned index) const;

      XmlData    *Data(const unsigned index) const;

      XmlData    *Data(const char *name) const;

   private :

      Collection<XmlData>  _Attributes;

};


/*-----------------------------------
   xml_node object
-----------------------------------*/
class XmlNode
{
   public :
            
                      XmlNode();

                      XmlNode(XmlData *data, XmlNode *parent = 0);

                     ~XmlNode();

      void            Destruct();

      XmlNode        *AppendSibling(XmlData *data);

      XmlNode        *AppendChild(XmlData *data);

      void            RemoveSibling(const char *name = 0);

      void            RemoveChild(const char *name = 0);
 
	  void            Remove();

      void            AddAttribute(XmlData *data);

      void            NewData(const char *name, const char *value);

      XmlNode        &operator = (XmlNode &node);

      XmlNode        *HasChild(const char *name) const;

      XmlNode        *HasSibling(const char *name) const;

      bool            FindNode(XmlNode *&node, const char *name) const;

      char           *Get(const char *path) const;

      XmlNode        *GetNode(const char *path) const;
      
      XmlData        *Exists(const char *path) const;

      XmlNode        *ExistsNode(const char *path) const;

      char           *Name() const { return Data->Name(); }

      char           *Value() const { return Data->Value(); }

      XmlData        *Data;

      XmlAttributes  *Attributes;

      XmlNode        *Parent;

      XmlNode        *Child;

      XmlNode        *Sibling;

      static void     ToString(char *out, const unsigned outLen, const XmlNode *top);

      static void     Tocstr(char *out, const unsigned outLen, XmlNode *top);

      static void     Print(FILE *dest, const XmlNode *top);

      static void     PrintRaw(FILE *dest, const XmlNode *top);

      static void     LoadFile(XmlNode &root, const char *fileName);

      static void     Load(XmlNode &root, const char *xml);

      static void     UpperCaseElements(XmlNode &node);

      static void     LowerCaseElements(XmlNode &node);

	  static void     StripXML(XmlNode* BaseNode, XmlNode* StartNode);

   private :

      bool            GetSubStr(char *&ptr, char *dest) const;

      void            SplitAttr(char *identifier, char *attr) const;

      static void     ChangeElementCase(XmlNode *node, const bool upper);
};

#endif

