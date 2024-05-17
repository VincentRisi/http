/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2007/02/01
 Name   : bstring
 Descr  : Yet a another string class, this one is based on the python 
          string object
==============================================================================*/

#include "bstring.h"

#if defined(_WIN32) || defined(_WIN64)
   #define _DIR_SEPERATOR '\\'
#else
   #define _DIR_SEPERATOR '/'
#endif


#include "xstring.h"
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>


/*-------------------------------------
   Hash defs
-------------------------------------*/
#define MAX_INT_STR_SIZE         64
#define BSTRING_NULL_STR         ""
#define BSTRING_TRUE             "true"
#define BSTRING_FALSE            "false"
#define BSTRING_YES              "yes"
#define BSTRING_NO               "no"
#define WHITE_SPACES             " \n\r\t\f"


/*------------------------------------------------------------------------------
   static methods
------------------------------------------------------------------------------*/
static bool validUnsigned(const char *str)
{
   char *ptr = (char *) str;

   for (; *ptr; ptr++)
      if (*ptr < '0' || *ptr > '9')
         return false;

   return true;
}


static bool validInt(const char *str)
{
   char *ptr = (char *) str;

   if (*ptr == '-')
      ptr++;

   if (*ptr == 0)
      return false;

   for (; *ptr; ptr++)
      if (*ptr < '0' || *ptr > '9')
         return false;

   return true;
}


static bool validDouble(const char *str)
{
   char *ptr       = (char *) str;
   char  dot_found = 0;

   if (*ptr == '-')
      ptr++;

   if (*ptr == 0)
      return false;

   for (; *ptr; ptr++)
      if (*ptr < '0' || *ptr > '9')
      {
         if (*ptr == '.' && !dot_found)
            dot_found = 1;
         else
            return false;
      }

   return true;
}


/*------------------------------------------------------------------------------
   xBstring Constructor
------------------------------------------------------------------------------*/
XBstring::XBstring(const char *file, const int line, const char *fmt, ...)
         :XCept(file, line, "xBstring", -1)
{
   va_list ap;
   char    jotter[1024];

   va_start(ap, fmt);

   vsnprintf(jotter, sizeof(jotter) - 1, fmt, ap);

   va_end(ap);

   //strncpy(errorStr, jotter, sizeof(errorStr));

   //AssignError(jotter);
   osErr << jotter << std::endl << std::ends;
}


/*----------------------------------------------------------------------------
   constructors
----------------------------------------------------------------------------*/
bstring::bstring() : _str(0), _strLen(0)
{
   newStr(BSTRING_NULL_STR);
}


bstring::bstring(const char *inStr) : _str(0), _strLen(0)
{
   newStr(inStr);
}


bstring::bstring(const bstring &inStr) : _str(0), _strLen(0)
{
   newStr(inStr.c_str());
}


bstring::bstring(const int32 strVal) : _str(0), _strLen(0)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%d", strVal);
   newStr(jotter);
}


bstring::bstring(const int64 strVal) : _str(0), _strLen(0)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%ld", strVal);
   newStr(jotter);
}


bstring::bstring(const unsigned strVal) : _str(0), _strLen(0)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%u", strVal);
   newStr(jotter);
}


bstring::bstring(const double strVal) : _str(0), _strLen(0)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%lf", strVal);
   newStr(jotter);
}


bstring::bstring(const char ch) : _str(0), _strLen(0)
{
   char inStr[2];

   inStr[0] = ch;
   inStr[1] = 0;

   newStr(inStr);
}


bstring::bstring(const bool strVal) : _str(0), _strLen(0)
{
   if (strVal)
      newStr(BSTRING_TRUE);
   else
      newStr(BSTRING_FALSE);
}


bstring::bstring(const char *str1, const char *str2) : _str(0), _strLen(0)
{
   if (str1)
      _strLen += strlen(str1);

   if (str2)
      _strLen += strlen(str2);

   if (!_strLen)
   {
      newStr(BSTRING_NULL_STR);
      return;
   }

   char *tmp;

   if ((tmp = (char *) realloc(_str, _strLen + 1)) == 0)
   {
      throw XBstring(__FILE__, __LINE__, "bstring::bstring - Error reallocating string - size '%u'", _strLen);
   }

   _str = tmp;

   if (str1)
   {
      strcpy(_str, str1);

      if (str2)
         strcat(_str, str2);
   }
   else
      strcpy(_str, str2);
}


