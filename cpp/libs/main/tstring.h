#ifndef _TSTRING_H_
#define _TSTRING_H_

#include <iostream>

#include "xstring.h"
#include "xcept.h"

//-------------------------------------------------------------
//  xString class
//-------------------------------------------------------------

class XString : public XCept
{
public:
  enum eError { errOverflow = 1 };
  XString(char * aFname, int aLine, eError aError);
  XString(const XString& x);

private:
  // following function not defined - error if used
  XString& operator=(const XString&);
};

inline XString::XString(const XString& x) : XCept(x) {}

#define XSTRING(err) XString(__FILE__, __LINE__, XString::err)

//-------------------------------------------------------------
//  tString class
//-------------------------------------------------------------

class TString
{
public:
  enum CompMode { cmSensitive, cmIgnore };

  TString();                               // default constructor
  TString(const TString& str);             // copy constructor
  TString(const char * cstr);               // cstring constructor
  TString(const char * anstr, size_t asize);// fixed size constructor
  TString(char c);                         // single-char constructor
  TString(size_t count, char fill = ' ');  // multiple-char constructor
  ~TString();                              // destructor

  void SetCompMode(CompMode aCompMode);    // change compare mode
  size_t Length() const;                   // string length
  operator const char *() const;           // conversion to const char * op.
  operator char *();                       // conversion to const char * op.
  TString& operator=(const TString& str);  // assignment operator
  TString& operator=(const char * cstr);   // assignment operator
  const char* c_str();
  char operator[](size_t pos) const;       // operator[]
  void ToUpper();                          // convert self to upper case
  void ToLower();                          // convert self to lower case
  TString AsUpper() const;                 // return copy as upper case
  TString AsLower() const;                 // return copy as lower case

  // concatenation operators
  friend TString operator+(const TString& str1, const TString& str2);
  void operator+=(const TString& str);
  friend TString operator+(const TString& str1, char ch);
  void operator+=(char ch);

  // comparison methods
  int operator< (const TString& str) const;
  int operator> (const TString& str) const;
  int operator<=(const TString& str) const;
  int operator>=(const TString& str) const;
  int operator==(const TString& str) const;
  int operator!=(const TString& str) const;

  // stream output
  friend std::ostream& operator<<(std::ostream& strm, const TString& str);

private:
  enum { allocIncr = 8, maxStrlen = 131072};
  static size_t CalcSize(size_t needed);  // calculate allocation size
  typedef unsigned long ulong;

  CompMode compMode;
  size_t siz; // allocated size
  size_t len; // current length
  char * p;    // pointer to text
  int Compare(const TString& str) const;

  // special constructors to speed operator+ ops
  TString(const TString& str1, const TString& str2);
  TString(const TString& str1, const char ch);
};

// calculate the allocation size for a string
inline size_t TString::CalcSize(size_t needed)
{
  return ((needed + allocIncr) / allocIncr) * allocIncr;
}

inline void TString::SetCompMode(CompMode aCompMode)
{
  compMode = aCompMode;
}

inline size_t TString::Length() const
{
  return len;
}

inline TString::operator const char *() const
{
  return p;
}

inline TString::operator char *()
{
  return p;
}

inline char TString::operator[](size_t pos) const
{
  return (pos >= len) ? char(0) : p[pos];
}

inline void TString::ToUpper()
{
  if (p != 0)
    strupr(p);
}

inline void TString::ToLower()
{
  if (p != 0)
    strlwr(p);
}

inline const char* TString::c_str()
{
  return p;
}

inline TString operator+(const TString& str1, const TString& str2)
{
  return TString(str1, str2);
}

inline void TString::operator+=(const TString& str)
{
  *this = *this + str;
}

inline TString operator+(const TString& str, char ch)
{
  return TString(str, ch);
}

inline void TString::operator+=(char ch)
{
  *this = *this + ch;
}

// comparison methods ----------------------------------
inline int TString::operator<(const TString& str) const
{
  return (Compare(str) < 0);
}

inline int TString::operator>(const TString& str) const
{
  return (Compare(str) > 0);
}

inline int TString::operator<=(const TString& str) const
{
  return (Compare(str) <= 0);
}

inline int TString::operator>=(const TString& str) const
{
  return (Compare(str) >= 0);
}

inline int TString::operator==(const TString& str) const
{
  return (Compare(str) == 0);
}

inline int TString::operator!=(const TString& str) const
{
  return (Compare(str) != 0);
}


inline std::ostream& operator<<(std::ostream& os, const TString& str)
{
  os << str.p;
  return os;
}

#endif

