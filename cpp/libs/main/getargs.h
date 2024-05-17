#ifndef _GETARGS_H_
#define _GETARGS_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(_GETARGS_H_);

#define INTEGER     0
#define BOOLEAN     1
#define CHARACTER   2
#define STRING      3
#define UPCASE     64

extern char getargs_switch_char;
extern int  getargs_passthru;

typedef struct
    {
    char  arg;        /* command line switch */
    char  type;       /* variable type */
    int   *variable;  /* pointer to variable */
    char  *errmsg;    /* pointer to error message */
    } ARG;

#define argswitch(a) getargs_switch_char = a
/* Unix style = '-' Dos style = '/' */

#define argspassthru() getargs_passthru = 1
/* Allow unrecognized switches for later processes */

#if defined(M_OS2) || defined(M_DOS)
#define pr_usage PrUsage
#define getargs  GetArgs
#endif

void PrUsage(ARG *tabp, int tabsize);
int  GetArgs(int argc, char **argv, ARG *tabp, int tabsize);

#if 0


   EXAMPLE OF USAGE
   ...
   #include "getargs.h"

   static int device   = 0;
   static int method   = 1;
   static int gridsave = 0;
   static int masksize = 16;
   static int restore = 0;
   static char *passon = "";
   ARG argtab[] =
       {
       {'d', INTEGER, &device,   "Device 0=EGA16, 1=VGA16, 2=VGA256"},
       {'m', INTEGER, &masksize, "Size picture from 16/16ths to 1/16th"},
       {'c', INTEGER, &method,   "Compress 0=None, 1=Packbits, 2=LZW"},
       {'p', STRING,  (int *) &passon,   "Switches to Device Overlays"},
       {'g', BOOLEAN, &gridsave, "Save previous grid information"},
       {'G', BOOLEAN, &restore, "Dont restore from graphics mode"}
       };
   #define TABSIZE (sizeof(argtab) / sizeof(ARG))
   int main(int argc, char *argv[])
       {
       argc = getargs(argc, argv, argtab, TABSIZE);
       ...
       return 0;
       }

#endif

#endif
