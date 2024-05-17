#include "versions.h"
#define VERSION_NO "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
COMPILED_VERSION(VERSION_NO);

#include "xmlmake.h"
#include "xstring.h"
#include <malloc.h>

#ifdef M_STDLIB
using namespace std;
#endif

TXMLBase::TXMLBase(char *AName)
{
  if (!strlen(AName))
    throw XXML(errxmlAttr);
  Name = new char[strlen(AName)+1];
  strcpy(Name, AName);
}

TXMLBase::~TXMLBase()
{
  delete [] Name;
}

TXMLAttribute::TXMLAttribute(char *AName, char *AValue)
: TXMLBase(AName)
{
  Value = new char[strlen(AValue)+1];
  strcpy(Value, AValue);
}

TXMLAttribute::~TXMLAttribute()
{
  delete [] Value;
}

void TXMLAttribute::AddValue(char *AValue)
{
  char* oldValue = Value;
  Value = new char[strlen(oldValue)+strlen(AValue)+2];
  strcpy(Value, oldValue);
  strcat(Value, ";");
  strcat(Value, AValue);
  delete [] oldValue;
}

TXMLElement::TXMLElement(char *AName, char *AValue)
: TXMLBase(AName)
, Values(4)
, Attributes(4)
, Elements(8)
, Parent(0)
{
  if (AValue)
    AddValue(AValue);
}

TXMLElement::~TXMLElement()
{
  int n;
  if ((n = Values.getCount()) > 0)
  {
    for (int i=0; i < n; i++)
      delete [] Values[i];
    Values.clear();
  }
  if ((n = Attributes.getCount()) > 0)
  {
    for (int i=0; i < n; i++)
      delete Attributes[i];
    Attributes.clear();
  }
  if ((n = Elements.getCount()) > 0)
  {
    for (int i=0; i < n; i++)
      if (Elements[i])
        delete Elements[i];
    Elements.clear();
  }
}

void TXMLElement::AddValue(char *AValue)
{
  int nlCount=0;
  AddValue(AValue, nlCount);
}

void TXMLElement::AddValue(char *AValue, int &nlCount)
{
  char *Value;
  int n=strlen(AValue);
  int last = Values.getCount()-1;
  if (last == -1)
  {
    Value = new char[n+1];
    strcpy(Value, AValue);
    Values.add(Value);
    return;
  }
  if (n == 1 && strchr("\n", *AValue) != 0)
  {
    Value = new char[1];
    Value[0] = 0;
    Values.add(Value);
    return;
  }
  int m = strlen(Values[last]);
  Value = new char[m+n+nlCount+1];
  strcpy(Value, Values[last]);
  for (int i=0; i<nlCount; i++)
    strcat(Value, "\n");
  nlCount = 0;
  strcat(Value, AValue);
  delete [] Values[last];
  Values[last] = Value;
}

void TXMLElement::AddAttribute(TXMLAttribute *AAttribute)
{
  Attributes.add(AAttribute);
}

void TXMLElement::AddElement(TXMLElement *AElement)
{
  for (int i=0; i<Elements.getCount(); i++)
  {
    if (stricmp(Elements[i]->Name, AElement->Name) == 0)
    {
      Elements[i]->arrayPair.of++;
      AElement->arrayPair.no++;
      AElement->arrayPair.of = Elements[i]->arrayPair.of;
    }
  }
  Elements.add(AElement);
  AElement->Parent = this;
}

TXMLElement *TXMLElement::FindElement(char *AName, TArrayPair &arrayPair)
{
  for (int i=0; i < Elements.getCount(); i++)
  {
    if (stricmp(AName, Elements[i]->Name) == 0)
    {
      arrayPair.of = Elements[i]->arrayPair.of;
      if (arrayPair.no == Elements[i]->arrayPair.no)
        return Elements[i];
    }
  }
  return 0;
}

TXMLAttribute *TXMLElement::FindAttribute(char *AName)
{
  for (int i=0; i < Attributes.getCount(); i++)
    if (stricmp(AName, Attributes[i]->Name) == 0)
      return Attributes[i];
  return 0;
}

char* EXMLError[] =
{ "OK"
, "Null Pointer passed"
, "Tag Name not specified"
, "Attr Name not specified"
, "The Top Element is not the same"
, "Load has already been done"
};

xXMLMake::xXMLMake(char* aFname, int aLine, eErrorXML aError, char* aMsg)
: xCept(aFname, aLine, "XMLMake", aError)
{
  osErr << aFname << "(" << aLine << ")";
  if(aError)
    osErr << "Error(" << EXMLError[aError] << ")";
  if(aMsg)
    osErr << aMsg << std::endl;
  osErr << ends;
}

