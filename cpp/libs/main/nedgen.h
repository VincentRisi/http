#ifndef _NEDGEN_H_
#define _NEDGEN_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(_NEDGEN_H_);

// This header is used by the DLL and Server code generated using
// the IDL compiler with the PopGen generator. It implements the
// inline Swap and Copy functions

#ifndef M_AIX
#define NGSwapMacro(a, b) (a) ^= (b) ^= (a) ^= (b)
#endif

inline void SwapBytes(double &a)
{
#ifndef M_AIX
  char* p = (char*)&a;
  NGSwapMacro(p[0], p[7]);
  NGSwapMacro(p[1], p[6]);
  NGSwapMacro(p[2], p[5]);
  NGSwapMacro(p[3], p[4]);
#endif
}

inline void SwapBytes(float &a)
{
#ifndef M_AIX
  char* p = (char*)&a;
  NGSwapMacro(p[0], p[3]);
  NGSwapMacro(p[1], p[2]);
#endif
}

inline void SwapBytes(long &a)
{
#ifndef M_AIX
  char* p = (char*)&a;
  NGSwapMacro(p[0], p[3]);
  NGSwapMacro(p[1], p[2]);
#endif
}

inline void SwapBytes(int32 &a)
{
#ifndef M_AIX
  char* p = (char*)&a;
  NGSwapMacro(p[0], p[3]);
  NGSwapMacro(p[1], p[2]);
#endif
}

inline void SwapBytes(short &a)
{
#ifndef M_AIX
  char* p = (char*)&a;
  NGSwapMacro(p[0], p[1]);
#endif
}

inline void SwapBytes(unsigned short &a)
{
#ifndef M_AIX
  char* p = (char*)&a;
  NGSwapMacro(p[0], p[1]);
#endif
}

inline void TrimTrailingBlanks(char *data, int length)
{
  for (int i=length-1; i >= 0 && (data[i] == ' ' || data[i] == 0); i--)
    data[i] = 0;
}

#ifndef M_AIX
#undef NGSwapMacro
#endif

#endif