#ifndef _DATABUILD2_H_
#define _DATABUILD2_H_
#include "machine.h"
#include "hugechar.h"

// ---------------------------------------------------------
// This is the base class for dbportal generated .sh files
// It has count and shows input outputs flag
// PLEASE DO NOT ADD APPLICATION FUNCTION AND VARIABLES HERE
// T H A T   M E A N S   Y  O  U
// ---------------------------------------------------------

class DataBuilder
{
public:
  virtual void count(int value) = 0;
  virtual void name(const char *inName) = 0;

  virtual void fill(size_t size) {}
  virtual void add(const char *name, int8    value, const char *io) = 0;
  virtual void add(const char *name, int16   value, const char *io) = 0;
  virtual void add(const char *name, int32   value, const char *io) = 0;
  virtual void add(const char *name, int64   value, const char *io) = 0;
  virtual void add(const char *name, char   *value, size_t size, const char *io) = 0;
  virtual void add(const char *name, double  value, const char *io) = 0;
  virtual void add(const char *name, void   *value, size_t size, const char *io) = 0;
  virtual void add(const char *name, tHugeCHAR &value, const char *io) = 0;

  virtual void skip(size_t size) {}
  virtual void set(const char *name, int8   &value, size_t size, const char *io) = 0;
  virtual void set(const char *name, int16  &value, size_t size, const char *io) = 0;
  virtual void set(const char *name, int32  &value, size_t size, const char *io) = 0;
  virtual void set(const char *name, int64  &value, size_t size, const char *io) = 0;
  virtual void set(const char *name, char   *value, size_t size, const char *io) = 0;
  virtual void set(const char *name, double &value, size_t size, const char *io) = 0;
  virtual void set(const char *name, void   *value, size_t size, const char *io) = 0;
  virtual void set(const char *name, tHugeCHAR &value, size_t size, const char *io) = 0;
};

#endif
