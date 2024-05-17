#ifndef __XMLRECORD_H__
#define __XMLRECORD_H__ "$Revision: 117 $ $Date: 2005-05-03 16:03:44 +0200 (Tue, 03 May 2005) $"
#include "versions.h"
HEADER_VERSION(__XMLRECORD_H__);

#include "expat.h"

#include "xmlmake.h"
#include "tbuffer.h"

typedef TAddList<TArrayPair, int> TArrayList;

struct TXMLRecord
{
  TXMLElement *Top;
  TBChar version;
  TBChar encoding;
  TBChar target;
  TBChar data;
  char memberChar;
  char attributeChar;
  char namespaceChar;
  char arrayChar;
  TXMLRecord(char aMemberChar = '.', char aAttributeChar = '/', char aNamespaceChar = '_', char aArrayChar = '%')
      : Top(0)
      , memberChar(aMemberChar)
      , attributeChar(aAttributeChar)
      , namespaceChar(aNamespaceChar)
      , arrayChar(aArrayChar)
      , CurrElement(Top)
      , version(16)
      , encoding(16)
      , target(16)
      , data(16)
      , alreadyLoaded(false)
  {}
  ~TXMLRecord();
  void InsertTag(char *key, char *value);
  void AsXML(TBChar &Message, int flat=0, int noEmpties=0);
  void AsYaml(TBChar& yaml);
  void Start()
  {
    CurrElement = Top;
    if (CurrElement)
      CurrElement->ElementIndex = CurrElement->AttrIndex = CurrElement->ValueIndex = 0;
  }
  bool Next(TBChar &key, TBChar &value, int &noOf, int &count);
  bool Next(TBChar &key, TBChar &value, TArrayPair &valueArray);
  void NewElement(char *key, char *value=0);
  void InsertValue(char *value);
  void InsertValue(char *value, int &nlCount);
  void InsertAttribute(char *key, char *value);
  void ToParent() {if (CurrElement) CurrElement = CurrElement->Parent;}
  void Element(char *key, char *value=0) {ToParent();NewElement(key,value);}
  void GetValue(char *key, TBChar &value);
  void GetValue(char *key, TBChar &value, char *defValue);
  void GetValue(char *key, TBChar &value, TArrayList &arrayList);
  void GetValue(char *key, TBChar &value, TArrayList &arrayList, char *defValue);
  int  Load(TBChar &Input);
  int  Load(const char* xml)
  {
    TBChar work;
    work.set(xml);
    return Load(work);
  }
  int  LoadFile(const char *fileName);
  void Clear() 
  { 
    if (Top) 
    { 
      delete Top; 
      Top = 0; 
    } 
    CurrElement = Top;
  }
protected:
  bool alreadyLoaded;
  enum EType {etTop, etElement, etAttribute, etArray};
  TXMLElement *CurrElement;
  void Append(TBChar &Message, char *value);
  void AsXML(TBChar &Message, TXMLElement* Entry, int pad, int flat, int noEmpties);
  void AsYaml(TBChar& yaml, TXMLElement* Entry, int pad = 0);
  void MakeKey(TBChar &Key);
  char* GetSection(char *key, TBChar &section, EType &type);
};

#endif
