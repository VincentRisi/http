#ifndef _RDC_H_
#define _RDC_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(_RDC_H_);

#include <stddef.h>

size_t RdcCompress(unsigned char *aInBuff, size_t aInBuffLen, unsigned char *aOutBuff);

size_t RdcDecompress(unsigned char * aInBuff, size_t aInbuffLen, unsigned char *aOutBuff);

#endif
