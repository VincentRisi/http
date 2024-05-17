#include "versions.h"
#define VERSION_NO "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
COMPILED_VERSION(VERSION_NO);

#include "machine.h"
#include "xstring.h"
#include "xmlrecord.h"
#include <stdio.h>
#include <stdlib.h>
#if !defined(M_AIX) && !defined(M_GNU)
#include <io.h>
#endif
#include <fcntl.h>

void TXMLRecord::InsertTag(char* key, char* value)
{
    TXMLElement* CurrEntry=0, * ThisEntry=0;
    TXMLAttribute* ThisAttribute=0;
    TBChar section(256);
    char* b, * e;
    EType type = etElement;
    for (b = key; *b; b = e)
    {
        e = strchr(b, memberChar);
        if (e == 0)
            e = strchr(b, attributeChar);
        if (e == 0)
            e = b + strlen(b);
        int n = e - b;
        if (*e)
            e++;
        if (b > key && b[-1] == attributeChar)
            type = etAttribute;
        section.resize(n + 1);
        strncpy(section.data, b, n);
        section.data[n] = 0;
        if (b == key)
        {
            if (Top == 0)
                Top = new TXMLElement(section.data);
            else if (stricmp(Top->Name, section.data) != 0)
                throw XXML1(errxmlTopNotSame, "The front tag must be the same");
            CurrEntry = Top;
            continue;
        }
        if (type == etAttribute)
        {
            ThisAttribute = CurrEntry->FindAttribute(section.data);
            if (!ThisAttribute)
            {
                ThisAttribute = new TXMLAttribute(section.data, value);
                CurrEntry->AddAttribute(ThisAttribute);
                return;
            }
            ThisAttribute->AddValue(value);
            return;
        }
        if (CurrEntry == 0)
            throw XXML1(errxmlNull, "CurrEntry is null");
        TArrayPair arrayPair;
        ThisEntry = CurrEntry->FindElement(section.data, arrayPair);
        if (!ThisEntry)
        {
            ThisEntry = new TXMLElement(section.data);
            CurrEntry->AddElement(ThisEntry);
        }
        CurrEntry = ThisEntry;
    }
    if (strlen(value) > 0 && CurrEntry != 0)
        CurrEntry->AddValue(value);
}

char* TXMLRecord::GetSection(char* key, TBChar& section, EType& type)
{
    char* b = key, * e;//, * m;
    if (b[0] == memberChar)
    {
        b++;
        type = etElement;
    }
    else if (b[0] == attributeChar)
    {
        b++;
        type = etAttribute;
    }
    else
        type = etTop;
    e = strchr(b, memberChar);
    if (e == 0)
        e = strchr(b, attributeChar);
    if (e == 0)
        e = b + strlen(b);
    int n = e - b;
    if (*e)
        e++;
    section.resize(n + 1);
    strncpy(section.data, b, n);
    section.data[n] = 0;
    return b + n;
}

static TArrayList emptyArrayList;

void TXMLRecord::GetValue(char* key, TBChar& value)
{
    GetValue(key, value, emptyArrayList);
}

void TXMLRecord::GetValue(char* key, TBChar& value, char* defValue)
{
    GetValue(key, value, emptyArrayList, defValue);
}