bstring::bstring(const char *str1, const unsigned len) : _str(0), _strLen(0)
{
   if (!str1 || !len)
   {
      newStr(BSTRING_NULL_STR);
      return;
   }

   _strLen = len;

   char *tmp;

   if ((tmp = (char *) realloc(_str, _strLen + 1)) == 0)
   {
      throw XBstring(__FILE__, __LINE__, "bstring::bstring - Error reallocating string - size '%u'", _strLen);
   }

   _str = tmp;

   memcpy(_str, str1, len);
   _str[len] = 0;
}


/*----------------------------------------------------------------------------
   bstring destructor
----------------------------------------------------------------------------*/
bstring::~bstring()
{
   freeStr();
}


/*----------------------------------------------------------------------------
   free's all memory associated to class
----------------------------------------------------------------------------*/
void bstring::freeStr()
{
   if (!_str)
      return;

   free(_str);

   _str     = 0;
   _strLen = 0;
}


/*----------------------------------------------------------------------------
   creates a new string
----------------------------------------------------------------------------*/
bstring *bstring::newStr(const char *str)

{
   if (!str)
      return newStr(BSTRING_NULL_STR);

   char *tmp;

   _strLen  = strlen(str);

   if ((tmp = (char *) realloc(_str, _strLen + 1)) == 0)
   {
      throw XBstring(__FILE__, __LINE__, "bstring::newStr - Error reallocating string - size '%u'", _strLen);
   }

   _str = tmp;
   strcpy(_str, str);
   return this;
}


/*----------------------------------------------------------------------------
   appends this extra piece of string 
----------------------------------------------------------------------------*/
bstring *bstring::appendStr(const char *str)
{
   if (!str || !str[0])
      return this;

   char *tmp;

   _strLen += strlen(str);

   if ((tmp = (char *) realloc(_str, _strLen + 1)) == 0)
   {
      throw XBstring(__FILE__, __LINE__, "bstring::appendStr - Error reallocating string - size '%u'", _strLen);
   }

   _str = tmp;
   strcat(_str, str);
   return this;
}


/*----------------------------------------------------------------------------
   = operators
----------------------------------------------------------------------------*/
bstring &bstring::operator = (const char ch)
{
   char inStr[2];

   inStr[0] = ch;
   inStr[1] = 0;

   return *newStr(inStr);
}


bstring &bstring::operator = (const char *inStr)
{
   return *newStr(inStr);;
}
 

bstring &bstring::operator = (const bstring &inStr)
{
   return *newStr(inStr._str);
}


bstring &bstring::operator = (const int strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%d", strVal);
   return *newStr(jotter);
}


bstring &bstring::operator = (const unsigned strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%u", strVal);
   return *newStr(jotter);
}


bstring &bstring::operator = (const double strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%lf", strVal);
   return *newStr(jotter);
}


bstring &bstring::operator = (const bool strVal)
{
   if (strVal)
      return *newStr(BSTRING_TRUE);
   
   return *newStr(BSTRING_FALSE);
}


/*----------------------------------------------------------------------------
   += operators
----------------------------------------------------------------------------*/
bstring &bstring::operator += (const char ch)
{
   char inStr[2];

   inStr[0] = ch;
   inStr[1] = 0;

   return *appendStr(inStr);
}


bstring &bstring::operator += (const char *inStr)
{
   return *appendStr(inStr);
}


bstring &bstring::operator += (const bstring &inStr)
{
   return *appendStr(inStr._str);
}


bstring &bstring::operator += (const int strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%d", strVal);
   return *appendStr(jotter);
}


bstring &bstring::operator += (const unsigned strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%u", strVal);
   return *appendStr(jotter);
}


bstring &bstring::operator += (const double strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%lf", strVal);
   return *appendStr(jotter);
}


bstring &bstring::operator += (const bool strVal)
{
   if (strVal)
      return *appendStr(BSTRING_TRUE);
   
   return *appendStr(BSTRING_FALSE);
}


