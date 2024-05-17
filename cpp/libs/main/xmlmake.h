#ifndef _XML_MAKE_H
#define _XML_MAKE_H "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(_XML_MAKE_H);

#include <stdlib.h>
#include "xcept.h"
#include "addlist.h"

struct TArrayPair
{
  int no;
  int of;
  TArrayPair(int n=0, int o=1)
  {
    no = n;
    of = o;
  }
  TArrayPair(TArrayPair &x)
  {
    no = x.no;
    of = x.of;
  }
};

struct TXMLBase
{
  char *Name;
  TXMLBase(char *AName);
  virtual ~TXMLBase();
};

struct TXMLAttribute : public TXMLBase
{
  char *Value;
  TXMLAttribute(char *AName, char *AValue);
  virtual ~TXMLAttribute();
  void AddValue(char *AValue);
};

struct TXMLElement : public TXMLBase
{
  TAddList<char*, int> Values;
  TAddList<TXMLAttribute*, int> Attributes;
  TAddList<TXMLElement*, int> Elements;
  TXMLElement *Parent;
  long ElementIndex, AttrIndex, ValueIndex;
  TArrayPair arrayPair;  // zero based
  TXMLElement(char *AName, char *AValue=0);
  virtual ~TXMLElement();
  void AddValue(char *AValue);
  void AddValue(char *AValue, int &nlCount);
  void AddAttribute(TXMLAttribute *AAttribute);
  void AddElement(TXMLElement *AElement);
  TXMLElement *FindElement(char *AName, TArrayPair &arrayPair);
  TXMLAttribute *FindAttribute(char *AName);
};

enum eErrorXML
{
   errxmlEROK,
   errxmlAttr,
   errxmlName,
   errxmlNull,
   errxmlTopNotSame,
   errxmlAlreadyLoaded
};


class xXMLMake : public xCept
{
   public :
      xXMLMake(char* aFname, int aLine, eErrorXML aError, char* aMsg=0);
      xXMLMake(const xXMLMake& aX)
      : xCept(aX) {};
};

#define XXML(err) \
        xXMLMake(__FILE__, __LINE__, err)
#define XXML1(err, msg) \
        xXMLMake(__FILE__, __LINE__, err, msg)

#endif
