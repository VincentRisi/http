#include "versions.h"
#define _VERSION_NO "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
COMPILED_VERSION(_VERSION_NO);

#include "machine.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined(M_OS2) || defined(M_DOS) || defined(M_W32)
#include <process.h>
#endif

#include "getargs.h"

#define ERRMSG "Illegal argument <%c>.  Legal arguments are:\n\n"

char getargs_switch_char = '-';
int  getargs_passthru = 0;

static ARG *findarg(int c, ARG *tabp, int tabsize);
static char *setarg(ARG *argp, char *linep);
static int stoi(char **instr);

static int stoi(char **instr)
{
  register int num = 0;
  register char *str;
  int sign = 1;

  str = *instr;
  while (*str == ' ' || *str == '\t' || *str == '\n')
    str++;
  if (*str == '-')
  {
    sign = -1;
    str++;
  }
  if (*str == '0')
  {
    ++str;
    if (*str == 'x' || *str == 'X')
    {
      str++;
      while (('0' <= *str && *str <= '9')
      ||   ('a' <= *str && *str <= 'f')
      ||   ('A' <= *str && *str <= 'F'))
      {
        num *= 16;
        num += ('0' <= *str && *str <= '9')
           ? *str - '0'
           : toupper(*str) - 'A' + 10;
        str++;
      }
    }
    else
    {
      while ('0' <= *str && *str <= '7')
      {
        num *= 8;
        num += *str++ - '0';
      }
    }
  }
  else
  {
    while ('0' <= *str && *str <= '9')
    {
      num *= 10;
      num += *str++ - '0';
    }
  }
  *instr = str;
  return num * sign;
}

static char _blank[1];
static char *setarg(ARG *argp, char *linep)
{
//simon 20010820  strcpy(linep, linep+1);
  linep++;
  switch (argp->type)
  {
  case INTEGER:
    *argp->variable = stoi(&linep);
    break;
  case BOOLEAN:
    if (*linep == '-')
    {
      *argp->variable = 0;
      linep++;
//simon 20010820      strcpy(linep, linep+1);
    }
    else *argp->variable = 1;
    break;
  case CHARACTER:
    *argp->variable = *linep;
//simon 20010820    strcpy(linep, linep+1);
    linep++;
    break;
  case STRING:
    *(char **) argp->variable = linep;
    linep = _blank;
    break;
  default:
    printf("getargs : INTERNAL ERROR: BAD ARGUMENT TYPE\n");
    break;
  }
  return linep;
}

static ARG *findarg(int c, ARG *tabp, int tabsize)
{
  for (;--tabsize >= 0;  tabp++)
    if (tabp->arg == (char) c)
      return tabp;
  return 0;
}

void PrUsage(ARG *tabp, int tabsize)
{
  for (;--tabsize >= 0;  tabp++)
  {
    switch (tabp->type)
    {
    case INTEGER:
      printf("%c%c<num> %-40s [%-5d]\n", getargs_switch_char,
             tabp->arg, tabp->errmsg, *(tabp->variable));
      break;
    case BOOLEAN:
      printf("%c%c      %-40s [%-5s]\n", getargs_switch_char,
             tabp->arg, tabp->errmsg, *(tabp->variable) ? "TRUE" : "FALSE");
      break;
    case CHARACTER:
      printf("%c%c<c>   %-40s [%-5c]\n", getargs_switch_char,
             tabp->arg, tabp->errmsg, *(tabp->variable));
      break;
    case STRING:
      printf("%c%c<str> %-40s [%s]\n", getargs_switch_char,
             tabp->arg, tabp->errmsg, *(char **) tabp->variable);
      break;
    }
  }
}

int GetArgs(int argc, char *argv[], ARG *tabp, int tabsize)
{
  register int nargc;
  register char **nargv, *p;
  register ARG *argp;

  nargc = 1;
  for (nargv = ++argv;  --argc > 0;  argv++)
  {
    if (**argv != getargs_switch_char)
    {
      *nargv++ = *argv;
      nargc++;
    }
    else
    {
      p = (*argv) + 1;
      while (*p)
      {
        if ((argp = findarg(*p, tabp, tabsize))!=0)
          p = setarg(argp, p);
        else if (getargs_passthru)
        {
          *nargv++ = *argv;
          nargc++;
          break;
        }
        else
        {
          PrUsage(tabp, tabsize);
#ifndef M_W32
          exit(1);
#else
          return -1;
#endif
        }
      }
    }
  }
  return nargc;
}