void TXMLRecord::GetValue(char* key, TBChar& value, TArrayList& arrayList)
{
    Start();
    TXMLElement* CurrEntry, * ThisEntry;
    TXMLAttribute* ThisAttribute;
    TBChar section(64);
    EType type = etTop;
    CurrEntry = Top;
    char* b = key;
    int pairNo = 0;
    TArrayPair* currPair;
    int n;
    while (*b)
    {
        b = GetSection(b, section, type);
        switch (type)
        {
        case etTop:
            if (stricmp(Top->Name, section.data) == 0)
                break;
        case etElement:
        {
            TArrayPair nonPair(0, 1);
            n = strlen(section.data) - 1;
            if (section.data[n] == arrayChar)
            {
                section.data[n] = 0;
                if (pairNo < arrayList.getCount())
                {
                    currPair = &arrayList[pairNo];
                    pairNo++;
                }
                else
                    throw XXML1(errxmlName, "Element does not have a suitable array pair");
            }
            else
                currPair = &nonPair;
            ThisEntry = CurrEntry->FindElement(section.data, *currPair);
            if (!ThisEntry)
                throw XXML1(errxmlName, "Element Not Found");
            CurrEntry = ThisEntry;
            break;
        }
        case etAttribute:
            ThisAttribute = CurrEntry->FindAttribute(section.data);
            if (!ThisAttribute)
                throw XXML1(errxmlAttr, "Attribute Not Found");
            value.set(ThisAttribute->Value);
            return;
        }
    }
    for (int i = 0; i < CurrEntry->Values.getCount(); i++)
    {
        if (i == 0)
            value.set(CurrEntry->Values[0]);
        else
        {
            value.append("\n");
            value.append(CurrEntry->Values[i]);
        }
    }
}

void TXMLRecord::GetValue(char* key, TBChar& value, TArrayList& arrayList, char* defValue)
{
    try
    {
        GetValue(key, value, arrayList);
    }
    catch (xXMLMake)// & x)
    {
        value.set(defValue);
    }
}

void TXMLRecord::NewElement(char* key, char* value)
{
    TXMLElement* ThisElement;
    ThisElement = new TXMLElement(key, value);
    if (CurrElement == 0 && Top == 0)
        Top = CurrElement = ThisElement;
    else if (CurrElement != 0)
    {
        CurrElement->AddElement(ThisElement);
        CurrElement = ThisElement;
    }
    else
    {
        CurrElement = Top;
        CurrElement->AddElement(ThisElement);
    }
}

void TXMLRecord::InsertValue(char* value)
{
    int nlCount = 0;
    InsertValue(value, nlCount);
}

void TXMLRecord::InsertValue(char* value, int& nlCount)
{
    CurrElement->AddValue(value, nlCount);
}

void TXMLRecord::InsertAttribute(char* key, char* value)
{
    TXMLAttribute* ThisAttribute = new TXMLAttribute(key, value);
    CurrElement->AddAttribute(ThisAttribute);
}

TXMLRecord::~TXMLRecord()
{
    if (Top)
        delete Top;
}

void TXMLRecord::Append(TBChar& Message, char* value)
{
    int no;
    while(true)
    {
        no = strcspn(value, "&<>'\"");
        if (no > 0)
            Message.append(value, no);
        if (value[no] == 0)
            break;
        switch (value[no])
        {
        case '&':
            Message.append("&amp;");
            break;
        case '<':
            Message.append("&lt;");
            break;
        case '>':
            Message.append("&gt;");
            break;
        case '\"':
            Message.append("&quot;");
            break;
        case '\'':
            Message.append("&apos;");
            break;
        }
        value += (no + 1);
    }
}

static int nameLength(const char* name, char arrayChar)
{
    int n = strlen(name);
    const char* p = strchr(name, arrayChar);
    if (p != 0)
        return p - name;
    return n;
}

#if defined M_W32
#  define OS_LINE_FEED "\r\n"
#else
#  define OS_LINE_FEED "\n"
#endif

