#ifndef swapbytesH
#define swapbytesH "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
#include "machine.h"
HEADER_VERSION(swapbytesH);

// This header is used by the DLL and Server code generated using
// the IDL compiler with the PopGen generator. It implements the
// inline Swap and Copy functions

inline void BBD_SWAP_MACRO(char &a, char &b) { a ^= b ^= a ^= b; }
inline void SwapBytes(char* p, int b)
{
  int f = 0;
  while (f < b)
    BBD_SWAP_MACRO(p[f++], p[--b]);
}

const int32 doSwap=0x03020100;
const bool mustDoSwap = (*(char*)&doSwap == 0);
template <class T>
inline void SwapBytes(T &a)
{
  if (mustDoSwap)
    SwapBytes((char*)&a, sizeof(a));
}

inline void TrimTrailingBlanks(char *data, int length)
{
  for (int i=length-1; i >= 0 && (data[i] == ' ' || data[i] == 0); i--)
    data[i] = 0;
}

#ifdef M_AIX
inline void TrimTrailingBlanks(int8 *data, int length)
{
  for (int i=length-1; i >= 0 && (data[i] == ' ' || data[i] == 0); i--)
    data[i] = 0;
}
#endif

inline void AlwaysSwapBytes(double &a)
{
  char* p = (char*)&a;
  char z;
  z =  p[0]; p[0] = p[7]; p[7] = z;
  z =  p[1]; p[1] = p[6]; p[6] = z;
  z =  p[2]; p[2] = p[5]; p[5] = z;
  z =  p[3]; p[3] = p[4]; p[4] = z;
/*
  NGSwapMacro(p[0], p[7]);
  NGSwapMacro(p[1], p[6]);
  NGSwapMacro(p[2], p[5]);
  NGSwapMacro(p[3], p[4]);
*/
}

inline void AlwaysSwapBytes(float &a)
{
  char* p = (char*)&a;
  char z;
  z =  p[0]; p[0] = p[3]; p[3] = z;
  z =  p[1]; p[1] = p[2]; p[2] = z;
/*
  NGSwapMacro(p[0], p[3]);
  NGSwapMacro(p[1], p[2]);
*/  
}

inline void AlwaysSwapBytes(long &a)
{
  char* p = (char*)&a;
  char z;
  z =  p[0]; p[0] = p[3]; p[3] = z;
  z =  p[1]; p[1] = p[2]; p[2] = z;
/*
  NGSwapMacro(p[0], p[3]);
  NGSwapMacro(p[1], p[2]);
*/
}

inline void AlwaysSwapBytes(short &a)
{
  char* p = (char*)&a;
  char z;
  z =  p[0]; p[0] = p[1]; p[1] = z;
/*
  NGSwapMacro(p[0], p[1]);
*/
}


#endif
