#ifndef _XSTRING_H_
#define _XSTRING_H_
#include "machine.h"

#include <string.h>

/*  case insensitive strcmp */
int tolower_stricmp(const char *a, const char *b);

/*  case insensitive bounded length strcmp */
int tolower_strnicmp(const char *a, const char *b, int n);

#if !defined(__BORLANDC__)
inline int stricmp(const char *a, const char *b) 
{ 
  return tolower_stricmp(a, b);
}

inline int strnicmp(const char *a, const char *b, int n)
{
  return tolower_strnicmp(a, b, n);
}
#endif

inline bool streq(const char* s, const char* t)
{
  return (strcmp(s, t) == 0);
}

inline bool strieq(const char* s, const char* t)
{
#ifdef M_W32
  return (_stricmp(s, t) == 0);
#else
    return (stricmp(s, t) == 0);
#endif
}

inline bool strneq(const char* s, const char* t, int n)
{
  return (strncmp(s, t, n) == 0);
}

inline bool strnieq(const char* s, const char* t, int n)
{
  return (strnicmp(s, t, n) == 0);
}

#if defined(M_AIX) || defined(M_GNU)
/*  convert string to lower case */
char *strlwr(char *s);

/*  convert string to upper case */
char *strupr(char *s);

#endif

/*  remove leading spaces and tabs */
char *strltrim(char *s);

/*  remove trailing spaces and tabs */
char *strrtrim(char *s);

/*  remove leading and trailing spaces and tabs */
inline char* strtrim(char *s)
{
  return strltrim(strrtrim(s));
}

/*  pad string in field of length fldlen (the data area length is fldlen + 1) */
/*  eg "abc"   --->  "       abc" */
char *strlpad(char *s, const int fldlen, const char padchar);

/*  pad string in field of length fldlen (the data area length is fldlen + 1) */
/*  eg "abc"   --->  "   abc    " */
char *strcpad(char *s, const int fldlen, const char padchar);

/*  pad string in field of length fldlen (the data area length is fldlen + 1) */
/*  eg "abc"   --->  "abc       " */
char *strrpad(char *s, const int fldlen, const char padchar);

/*  copies src string to dest for at most n bytes. */
/*  dest is null-terminated in all cases (unlike strncpy) */
/*  example usage: strncpyz(dest, src, sizeof(dest) - 1); */
/*  after the function call strlen(dest) = min(n, strlen(src)) */
char *strncpyz(char *dest, const char *src, int n);

/*  allocate an array - the same length of s, and copy s into it */
/*  Self exists in the aix library but is not in the header file */
/*  unless one uses the cc (extended compiler) - Self is not feasible */
/*  since cc does not expand macros correctly (in particular the */
/*  stringizing operator used in the assert macro) */
/* char* strdup(const char* s); */

#endif
