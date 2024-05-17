#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "zlib.h"
#include "machine.h"

#include "zcompress.h"

const int32 ZSIGNET = 0xEDACACED;

static void* ZAlloc(voidpf opaque, unsigned items, unsigned size)
{
	void * Ptr;
  Ptr = malloc(items * size);
  return Ptr;
}

static void  ZFree(voidpf opaque, void * ptr)
{
  free(ptr);
}

extern "C" DLLEXPORT_STDCALL(int) ZDecompress(unsigned char * InBuff, int32 InLen, unsigned char * OutBuff, int32 * OutLen)
{
  if (*(int32*)InBuff != ZSIGNET || InLen == *OutLen)
  {
    memcpy(OutBuff, InBuff, *OutLen);
    return Z_OK;
  }
  z_stream ZStream;
  int Erc;
	ZStream.avail_in = 0;
	ZStream.avail_out = *OutLen;
	ZStream.next_out = OutBuff;
	ZStream.zalloc = ZAlloc;
	ZStream.zfree  = ZFree;
	ZStream.opaque = Z_NULL;
	ZStream.next_in = InBuff+4;
	ZStream.avail_in = InLen-4;
  Erc = inflateInit(&ZStream);
  if (Erc < Z_OK)
		return Erc;
	Erc = inflate(&ZStream, Z_NO_FLUSH);
  if (Erc < Z_OK)
		return Erc;
	Erc = inflateEnd(&ZStream);
  if (Erc < Z_OK)
		return Erc;
  *OutLen = ZStream.total_out;
	return Z_OK;
}

extern "C" DLLEXPORT_STDCALL(int) ZCompress(unsigned char * InBuff, int32 InLen, unsigned char * OutBuff, int32 * OutLen)
{
  z_stream ZStream;
  int Level;
  int Erc = Z_OK;
  if (*(int32*)InBuff == ZSIGNET)
    goto CopyBuffer;
  Level = 6;
  ZStream.avail_in = 0;
  ZStream.avail_out = (*OutLen)-4;
  ZStream.next_out = OutBuff+4;
  ZStream.zalloc = ZAlloc;
  ZStream.zfree  = ZFree;
  ZStream.opaque = Z_NULL;
  Erc = deflateInit(&ZStream, Level);
  if (Erc == Z_STREAM_ERROR)
    goto CopyBuffer;
  ZStream.next_in = InBuff;
  ZStream.avail_in = InLen;
  Erc = deflate(&ZStream, Z_FINISH);
  if (Erc < Z_OK)
    goto CopyBuffer;
  Erc = deflateEnd(&ZStream);
  if (Erc < Z_OK)
    goto CopyBuffer;
  if (ZStream.total_out+4 >= (unsigned) InLen)
    goto CopyBuffer;
  *(int32*)OutBuff = ZSIGNET;
  *OutLen = ZStream.total_out+4;
  return Erc;
CopyBuffer:
  *OutLen = InLen < *OutLen ? InLen : *OutLen;
  memcpy(OutBuff, InBuff, *OutLen);
  return Erc;
}

extern "C" DLLEXPORT_STDCALL(int) ZImageDecompress(unsigned char * InBuff, unsigned char * OutBuff, int32_t * OutLen)
{
  int32_t InLen = *(short*)InBuff;
  unsigned char *Buff = InBuff+2;
  int Erc = ZDecompress(Buff, InLen, OutBuff, OutLen);
  return Erc;
}

extern "C" DLLEXPORT_STDCALL(int) ZImageCompress(unsigned char * InBuff, int32_t InLen, unsigned char * OutBuff, int32_t MaxLen)
{
  int32_t OutLen=MaxLen;
  int Erc = ZCompress(InBuff, InLen, OutBuff+2, &OutLen);
  *(short *)OutBuff = (short) OutLen;
  return Erc;
}

