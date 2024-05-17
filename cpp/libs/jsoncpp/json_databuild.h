// This uses a modified version of jsoncpp in order to work on AIX 13.1.3
// Copyright 2011 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef _JSON_DATABUILD_H_
#define _JSON_DATABUILD_H_
#if !defined _DATABUILD2_H_ && !defined _DATABUILDJP_H_
#  include "databuilderjp.h"
#endif
#include "json/json.h"

using namespace std;
using namespace Json;

class DataBuilderJson : public DataBuilder
{
  int _count;
  string _name;
  Value record;
  char sn_buff[512];
public:
  DataBuilderJson();
  void setRecord(const char* data);
  void setValue(Value inValue);
  Value getRecord(string &data);
  void clear() { record.clear(); }
  virtual ~DataBuilderJson();
  void count(int value);
  void name(const char *inName);
  void fill(size_t size) {}
  void skip(size_t size) {}

#if defined _DATABUILD2_H_
  void add(const char *name, int8    value, const char *io);
  void add(const char *name, int16   value, const char *io);
  void add(const char *name, int32   value, const char *io);
  void add(const char *name, int64   value, const char *io);
  void add(const char *name, char   *value, size_t size, const char *io);
  void add(const char *name, double  value, const char *io);
  void add(const char *name, void   *value, size_t size, const char *io);
  void add(const char *name, tHugeCHAR &value, const char *io);

  void set(const char *name, int8   &value, size_t size, const char *io);
  void set(const char *name, int16  &value, size_t size, const char *io);
  void set(const char *name, int32  &value, size_t size, const char *io);
  void set(const char *name, int64  &value, size_t size, const char *io);
  void set(const char *name, char   *value, size_t size, const char *io);
  void set(const char *name, double &value, size_t size, const char *io);
  void set(const char *name, void   *value, size_t size, const char *io);
  void set(const char *name, tHugeCHAR &value, size_t size, const char *io);
#elif defined _DATABUILDJP_H_
  void add(const char* name, const int8    value);
  void addByte(const char* name, const int8 value);
  void addByte(const char* name, const int8* value, const size_t size);
  void add(const char* name, const int16   value);
  void add(const char* name, const int32   value);
  void add(const char* name, const int64   value);
  void add(const char* name, const char* value);
  void add(const char* name, const size_t len, const unsigned char* value);
  void add(const char* name, const double  value);

  void add(const char* name, const int8    value, const int16 isNull);
  void addByte(const char* name, const int8 value, const int16 isNull);
  void addByte(const char* name, const int8* value, const size_t size, const int16 isNull);
  void add(const char* name, const int16 value, const int16 isNull);
  void add(const char* name, const int32 value, const int16 isNull);
  void add(const char* name, const int64   value, const int16 isNull);
  void add(const char* name, const char* value, const int16 isNull);
  void add(const char* name, const size_t len, const unsigned char* value, const int16 isNull);
  void add(const char* name, const double  value, const int16 isNull);

  void set(const char* name, int8& value, const size_t size);
  void setByte(const char* name, int8& value, const size_t size);
  void setByte(const char* name, int8* value, const size_t size);
  void set(const char* name, int16& value, const size_t size);
  void set(const char* name, int32& value, const size_t size);
  void set(const char* name, int64& value, const size_t size);
  void set(const char* name, char* value, const size_t size);
  void set(const char* name, int& len, unsigned char* value, const size_t size);
  void set(const char* name, double& value, const size_t size);

  void set(const char* name, int8& value, const size_t size, int16& null);
  void setByte(const char* name, int8& value, const size_t size, int16& null);
  void setByte(const char* name, int8* value, const size_t size, int16& null);
  void set(const char* name, int16& value, const size_t size, int16& null);
  void set(const char* name, int32& value, const size_t size, int16& null);
  void set(const char* name, int64& value, const size_t size, int16& null);
  void set(const char* name, char* value, const size_t size, int16& null);
  void set(const char* name, int& len, unsigned char* value, const size_t size, int16& null);
  void set(const char* name, double& value, const size_t size, int16& null);

#endif
};

#endif
