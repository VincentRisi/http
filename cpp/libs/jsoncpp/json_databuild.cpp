// This uses a modified version of jsoncpp in order to work on AIX 13.1.3
// Copyright 2011 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#include "json_databuild.h"

DataBuilderJson::DataBuilderJson()
{
    _count = 0;
    memset(sn_buff, 0, sizeof(sn_buff));
}

DataBuilderJson::~DataBuilderJson()
{
}

void DataBuilderJson::setRecord(const char* data)
{
    clear();
    stringstream sstr(data);
    sstr >> record;
}

void DataBuilderJson::setValue(Value inValue)
{
    clear();
    record = inValue;
}

Value DataBuilderJson::getRecord(string &data)
{
    StreamWriterBuilder swb;
    data = writeString(swb, record);
    return record;
}

void DataBuilderJson::count(int value)
{
    _count = value;
}

void DataBuilderJson::name(const char *value)
{
    _name = value;
}

