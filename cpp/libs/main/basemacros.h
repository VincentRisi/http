/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2007/01/01
 Name   : base macros
 Descr  : A bunch of macros that are useful, and commonly defined
==============================================================================*/


#ifndef __BASE_MACROS_H_
#define __BASE_MACROS_H_

#define _DELETE(ptr)     if (ptr) { delete ptr; ptr = 0; }
#define _DELETE_ARR(ptr) if (ptr) { delete [] ptr; ptr = 0; }
#define _FREE(ptr) if (ptr) { free(ptr); ptr = 0; }
#define _SWOP_VAL(a, b) (a) ^= (b) ^= (a) ^= (b)
#define _FAST_INT_DIV_BY_2(val) (((val) + (val)) >> 2)
#define _SWOP_SIGN(val) (val) = 0 - (val)

#ifndef STRNCPY
#include "xstring.h"
template <size_t N>
inline char* zzcopy(char(&target)[N], const char *source){ return strncpyz(target, source, N - 1); }
#define STRNCPY(targ, src) zzcopy(targ, src) 
#endif

#define _STR_IS_NULL(s) ((s) == 0 || *(s) == 0)

#define _BITON(aa,bb)     (aa |= (bb)) 
#define _BITOFF(aa,bb)    (aa = ((aa&(bb)) == (bb) ? (aa^(bb)) : aa))
#define _BITTOGGLE(aa,bb) (aa = aa^(bb))
#define _ALLBITSON(aa)    (aa = 0, aa = ~aa)
#define _ALLBITSOFF(aa)   (aa = 0)
#define _ISBITON(aa,bb)   (aa&(bb))

#define _BIT1  0x01 
#define _BIT2  0x02
#define _BIT3  0x04
#define _BIT4  0x08
#define _BIT5  0x10
#define _BIT6  0x20
#define _BIT7  0x40
#define _BIT8  0x80   


#endif