void bstring::shrink(const unsigned val)
{
   if (val >= _strLen)
   {
      newStr(BSTRING_NULL_STR);
      return;
   }

   bstring tmp = *this;

   tmp._str[_strLen - (val + 1)] = 0;

   newStr(tmp._str);
}


/*----------------------------------------------------------------------------
   friend + operators
----------------------------------------------------------------------------*/
bstring operator + (const bstring &str, const char ch)
{
   char inStr[2];

   inStr[0] = ch;
   inStr[1] = 0;

   return bstring(str._str, inStr);
}


bstring operator + (const bstring &str, const char *inStr)
{
   return bstring(str._str, inStr);
}


bstring operator + (const bstring &str, const bstring &inStr)
{
   return bstring(str._str, inStr._str);
}


bstring operator + (const bstring &str, const int strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%d", strVal);
   return bstring(str._str, jotter);
}


bstring operator + (const bstring &str, const unsigned strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%u", strVal);
   return bstring(str._str, jotter);
}


bstring operator + (const bstring &str, const double strVal)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%lf", strVal);
   return bstring(str._str, jotter);
}


bstring operator + (const char ch, const bstring &str)
{
   char inStr[2];

   inStr[0] = ch;
   inStr[1] = 0;

   return bstring(inStr, str._str);
}


bstring operator + (const char *inStr, const bstring &str)
{
   return bstring(inStr, str._str);
}


bstring operator + (const int strVal, const bstring &str)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%d", strVal);
   return bstring(jotter, str._str);
}


bstring operator + (const unsigned strVal, const bstring &str)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%u", strVal);
   return bstring(jotter, str._str);
}


bstring operator + (const double strVal, const bstring &str)
{
   char jotter[MAX_INT_STR_SIZE];

   sprintf(jotter, "%lf", strVal);
   return bstring(jotter, str._str);
}


bstring operator + (const bool strVal, const bstring &str)
{
   if (strVal)
      return bstring(BSTRING_TRUE, str._str);

   return bstring(BSTRING_FALSE, str._str);
}


bstring operator + (const bstring &str, const bool strVal)
{
   if (strVal)
      return bstring(BSTRING_TRUE, str._str);

   return bstring(BSTRING_FALSE, str._str);
}


/*------------------------------------------------------------------------------
   format
------------------------------------------------------------------------------*/
bstring &bstring::format(const char *fmt, ...)
{
   va_list ap;
   char    jotter[4096];

   va_start(ap, fmt);

   vsnprintf(jotter, sizeof(jotter)-1, fmt, ap);

   va_end(ap);

   newStr(jotter);

   return *this;
}


/*----------------------------------------------------------------------------
   check functions
----------------------------------------------------------------------------*/
bool bstring::isDouble() const
{
   return validDouble(_str);
}


bool bstring::isInt() const
{
   return validInt(_str);
}


bool bstring::isLong() const
{
   return validInt(_str);
}


bool bstring::isUnsigned() const
{
   return validUnsigned(_str);
}


bool bstring::isBool() const
{
   if (isTrue(_str) ||
       isFalse(_str))
      return true;

   return false;
}