void TXMLRecord::AsXML(TBChar& Message, TXMLElement* Entry, int pad, int flat, int noEmpties)
{
    const char padValue[] = "                                            "
        "                                            "
        "                                            "
        "                                            ";
    if (noEmpties == 1)
    {
        if (Entry->Values.getCount() == 1 && strlen(Entry->Values[0]) == 0)
            return;
    }
    if (flat == 0)
    {
        if (pad > sizeof(padValue) - 1)
            pad = sizeof(padValue) - 1;
        if (pad)
            Message.append((char*)padValue, pad);
    }
    Message.append("<");
    Message.append(Entry->Name, nameLength(Entry->Name, arrayChar));
    {
        for (int i = 0; i < Entry->Attributes.getCount(); i++)
        {
            TXMLAttribute* Attribute = Entry->Attributes[i];
            Message.append(" ");
            Message.append(Attribute->Name);
            Message.append("=\"");
            Append(Message, Attribute->Value);
            Message.append("\"");
        }
    }
    if ((Entry->Values.getCount() == 0 || Entry->Values.getCount() == 1 && strlen(Entry->Values[0]) == 0)
        && Entry->Elements.getCount() == 0)
    {
        if (flat == 0)
            Message.append("/>" OS_LINE_FEED);
        else
            Message.append("/>");
        return;
    }
    if (Entry->Values.getCount() == 1 && Entry->Elements.getCount() == 0)
    {
        Message.append(">");
        Append(Message, Entry->Values[0]);
    }
    else
    {
        if (flat == 0)
            Message.append(">" OS_LINE_FEED);
        else
            Message.append(">");
        {
            for (int i = 0; i < Entry->Values.getCount(); i++)
            {
                if (flat == 0)
                {
                    if (pad)
                        Message.append((char*)padValue, pad);
                    Message.append("  ");
                }
                Append(Message, Entry->Values[i]);
                if (flat == 0)
                    Message.append(OS_LINE_FEED);
            }
        }
        {
            for (int i = 0; i < Entry->Elements.getCount(); i++)
            {
                TXMLElement* Element = Entry->Elements[i];
                AsXML(Message, Element, pad + 2, flat, noEmpties);
            }
        }
        if (pad)
            Message.append((char*)padValue, pad);
    }
    Message.append("</");
    Message.append(Entry->Name, nameLength(Entry->Name, arrayChar));
    if (flat == 0)
        Message.append(">" OS_LINE_FEED);
    else
        Message.append(">");
}

void TXMLRecord::AsXML(TBChar& Message, int flat, int noEmpties)
{
    AsXML(Message, Top, 0, flat, noEmpties);
}

struct autoChar
{
    char* data;
    autoChar(int size)
    {
        data = new char[size + 4];
    }
    ~autoChar()
    {
        delete[] data;
    }
};

