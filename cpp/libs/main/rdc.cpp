// RDCC.C
// Ross Data Compression (RDC)
// Compress function
// Written Ed Ross, 1/92
// Modified Vince Risi, 7/94

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rdc.h"

#define BEST_HASH 4096

size_t RdcCompress(unsigned char *aInBuff, size_t aInBuffLen, unsigned char *aOutBuff)
{
  unsigned char *HashTbl[BEST_HASH];
  const int HashLen = BEST_HASH;
  unsigned char *InBuff = aInBuff;
  unsigned char *InBuffEnd = aInBuff + aInBuffLen;
  unsigned char *Anchor;
  unsigned char *PatIdx;
  int Cnt;
  int Gap;
  int c;
  int Hash;
  unsigned char *CtrlIdx = aOutBuff;
  unsigned char  CtrlBits = 0;                  // for lint sake
  unsigned char  CtrlCnt = 0;
  unsigned char *OutBuff = aOutBuff + 1;
  unsigned char *OutBuffEnd = aOutBuff + (aInBuffLen - 24);
  if (aInBuffLen <= 9)                  // skip compression for smaller
  {                                     // buffer
    memcpy(aOutBuff, aInBuff, aInBuffLen);
    return 0;
  }
  memset(HashTbl, 0, sizeof(unsigned char *)    // clear Hash table for given length
                      * HashLen);
  while (InBuff < InBuffEnd)            // scan thru buff
  {
    if (CtrlCnt++ == 8)                 // make room for control bits
    {                                   // and check for aOutBuff overflow
      *CtrlIdx = CtrlBits;
      CtrlCnt = 1;
      CtrlIdx = OutBuff;
      OutBuff++;
      if (OutBuff > OutBuffEnd)
      {
         memcpy(aOutBuff, aInBuff, aInBuffLen);
         return 0;
      }
    }
    //  look for RLE
    Anchor = InBuff;
    c = *InBuff++;
    while (InBuff < InBuffEnd
       && *InBuff == c
       && (InBuff - Anchor) < 4110)     // Was 4114
      InBuff++;
                                        // store compression code if character
                                        // is repeated more than 2 times
    if ((Cnt = (InBuff - Anchor)) > 2)
    {
      if (Cnt <= 18)                    // short RLE
      {
        *OutBuff++ = (unsigned char)(Cnt - 3);
        *OutBuff++ = (unsigned char)c;
      }
      else                              // long  RLE
      {
        Cnt -= 19;
        *OutBuff++ = (unsigned char)(16 + (Cnt & 0x0F));
        *OutBuff++ = (unsigned char)(Cnt >> 4);
        *OutBuff++ = (unsigned char)c;
      }
      CtrlBits = (unsigned char)((CtrlBits << 1) | 1);
      continue;
    }
    InBuff = Anchor;                    // look for pattern if 2 or more
    if ((InBuffEnd - InBuff) > 2)       // chars remain in the input buffer
    {                                   // locate offset of possible pattern
                                        // in sliding dictionary
      Hash = ((((InBuff[0] << 8) | InBuff[1]) ^
             ((InBuff[0] >> 4) | (InBuff[2] << 4))) % HashLen);
      PatIdx = HashTbl[Hash];
      HashTbl[Hash] = InBuff;
      if (PatIdx && (Gap = (InBuff - PatIdx)) <= 4090)
      {                                 // compare chars id wer'e within
                                        // 4098 bytes (set to 4090)
        while (InBuff < InBuffEnd && PatIdx < Anchor
           &&  *PatIdx == *InBuff && (InBuff - Anchor) < 271)
        {
          InBuff++;
          PatIdx++;
        }
        if ((Cnt = (InBuff - Anchor)) > 2)
        {                               // store pattern if it is more
          Gap -= 3;                     // than two chars
          if (Cnt <= 15)                // short pattern
          {
            *OutBuff++ = (unsigned char)((Cnt << 4) + (Gap & 0x0F));
            *OutBuff++ = (unsigned char)(Gap >> 4);
          }
          else
          {
            *OutBuff++ = (unsigned char)(32 + (Gap & 0x0F));
            *OutBuff++ = (unsigned char)(Gap >> 4);
            *OutBuff++ = (unsigned char)(Cnt - 16);
          }
          CtrlBits = (unsigned char)((CtrlBits << 1) | 1);
          continue;
        }
      }
    }
    *OutBuff++ = (unsigned char)c;              // cant compress this char so
    InBuff = ++Anchor;                  // copy it to out buf
    CtrlBits <<= 1;
  }
  CtrlBits <<= (8 - CtrlCnt);           // save last load of bits
  *CtrlIdx = CtrlBits;
  return (size_t) (OutBuff - aOutBuff); // and return size of buffer
}


size_t RdcDecompress(unsigned char *aInBuff, size_t aInBuffLen, unsigned char *aOutBuff)
{
  unsigned char CtrlBits;
  unsigned char CtrlMask = 0;
  unsigned char *InBuff = aInBuff;
  unsigned char *OutBuff = aOutBuff;
  unsigned char *InBuffEnd = InBuff + aInBuffLen;
  int Cmd;
  int Cnt;
  int Ofs;

  while (InBuff < InBuffEnd)            // process each item in InBuff
  {
    if ((CtrlMask >>= 1) == 0)          // get new load of control bits
    {                                   // if needed
      CtrlBits = *InBuff;
      InBuff++;
      CtrlMask = 0x80;
    }
    if ((CtrlBits & CtrlMask) == 0)     // just copy this char if control
    {                                   // bit is zero
      *OutBuff++ = *InBuff++;
      continue;
    }
    Cmd = ((*InBuff >> 4) & 0x0F);        // Undo the compression code
    Cnt = (*InBuff++ & 0x0F);
    switch(Cmd)
    {
    case 0:                             // short rule
      Cnt += 3;
      memset(OutBuff, *InBuff++, Cnt);
      OutBuff += Cnt;
      break;
    case 1:                             // long rule
      Cnt += (*InBuff++ << 4);
      Cnt += 19;
      memset(OutBuff, *InBuff++, Cnt);
      OutBuff += Cnt;
      break;
    case 2:                             // long pattern
      Ofs = (Cnt + 3);
      Ofs += (*InBuff++ << 4);
      Cnt = *InBuff++;
      Cnt += 16;
      memcpy(OutBuff, OutBuff - Ofs, Cnt);
      OutBuff += Cnt;
      break;
    default:                            // short pattern
      Ofs = (Cnt + 3);
      Ofs += (*InBuff++ << 4);
      memcpy(OutBuff, OutBuff - Ofs, Cmd);
      OutBuff += Cmd;
      break;
    }
  }

  return (size_t) (OutBuff - aOutBuff); // return length decompressed
}

