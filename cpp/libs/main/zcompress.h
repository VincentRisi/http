#ifndef _COMPRESSLIB_H_
#define _COMPRESSLIB_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(_COMPRESSLIB_H_);

extern "C" DLLEXPORT_STDCALL(int) ZDecompress(unsigned char * InBuff, int32 InLen, unsigned char * OutBuff, int32 * OutLen);
extern "C" DLLEXPORT_STDCALL(int) ZCompress(unsigned char * InBuff, int32 InLen, unsigned char * OutBuff, int32 * OutLen);
extern "C" DLLEXPORT_STDCALL(int) ZImageDecompress(unsigned char * InBuff, unsigned char * OutBuff, int32 * OutLen);
extern "C" DLLEXPORT_STDCALL(int) ZImageCompress(unsigned char * InBuff, int32 InLen, unsigned char * OutBuff, int32 MaxLen);

#endif
