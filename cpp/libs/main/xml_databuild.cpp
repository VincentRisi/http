#include "xml_databuild.h"

void DataBuilderXML::fromXML(TBAmp& xml)
{
    record.Clear();
    record.Load(xml);
}

void DataBuilderXML::toXML(TBAmp& xml)
{
    record.AsXML(xml);
}

void DataBuilderXML::count(int value)
{
    _count = value;
}

void DataBuilderXML::name(const char *value)
{
    _name = value;
}

char* DataBuilderXML::key(const char* name)
{
    snprintf(_key, sizeof(_key), "%s.%s", _name.c_str(), name);
    return _key;
}

void DataBuilderXML::add(const char *name, int8 value, const char *io)
{
    snprintf(_value, sizeof(_value), "%d", value);
    record.InsertTag(key(name), _value);
}

void DataBuilderXML::add(const char *name, int16 value, const char *io)
{
    snprintf(_value, sizeof(_value), "%d", value);
    record.InsertTag(key(name), _value);
}

void DataBuilderXML::add(const char *name, int32 value, const char *io)
{
    snprintf(_value, sizeof(_value), "%d", value);
    record.InsertTag(key(name), _value);
}

void DataBuilderXML::add(const char *name, int64 value, const char *io)
{
#if defined(M_W32)
    snprintf(_value, sizeof(_value), "%lld", value);
#else
    snprintf(_value, sizeof(_value), "%ld", value);
#endif
    record.InsertTag(key(name), _value);
}

void DataBuilderXML::add(const char *name, char *value, size_t size, const char *io)
{
    record.InsertTag(key(name), value);
}

void DataBuilderXML::add(const char *name, double value, const char *io)
{
    snprintf(_value, sizeof(_value), "%0.15g", value);
}

void DataBuilderXML::add(const char *name, void *value, size_t size, const char *io)
{
    int no;
    const char* bp = (char*)value;
    for (no = size; no > 0; no--)
        if (bp[no - 1] != 0x00)
            break;
    if (no > 4)
    { 
        string x = base64_encode((unsigned char const*)value + 4, (unsigned int)no - 4);
        record.InsertTag(key(name), (char*)x.c_str());
    }
    else
    { 
        record.InsertTag(key(name), "");
    }
}

void DataBuilderXML::add(const char *name, tHugeCHAR &value, const char *io)
{
    record.InsertTag(key(name), value.data);
}

void DataBuilderXML::set(const char *name, int8   &value, size_t size, const char *io)
{
    record.GetValue(key(name), work);
    int64 number = atoint64(work.data);
    if (number >= -127 && number < 128)
      value = (int8)number;
    else
    {
#if defined(M_W32)
        snprintf(_value, sizeof(+value), "Value %s exceeds range for int8 %lld.", name, number);
#else
        snprintf(_value, sizeof(+value), "Value %s exceeds range for int8 %ld.", name, number);
#endif
        throw XXMLDataBuild_Message(errxmlDBAValueError, _value);
    }
}

void DataBuilderXML::set(const char *name, int16  &value, size_t size, const char *io)
{
    record.GetValue(key(name), work);
    int64 number = atoint64(work.data);
    if (number >= -32767 && number < 32768)
        value = (int16)number;
    else
    {
#if defined(M_W32)
        snprintf(_value, sizeof(+value), "Value %s exceeds range for int16 %lld.", name, number);
#else
        snprintf(_value, sizeof(+value), "Value %s exceeds range for int16 %ld.", name, number);
#endif
        throw XXMLDataBuild_Message(errxmlDBAValueError, _value);
    }
}

void DataBuilderXML::set(const char *name, int32  &value, size_t size, const char *io)
{
    record.GetValue(key(name), work);
    int64 number = atoint64(work.data);
    if (number >= -2147483647 && number < 2147483648)
        value = (int32)number;
    else
    {
#if defined(M_W32)
        snprintf(_value, sizeof(+value), "Value %s exceeds range for int32 %lld.", name, number);
#else
        snprintf(_value, sizeof(+value), "Value %s exceeds range for int32 %ld.", name, number);
#endif
        throw XXMLDataBuild_Message(errxmlDBAValueError, _value);
    }
}

void DataBuilderXML::set(const char *name, int64  &value, size_t size, const char *io)
{
    record.GetValue(key(name), work);
    int64 number = atoint64(work.data);
    value = number;
}

void DataBuilderXML::set(const char *name, char *value, size_t size, const char *io)
{
    record.GetValue(key(name), work);
    string str = work.data;
    size_t from_size = str.length();
    if (from_size < size)
        strncpyz(value, str.c_str(), from_size);
    else
    {
        snprintf(_value, sizeof(_value), "Value %s sizeof string %d exceeds allowed size %d.", name, from_size, size);
        throw XXMLDataBuild_Message(errxmlDBAValueError, _value);
    }
}

void DataBuilderXML::set(const char *name, double &value, size_t size, const char *io)
{
    record.GetValue(key(name), work);
    double number = atof(work.data);
    value = number;
}

void DataBuilderXML::set(const char *name, void *value, size_t size, const char *io)
{
    record.GetValue(key(name), work);
    string from4 = work.data;
    string as3 = base64_decode(from4);
    size_t as3_size = as3.length();
    if (as3_size + 4 <= size)
    {
        memset(value, 0, size);
        *(int*)value = as3_size;
        memcpy((char*)value+4, (void *)as3.c_str(), as3_size);
    }
    else
    {
        snprintf(_value, sizeof(_value), "Value %s sizeof string %d exceeds allowed size %d.", name, as3_size, size);
        throw XXMLDataBuild_Message(errxmlDBAValueError, _value);
    }
}

void DataBuilderXML::set(const char *name, tHugeCHAR &value, size_t size, const char *io)
{
    record.GetValue(key(name), work);
    size_t from_size = strlen(work.data);
    if (from_size < size)
    {
        if (value.data != 0)
           delete[] value.data;
        value.data = new char[from_size + 1];
        strncpyz(value.data, work.data, from_size);
        value.length = from_size;
    }
    else
    {
        snprintf(_value, sizeof(_value), "Value %s sizeof string %d exceeds allowed size %d.", name, from_size, size);
        throw XXMLDataBuild_Message(errxmlDBAValueError, _value);
    }
}

static char* EXMLDataBuildError[] =
{ "OK"
, "Value Error"
};

xXMLDataBuild::xXMLDataBuild(char* aFname, int aLine, int aError, char* aMsg)
    : xCept(aFname, aLine, "XMLMake", aError)
{
    osErr << aFname << "(" << aLine << ")";
    if (aError)
        osErr << "Error(" << EXMLDataBuildError[aError] << ")";
    if (aMsg)
        osErr << aMsg << std::endl;
    osErr << ends;
}