static const char* fixChar(char* work, const char* value)
{
    if (strlen(value) == 0)
        return "''";
    if (strspn(value, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.") == strlen(value))
        return value;
    if (value[0] == '-' && strspn(value+1, "0123456789_.") == strlen(value)-1)
        return value;
    snprintf(work, strlen(value) + 4, "'%s'", value);
    return work;
}

void TXMLRecord::AsYaml(TBChar& yaml, TXMLElement* Entry, int pad)
{
    int noValues = Entry->Values.getCount();
    int noAttributes = Entry->Attributes.getCount();
    int noElements = Entry->Elements.getCount();
    const int size = 512;
    char buff[size];
    char arr[16];
    arr[0] = 0;
    if (Entry->arrayPair.of > 1)
        snprintf(arr, 16, "-%d", Entry->arrayPair.no+1);
    snprintf(buff, size, "%-*s%s%s:"
        , pad * 2
        , ""
        , Entry->Name
        , arr
    );
    yaml.append(buff);
    if (noValues == 0)
    {
        yaml.append(OS_LINE_FEED);
    }
    else if (noValues == 1)
    {
        if (noAttributes == 0)
        {
            char* value = Entry->Values[0];
            autoChar work(strlen(value));
            snprintf(buff, size, " %s" OS_LINE_FEED, fixChar(work.data, value));
            yaml.append(buff);
        }
    }
    else
    {
        yaml.append("|" OS_LINE_FEED);
        for (int i = 0; i < noValues; i++)
        {
            char* value = Entry->Values[i];
            autoChar work(strlen(value));
            snprintf(buff, size, "%-*s%s%s" OS_LINE_FEED, pad * 2, "", "  ", fixChar(work.data, value));
            yaml.append(buff);
        }
    }
    if (noAttributes > 0)
    { 
        if (noValues != 1)
        {
            for (int i = 0; i < noAttributes; i++)
            {
                TXMLAttribute* Attribute = Entry->Attributes[i];
                autoChar work(strlen(Attribute->Value));
                snprintf(buff, size, "%-*s%s%s: %s" OS_LINE_FEED, pad * 2, "", "  ", Attribute->Name, fixChar(work.data, Attribute->Value));
                yaml.append(buff);
            }
        }
        else
        {
            TBChar array;
            array.set(" [");
            char* value = Entry->Values[0];
            autoChar work(strlen(value));
            array.append(fixChar(work.data, value));
            for (int i = 0; i < noAttributes; i++)
            {
                array.append(", ");
                TXMLAttribute* Attribute = Entry->Attributes[i];
                autoChar work(strlen(Attribute->Value));
                array.append(Attribute->Name);
                array.append(": ");
                array.append(fixChar(work.data, Attribute->Value));
            }
            array.append("]" OS_LINE_FEED);
            yaml.append(array.data);
        }
    }
    for (int i = 0; i < noElements; i++)
    {
        TXMLElement* Element = Entry->Elements[i];
        AsYaml(yaml, Element, pad + 1);
    }
}

void TXMLRecord::AsYaml(TBChar& yaml)
{
    AsYaml(yaml, Top, 0);
}

void TXMLRecord::MakeKey(TBChar& Key)
{
    TXMLElement* Curr;
    TBChar W2;
    char work[16];
    Key.clear();
    for (Curr = CurrElement; Curr != 0; Curr = Curr->Parent)
    {
        W2.clear();
        if (Curr->Parent)
            W2.set(memberChar);
        W2.append(Curr->Name);
        if (Curr->arrayPair.of > 1)
        {
            sprintf(work, "%c%d", arrayChar, Curr->arrayPair.no);
            W2.append(work);
        }
        W2.append(Key);
        Key.set(W2);
    }
}

bool TXMLRecord::Next(TBChar& key, TBChar& value, int& noOf, int& count)
{
    TArrayPair valuePair;
    bool result = Next(key, value, valuePair);
    noOf = valuePair.no;
    count = valuePair.of;
    return result;
}

bool TXMLRecord::Next(TBChar& key, TBChar& value, TArrayPair& valuePair)
{
    for (; ; )
    {
        if (CurrElement == 0)
            return false;
        MakeKey(key);
        if (CurrElement->AttrIndex < CurrElement->Attributes.getCount())
        {
            TXMLAttribute* Attribute = CurrElement->Attributes[CurrElement->AttrIndex];
            CurrElement->AttrIndex++;
            key.append(attributeChar);
            key.append(Attribute->Name);
            value.set(Attribute->Value);
            valuePair.no = 0;
            valuePair.of = 1;
            return true;
        }
        if (CurrElement->ValueIndex < CurrElement->Values.getCount())
        {
            value.set(CurrElement->Values[CurrElement->ValueIndex]);
            valuePair.no = CurrElement->ValueIndex;
            valuePair.of = CurrElement->Values.getCount();
            CurrElement->ValueIndex++;
            return true;
        }
        if (CurrElement->ElementIndex < CurrElement->Elements.getCount())
        {
            CurrElement->ElementIndex++;
            CurrElement = CurrElement->Elements[CurrElement->ElementIndex - 1];
            CurrElement->ElementIndex = CurrElement->AttrIndex = CurrElement->ValueIndex = 0;
            continue;
        }
        if (CurrElement->Elements.getCount() == 0 && CurrElement->Values.getCount() == 0)
        {
            valuePair.no = 0;
            valuePair.of = 1;
            value.clear();
            CurrElement = CurrElement->Parent;
            return true;
        }
        CurrElement = CurrElement->Parent;
    }
}

struct autoUserData
{
    TXMLRecord& Record;
    int FilledIndex;
    TBByte Filled;
    int nlCounter;
    autoUserData(TXMLRecord& ARecord) : Record(ARecord), Filled(32), FilledIndex(0), nlCounter(0)
    {
    }
};

static void characterData(void* userData, const XML_Char* s, int len)
{
    autoUserData* aud = (autoUserData*)userData;
    TBChar buff(len + 1);
    buff.append((char*)s, len);
    if (len == 1 && buff.data[0] == '\n')
    {
        aud->nlCounter++;
        return;
    }
    // check for white space only and do not store
    //  -- storage of later data will bring in a newline
    //  -- if needed.
    if (strspn(buff.data, " \t\n") == strlen(buff.data))
        return;
    aud->Record.InsertValue(buff.data, aud->nlCounter);
    aud->nlCounter = 0;
    aud->Filled.data[aud->FilledIndex] = 1;
}

static void startElement(void* userData, const XML_Char* name, const XML_Char** atts)
{
    autoUserData* aud = (autoUserData*)userData;
    aud->nlCounter = 0;
    aud->Record.NewElement((char*)name);
    aud->Filled.data[aud->FilledIndex++] = 1;
    aud->Filled.resize(aud->FilledIndex + 1);
    aud->Filled.data[aud->FilledIndex] = 0;
    TBChar buff(256);
    for (int i = 0; atts[i]; i += 2)
    {
        const XML_Char* att = atts[i];
        const XML_Char* val = atts[i + 1];
        buff.set((char*)val);
        //strtrim(buff.data); -- removed - whatever is between quotes should be left intact Vince 9th Oct 2008
        aud->Record.InsertAttribute((char*)att, buff.data);
        aud->Filled.data[aud->FilledIndex] = 1;
    }
}

static void endElement(void* userData, const XML_Char* /*name*/)
{
    autoUserData* aud = (autoUserData*)userData;
    aud->nlCounter = 0;
    if (aud->Filled.data[aud->FilledIndex] == 0)
        aud->Record.InsertValue("");
    aud->FilledIndex--;
    aud->Record.ToParent();
}

static void procInstr(void* userData, const XML_Char* inTarget, const XML_Char* inData)
{
    autoUserData* aud = (autoUserData*)userData;
    if (inTarget)
        aud->Record.target.append((char*)inTarget);
    aud->Record.target.append("(:>[");
    if (inData)
        aud->Record.data.append((char*)inData);
    aud->Record.data.append("]<:)");
}

int TXMLRecord::Load(TBChar& Input)
{
    if (alreadyLoaded == true)
        throw XXML1(errxmlAlreadyLoaded, "Create another instance, this cannot be reused.");
    alreadyLoaded = true;
    XML_Parser parser;
    parser = XML_ParserCreate(0);
    XML_SetParamEntityParsing(parser, XML_PARAM_ENTITY_PARSING_NEVER);
    XML_SetElementHandler(parser, startElement, endElement);
    XML_SetCharacterDataHandler(parser, characterData);
    XML_SetProcessingInstructionHandler(parser, procInstr);
    autoUserData UD(*this);
    XML_SetUserData(parser, &UD);
    int rc = XML_Parse(parser, Input, strlen(Input), 1);
    XML_ParserFree(parser);
    return rc;
}

struct xmlautoFile
{
    int handle;

    xmlautoFile(const char* fileName)
    {
#if _MSC_VER >= 1300
        handle = _open(fileName, O_RDONLY);
#else
        handle = open(fileName, O_RDONLY);
#endif
    }

    ~xmlautoFile()
    {
#if _MSC_VER >= 1300
        if (handle != -1) _close(handle);
#else
        if (handle != -1) close(handle);
#endif
    }

    operator int()
    {
        return handle;
    }
};

int TXMLRecord::LoadFile(const char* fileName)
{
    xmlautoFile infile(fileName);
    if (infile != -1)
    {
#if _MSC_VER >= 1300
        long size = _lseek(infile, 0, SEEK_END);
        _lseek(infile, 0, SEEK_SET);
        TBChar data(size + 1);
        _read(infile, data.data, size);
#else
        long size = lseek(infile, 0, SEEK_END);
        lseek(infile, 0, SEEK_SET);
        TBChar data(size + 1);
        read(infile, data.data, size);
#endif

        return Load(data);
    }
    return -1;
}