bool bstring::isAlpha() const
{
   char *ptr;

   for (ptr = _str; *ptr; ptr++)
      if (!((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z')))
         return false;

   return true;
}


bool bstring::isUpper() const
{
   char *ptr;

   for (ptr = _str; *ptr; ptr++)
      if (*ptr >= 'a' && *ptr <= 'z')
         return false;

   return true;
}


bool bstring::isLower() const
{
   char *ptr;

   for (ptr = _str; *ptr; ptr++)
      if (*ptr >= 'A' && *ptr <= 'Z')
         return false;

   return true;
}


bstring bstring::splice(const int pos1, const int pos2) const
{
   char *start_pos;
   char *end_pos;

   if (pos1 == 0 && pos2 == 0)
      return bstring("");

   if (pos1 > 0)
   {
      if (pos1 > (int) _strLen)
         throw XBstring(__FILE__, __LINE__, "bstring::splice - pos1 index out of range (%d/%u)", pos1, _strLen);

      start_pos = _str + pos1;
   }
   else if (pos1 < 0)
   {
      if ((0 - pos1) > (int) _strLen)
         throw XBstring(__FILE__, __LINE__, "bstring::splice - pos1 index out of range (%d/%u)", pos1, _strLen);

      start_pos  = _str + _strLen;
      start_pos += pos1;
   }
   else
      start_pos = _str;

   if (pos2 > 0)
   {
      if (pos2 > (int) _strLen)
         throw XBstring(__FILE__, __LINE__, "bstring::splice - pos2 index out of range (%d/%u)", pos2, _strLen);

      end_pos = _str + pos2;
   }
   else if (pos2 < 0)
   {
      if ((0 - pos2) > (int) _strLen)
         throw XBstring(__FILE__, __LINE__, "bstring::splice - pos2 index out of range (%d/%u)", pos2, _strLen);

      end_pos  = _str + _strLen;
      end_pos += pos2;
   }
   else
      end_pos = _str + _strLen;

   if (end_pos <= start_pos)
      return bstring("");

   return bstring(start_pos, end_pos - start_pos);
}


/*----------------------------------------------------------------------------
   returns the directory seperator
----------------------------------------------------------------------------*/
char bstring::dirSep()
{
   return _DIR_SEPERATOR;
}


/*----------------------------------------------------------------------------
   conversion functions
----------------------------------------------------------------------------*/
int bstring::toInt() const
{
   if (isInt())
      return atoi(_str);

   return 0;
}


long bstring::toLong() const
{
   if (isInt())
      return atol(_str);

   return 0L;
}


unsigned bstring::toUnsigned() const
{
   if (isUnsigned())
      return (unsigned) atoi(_str);

   return 0;
}


double bstring::toDouble() const
{
   if (isDouble())
      return (double) atof(_str);

   return 0.0;
}


bool bstring::toBool() const
{
   if (isBool())
   {
      if (isTrue(_str))
         return true;

      return false;
   }

   return false;
}


/*----------------------------------------------------------------------------
   comparison functions - bstring
----------------------------------------------------------------------------*/
bool operator < (const bstring &str1, const bstring &str2)
{
   return strcmp(str1._str, str2._str) < 0 ? true : false;
}


bool operator > (const bstring &str1, const bstring &str2)
{
   return strcmp(str1._str, str2._str) > 0 ? true : false;
}


bool operator <= (const bstring &str1, const bstring &str2)
{
   return strcmp(str1._str, str2._str) <= 0 ? true : false;
}


bool operator >= (const bstring &str1, const bstring &str2)
{
   return strcmp(str1._str, str2._str) >= 0 ? true : false;
}


bool operator == (const bstring &str1, const bstring &str2)
{
   return strcmp(str1._str, str2._str) == 0 ? true : false;
}


bool operator != (const bstring &str1, const bstring &str2) 
{
   return strcmp(str1._str, str2._str) != 0 ? true : false;
}


/*----------------------------------------------------------------------------
   comparison functions - char *
----------------------------------------------------------------------------*/
bool operator < (const bstring &str1, const char *inStr)
{
   return strcmp(str1._str, inStr) < 0 ? true : false;
}


bool operator > (const bstring &str1, const char *inStr)
{
   return strcmp(str1._str, inStr) > 0 ? true : false;
}


bool operator <= (const bstring &str1, const char *inStr)
{
   return strcmp(str1._str, inStr) <= 0 ? true : false;
}


bool operator >= (const bstring &str1, const char *inStr)
{
   return strcmp(str1._str, inStr) >= 0 ? true : false;
}


bool operator == (const bstring &str1, const char *inStr)
{
   return strcmp(str1._str, inStr) == 0 ? true : false;
}


bool operator != (const bstring &str1, const char *inStr)
{
   return strcmp(str1._str, inStr) != 0 ? true : false;
}


/*----------------------------------------------------------------------------
   comparison functions - char
----------------------------------------------------------------------------*/
bool operator < (const bstring &str1, const char ch)
{
   return str1._str[0] < ch ? true : false;
}


bool operator > (const bstring &str1, const char ch)
{
   return str1._str[0] > ch ? true : false;
}


bool operator <= (const bstring &str1, const char ch)
{
   return str1._str[0] <= ch ? true : false;
}


bool operator >= (const bstring &str1, const char ch)
{
   return str1._str[0] >= ch ? true : false;
}


bool operator == (const bstring &str1, const char ch)
{
   return str1._str[0] == ch ? true : false;
}


bool operator != (const bstring &str1, const char ch)
{
   return str1._str[0] != ch ? true : false;
}


/*----------------------------------------------------------------------------
   comparison functions - int
----------------------------------------------------------------------------*/
bool operator < (const bstring &str1, const int strVal)
{
   return str1.toInt() < strVal ? true : false;
}


bool operator > (const bstring &str1, const int strVal)
{
   return str1.toInt() > strVal ? true : false;
}


bool operator <= (const bstring &str1, const int strVal)
{
   return str1.toInt() <= strVal ? true : false;
}


bool operator >= (const bstring &str1, const int strVal)
{
   return str1.toInt() >= strVal ? true : false;
}


bool operator == (const bstring &str1, const int strVal)
{
   return str1.toInt() == strVal ? true : false;
}


bool operator != (const bstring &str1, const int strVal)
{
   return str1.toInt() != strVal ? true : false;
}


/*----------------------------------------------------------------------------
   comparison functions - unsigned
----------------------------------------------------------------------------*/
bool operator < (const bstring &str1, const unsigned strVal)
{
   return str1.toUnsigned() < strVal ? true : false;
}


bool operator > (const bstring &str1, const unsigned strVal)
{
   return str1.toUnsigned() > strVal ? true : false;
}


bool operator <= (const bstring &str1, const unsigned strVal)
{
   return str1.toUnsigned() <= strVal ? true : false;
}


bool operator >= (const bstring &str1, const unsigned strVal)
{
   return str1.toUnsigned() >= strVal ? true : false;
}


bool operator == (const bstring &str1, const unsigned strVal)
{
   return str1.toUnsigned() == strVal ? true : false;
}


bool operator != (const bstring &str1, const unsigned strVal)
{
   return str1.toUnsigned() != strVal ? true : false;
}


/*----------------------------------------------------------------------------
   comparison functions - double
----------------------------------------------------------------------------*/
bool operator < (const bstring &str1, const double strVal)
{
   return str1.toDouble() < strVal ? true : false;
}


bool operator > (const bstring &str1, const double strVal)
{
   return str1.toDouble() > strVal ? true : false;
}


bool operator <= (const bstring &str1, const double strVal)
{
   return str1.toDouble() <= strVal ? true : false;
}


bool operator >= (const bstring &str1, const double strVal)
{
   return str1.toDouble() >= strVal ? true : false;
}


bool operator == (const bstring &str1, const double strVal)
{
   return str1.toDouble() == strVal ? true : false;
}


bool operator != (const bstring &str1, const double strVal)
{
   return str1.toDouble() != strVal ? true : false;
}


/*----------------------------------------------------------------------------
   comparison functions bool
----------------------------------------------------------------------------*/
bool operator == (const bstring &str, const bool strVal)
{
   if (strVal)
      return bstring::isTrue(str._str);

   return bstring::isFalse(str._str);
}


bool operator != (const bstring &str, const bool strVal)
{
   if (strVal)
      return !bstring::isTrue(str._str);

   return !bstring::isFalse(str._str);
}

/*----------------------------------------------------------------------------
   static functions
----------------------------------------------------------------------------*/
bool bstring::isTrue(const char *str)
{
   if (!stricmp(str, "y")          ||
       !stricmp(str, "t")           ||
       !stricmp(str, BSTRING_TRUE) ||
       !stricmp(str, BSTRING_YES)  ||
       !stricmp(str, "1"))
      return true;

   return false;
}


bool bstring::isFalse(const char *str)
{
   if (!stricmp(str, "n")              ||
       !stricmp(str, "f")              ||
       !stricmp(str, BSTRING_FALSE)    ||
       !stricmp(str, BSTRING_NO)       ||
       !stricmp(str, "0"))
      return true;

   return false;
}


/*----------------------------------------------------------------------------
   utilities
----------------------------------------------------------------------------*/
bool bstring::isEmpty() const
{
   if (_str == 0 || *_str == 0)
      return true;

   return false;
}


bstring &bstring::lower()
{
   for (char *p = _str; *p; p++)
      *p = tolower(*p);

   return *this;
}


bstring &bstring::upper()
{
   for (char *p = _str; *p; p++)
      *p = toupper(*p);

   return *this;
}


bstring &bstring::makeDirectory()
{
   if (_strLen == 0)
   {
      char tmp[3];

      tmp[0] = '.';
      tmp[1] = _DIR_SEPERATOR;
      tmp[2] = 0;
      
      *this = tmp;
      return *this;
   }

   if (_str[_strLen - 1] != _DIR_SEPERATOR)
      *this += _DIR_SEPERATOR;

   return *this;
}


bstring bstring::fileExtension(const char ext_ch) const
{
   char *ptr;

   ptr = strrchr(_str, ext_ch);

   return bstring(ptr);
}


int bstring::find(const bstring &inStr, const unsigned offset) const
{
   char *tmp;
   char *pos;

   if (offset >= _strLen)
      return -1;

   pos = _str + offset;

   if ((tmp = strstr(pos, inStr)) == 0)
      return -1;

   return tmp - _str;
}


int bstring::rfind(const bstring &inStr, const unsigned offset) const
{
   char   *tmp = _str;
   size_t  len = strlen(inStr);

   if (offset >= _strLen || len < 1)
      return -1;

   if (offset == 0)
      tmp += _strLen;
   else
      tmp += offset;

   for (; tmp >= _str; tmp--)
      if (strncmp(tmp, inStr, len) == 0)
         return tmp - _str;

   return -1;
}


unsigned bstring::count(const bstring &inStr) const
{
   char     *tmp;
   char     *pos = _str;
   unsigned  cnt = 0;

   while ((tmp = strstr(pos, inStr)) != 0)
   {
      pos = tmp + 1;
      cnt++;
   }

   return cnt;
}


bstring &bstring::rstrip(const bstring &inStr)
{
   stripChars(false, true, (char *) inStr.c_str());
   return *this;
}


bstring &bstring::lstrip(const bstring &inStr)
{
   stripChars(true, false, (char *) inStr.c_str());
   return *this;
}


bstring &bstring::strip(const bstring &inStr)
{
   stripChars(true, true, (char *)  inStr.c_str());
   return *this;
}


bstring &bstring::replace(const bstring &oldStr, const bstring &newStr)
{
   replaceStr(oldStr, newStr);
   return *this;
}


bstring &bstring::reverse()
{
   char *p = _str;
   char *e = _str + _strLen;
   char  tmp;

   for (e--; p < e; p++, e--)
   {
      tmp = *p;
      *p  = *e;
      *e  = tmp;
   }
   
   return *this;
}


bstring &bstring::capitalize()
{
   _str[0] = toupper(_str[0]);
   return *this;
}


bstring &bstring::title()
{
   if (_str[0] == 0)
      return *this;

   char *p = _str;
   char *ch;
   bool  cap = false;

   *p = toupper(*p);

   for (p++; *p; p++)
   {
      for (ch = WHITE_SPACES; *ch; ch++)
         if (*p == *ch)
         {
            cap = true;
            break;
         }

      if (*ch == 0)
      {
         if (cap)
         {
            cap = false;
            *p = toupper(*p);
         }
      }
   }
   return *this;
}


bool bstring::endsWith(const bstring &inStr) const
{
   if (_strLen < inStr._strLen)
      return false;

   if (strcmp(_str + (_strLen - inStr._strLen), inStr._str) == 0)
      return true;

   return false;
}


bool bstring::startsWith(const bstring &inStr) const
{
   if (_strLen < inStr._strLen)
      return false;

   if (strncmp(_str, inStr._str, inStr._strLen) == 0)
      return true;

   return false;
}


int bstring::compare(const bstring &cmpStr, const bool ignoreCase) const
{
   if (ignoreCase)
      return stricmp(_str, cmpStr._str);

   return strcmp(_str, cmpStr._str);
}


bstring &bstring::rjust(const unsigned size, const char ch)
{
   if (size <= _strLen)
      return *this;

   bstring ret;

   ret.setLength(size, ch);

   memcpy(ret._str + (ret._strLen - _strLen), _str, _strLen);

   *this = ret;
   return *this;
}


bstring &bstring::ljust(const unsigned size, const char ch)
{
   if (size <= _strLen)
      return *this;

   bstring ret;

   ret.setLength(size, ch);

   memcpy(ret._str, _str, _strLen);

   *this = ret;
   return *this;
}


bstring &bstring::center(const unsigned size, const char ch)
{
   if (size <= _strLen)
      return *this;

   bstring ret;

   ret.setLength(size, ch);

   memcpy(ret._str + (unsigned) (((ret._strLen) * 0.5) - ((_strLen) * 0.5)), _str, _strLen);

   
   *this = ret;
   return *this;
}


unsigned bstring::split(const bstring &sep, bstrings &dest) const
{
   dest.Reset();

   if (*_str == 0)
      return 0;

   char *pos = _str;
   char *ptr = _str;

   for (; *ptr; ptr++)
      if (strncmp(ptr, sep._str, sep._strLen) == 0)
      {
         if (pos == ptr || strncmp(pos, sep._str, sep._strLen) == 0)
         {
            pos += sep._strLen;
            ptr = pos;
            dest.Append(new bstring(""));
            continue;
         }

         dest.Append(new bstring(pos, ptr - pos));
         ptr += sep._strLen;
         pos = ptr;
      }

   dest.Append(new bstring(pos));

   return dest.Count();
}


unsigned bstring::splitLines(bstrings &dest) const
{
   return split("\n\r", dest);
}


/*----------------------------------------------------------------------------
   replaces all occurences of 'dest' sub string with the 'source' sub string
----------------------------------------------------------------------------*/
void bstring::replaceStr(const bstring &dest, const bstring &source)
{
   char *to_ptr;

   if ((to_ptr = strstr(_str, dest._str)) == 0)
      return;

   char          *from_ptr;
   char          *mem;
   bstring       newStr;
   unsigned int  offset;

   from_ptr = _str;

   for (; to_ptr;)
   {
      if (to_ptr - from_ptr > 0)
      {
         offset          = newStr._strLen;
         newStr._strLen += to_ptr - from_ptr;
         
         if ((mem = (char *) realloc(newStr._str, newStr._strLen + 1)) == 0)
            throw XBstring(__FILE__, __LINE__, "bstring::replace - Error reallocating string - size '%u'", newStr._strLen + 1);

         newStr._str = mem;

         memcpy(newStr._str + offset, from_ptr, to_ptr - from_ptr);
      }

      offset          = newStr._strLen;
      newStr._strLen += source._strLen;

      if ((mem = (char *) realloc(newStr._str, newStr._strLen + 1)) == 0)
         throw XBstring(__FILE__, __LINE__, "bstring::replace - Error reallocating string - size '%u'", newStr._strLen + 1);

      newStr._str = mem;
      memcpy(newStr._str + offset, source._str, source._strLen);

      from_ptr  = to_ptr + dest._strLen;
      to_ptr    = strstr(from_ptr, dest._str);
   }

   if (*from_ptr)
   {
      offset = newStr._strLen;

      newStr._strLen += strlen(from_ptr);

      if ((mem = (char *) realloc(newStr._str, newStr._strLen + 1)) == 0)
         throw XBstring(__FILE__, __LINE__, "bstring::replace - Error reallocating string - size '%u'", newStr._strLen + 1);

      newStr._str = mem;
      strcpy(newStr._str + offset, from_ptr);
   }

   newStr._str[newStr._strLen] = 0;

   freeStr();

   _str           = newStr._str;
   _strLen        = newStr._strLen;
   newStr._str    = 0;
   newStr._strLen = 0;
}


/*----------------------------------------------------------------------------
   allocates the space filled string at size 'length'
----------------------------------------------------------------------------*/
void bstring::setLength(const unsigned length, const char ch)
{
   char *tmp;

   _strLen = length;

   if ((tmp = (char *) realloc(_str, _strLen + 1)) == 0)
   {
      throw XBstring(__FILE__, __LINE__, "bstring::setLength - Error reallocating string - size '%u'", _strLen);
   }

   _str = tmp;

   memset(_str, ch, _strLen + 1);

   _str[_strLen] = 0;
}


/*------------------------------------------------------------------------------
   strips chars from the string
------------------------------------------------------------------------------*/
void bstring::stripChars(const bool left, const bool right, char *chars)
{
   if (*_str == 0)
      return;

   if (chars == 0 || *chars == 0)
      chars = WHITE_SPACES;

   char *ptr;
   char *ch;

   if (left)
   {
      for (ptr = _str; *ptr; ptr++)
      {
         for (ch = chars; *ch; ch++)
         {
            if (*ptr == *ch)
               break;
         }

         if (*ch == 0)
            break;
      }

      if (*ptr == 0)
         return;

      if (ptr > _str)
      {
         _strLen -= (ptr - _str);
         memmove(_str, ptr, _strLen + 1);
      }

      if (*_str == 0)
         return;
   }

   if (right)
   {
      ptr = _str + _strLen;

      for (ptr--; ptr >= _str; ptr--)
      {
         for (ch = chars; *ch; ch++)
         {
            if (*ptr == *ch)
               break;
         }

         if (*ch == 0)
            break;
      }

      if (ptr >= _str)
      {
         ptr++;
         _strLen = ptr - _str;
         *ptr = 0;
      }
   }
}



/*------------------------------------------------------------------------------
   bstrings constructor
------------------------------------------------------------------------------*/
bstrings::bstrings()
{
    
}


/*------------------------------------------------------------------------------
   bstrings destructor
------------------------------------------------------------------------------*/
bstrings::~bstrings()
{
}


/*------------------------------------------------------------------------------
   bstringlink constructor
------------------------------------------------------------------------------*/
bstringlink::bstringlink()
{
    
}


/*------------------------------------------------------------------------------
   bstringlink destructor
------------------------------------------------------------------------------*/
bstringlink::~bstringlink()
{
}


/*------------------------------------------------------------------------------
   sets/updates a link
------------------------------------------------------------------------------*/
void bstringlink::set(const bstring &link, const bstring &value)
{
   bstring *has = get(link);

   if (has)
   {
      *has = value;
      return;
   }

   _links.Append(new bstring(link));
   _values.Append(new bstring(value));
}


/*------------------------------------------------------------------------------
   removes by link name
------------------------------------------------------------------------------*/
void bstringlink::remove(const bstring &link)
{
   unsigned index = getIndex(link);

   if (index >= count())
      return;

   _links.Remove(index);
   _values.Remove(index);
}


/*------------------------------------------------------------------------------
   removes by index
------------------------------------------------------------------------------*/
void bstringlink::remove(const unsigned index)
{
   _links.Remove(index);
   _values.Remove(index);
}


/*------------------------------------------------------------------------------
   gets link value
------------------------------------------------------------------------------*/
bstring *bstringlink::get(const bstring &link, 
                          const bool     ignoreCase) const
{
   unsigned index = getIndex(link, ignoreCase);

   if (index >= count())
      return 0;

   return _values.Get(index);
}


/*------------------------------------------------------------------------------
   gets value via index
------------------------------------------------------------------------------*/
bstring *bstringlink::get(const unsigned index) const
{
   return _values.Get(index);
}


/*------------------------------------------------------------------------------
   gets Link value via index   
------------------------------------------------------------------------------*/
bstring *bstringlink::getLink(const unsigned index) const
{
   return _links.Get(index);
}


/*------------------------------------------------------------------------------
   gets the index of a link, returns the count if not found
------------------------------------------------------------------------------*/
unsigned bstringlink::getIndex(const bstring &link, 
                               const bool     ignoreCase) const
{
   unsigned index;

   for (index = 0; index < _links.Count(); index++)
      if (link.compare(_links[index], ignoreCase) == 0)
         break;

   return index;
}


/*------------------------------------------------------------------------------
   clears the objects
------------------------------------------------------------------------------*/
void bstringlink::clear()
{
   _links.Purge();
   _values.Purge();
}


/*------------------------------------------------------------------------------
   converts the links to upper case
------------------------------------------------------------------------------*/
void bstringlink::linksToUpper()
{
   unsigned index;

   for (index = 0; index < _links.Count(); index++)
      _links[index].upper();
}

