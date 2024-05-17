#ifndef _XML_DATABUILD_H_
#define _XML_DATABUILD_H_

#include "databuilder2.h"
#include "xmlrecord.h"
#include "base64.h"

using namespace std;

class DataBuilderXML : public DataBuilder
{
    int _count;
    string _name;
    TXMLRecord record;
    TBAmp work;
    char _key[512], _value[512];
    char* key(const char* name);
public:
    void count(int value);
    void name(const char* inName);
    void fill(size_t size) {}
    void skip(size_t size) {}

    void add(const char* name, int8    value, const char* io);
    void add(const char* name, int16   value, const char* io);
    void add(const char* name, int32   value, const char* io);
    void add(const char* name, int64   value, const char* io);
    void add(const char* name, char* value, size_t size, const char* io);
    void add(const char* name, double  value, const char* io);
    void add(const char* name, void* value, size_t size, const char* io);
    void add(const char* name, tHugeCHAR& value, const char* io);

    void set(const char* name, int8& value, size_t size, const char* io);
    void set(const char* name, int16& value, size_t size, const char* io);
    void set(const char* name, int32& value, size_t size, const char* io);
    void set(const char* name, int64& value, size_t size, const char* io);
    void set(const char* name, char* value, size_t size, const char* io);
    void set(const char* name, double& value, size_t size, const char* io);
    void set(const char* name, void* value, size_t size, const char* io);
    void set(const char* name, tHugeCHAR& value, size_t size, const char* io);

    void fromXML(TBAmp& xml);
    void fromXML(const char* xml)
    {
        TBAmp work; 
        work.set(xml);
        fromXML(work);
    }
    void toXML(TBAmp& xml);
};

enum eErrorXMLDataBuild
{
    errxmlDBERROK,
    errxmlDBAValueError
};


class xXMLDataBuild : public xCept
{
public:
    xXMLDataBuild(char* aFname, int aLine, int aError, char* aMsg = 0);
    xXMLDataBuild(const xXMLDataBuild& aX)
        : xCept(aX) {};
};

#define XXMLDataBuild(err) \
        xXMLDataBuild(__FILE__, __LINE__, (int)err)
#define XXMLDataBuild_Message(err, msg) \
        xXMLDataBuild(__FILE__, __LINE__, (int)err, msg)

#endif
