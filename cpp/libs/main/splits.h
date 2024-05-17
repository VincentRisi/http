#ifndef splitsH
#define splitsH

#include "machine.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#ifdef M_W32
#define atoll _atoi64 
#endif

typedef struct Splits
{
  char* readbuff;
  char* writebuff;
  char* workbuff;
  char** flds;
  char* next;
  size_t left;
  int fldc;
  int no_flds;
  int error_no;
  int last_no;
} Splits;

/**
 * init constructs context
 */
int   splits_init(Splits** ctx, int fldc);
/**
 * done cleans up context
 */
int   splits_done(Splits** ctx);
/**
 * read splits data into at least fldc fields
 */
int   splits_read(Splits* ctx, const char* data, char delim);
/**
 * make builds a line from flds using delim char
 */
int   splits_make(Splits* ctx, char* line, size_t line_len, char delim);
/**
 * no_flds reports back no of fields were split
 */
int   splits_used(Splits* ctx);
/**
 * last_no reports back last no asked for
 */
int   splits_last_no(Splits* ctx);
/**
 * returns last error or no context
 */
const char* splits_error(Splits* ctx);
/**
 * copies field into tgt short int16
 */
int splits_tos(Splits* ctx, short* tgt, int no);
/**
 * makes field into write short int16
 */
int splits_ass(Splits* ctx, short src, int no);
/**
 * copies field into tgt int int32
 */
int splits_toi(Splits* ctx, int* tgt, int no);
/**
 * makes field into write int int32
 */
int splits_asi(Splits* ctx, int src, int no);
/**
 * copies field into tgt long int64
 */
int splits_tol(Splits* ctx, long long* tgt, int no);
/**
 * makes field into write long int64
 */
int splits_asl(Splits* ctx, long long src, int no);
/**
 * copies field into tgt double 15 digit precision
 */
int splits_tof(Splits* ctx, double* tgt, int no);
/**
 * makes field into write double as money
 */
int splits_asf2(Splits* ctx, double src, int no);
/**
 * makes field into write double
 */
int splits_asf(Splits* ctx, double src, int no);
/**
 * copies field to char tgt a max of worklen-1
 */
int splits_toch(Splits* ctx, char* tgt, int maxlen, int no);
/**
 * makes field into write null terminated char*
 */
int splits_asch(Splits* ctx, char* src, int no);

/**
 * readbuff    - input line
 * delim       - delimiter
 * fldc        - no of elements in flds list
 * flds        - flds array to contain pointers
 * returns     - copy of readbuff - free up after use
 * no_flds     - is filled in with the no of elements found
 */

char* splits(const char *readbuff, char delim, int fldc, const char** flds, int *no_flds);

/**
Wrapper on context based c code
if used as stack based the destructor will auto destroy
*/
class Splitter
{
  Splits* ctx;
  int rc;
public:
  /* constructor uses count for max fields to use */
  Splitter(int noFields) { rc = splits_init(&ctx, noFields); }
  virtual ~Splitter()    { rc = splits_done(&ctx); }
  const char* error()                     { return splits_error(ctx); }
  int get_rc()                            { return rc; }
  int no_flds()                           { return ctx->no_flds; }
  int error_no()                          { return ctx->error_no; }
  int read(const char* data, char delim)  { return rc = splits_read(ctx, data, delim); }
  int make(char* line, size_t line_len, char delim) { return rc = splits_make(ctx, line, line_len, delim); }
  int used()                              { return rc = splits_used(ctx); }
  int last_no()                           { return rc = splits_last_no(ctx); }
  int tos(short* tgt, int no)             { return rc = splits_tos(ctx, tgt, no); }
  int ass(short src, int no)              { return rc = splits_ass(ctx, src, no); }
  int toi(int* tgt, int no)               { return rc = splits_toi(ctx, tgt, no); }
  int asi(int src, int no)                { return rc = splits_asi(ctx, src, no); }
  int tol(long long* tgt, int no)         { return rc = splits_tol(ctx, tgt, no); }
  int asl(long long src, int no)          { return rc = splits_asl(ctx, src, no); }
  int tof(double* tgt, int no)            { return rc = splits_tof(ctx, tgt, no); }
  int asf2(double src, int no)            { return rc = splits_asf2(ctx, src, no); }
  int asf(double src, int no)             { return rc = splits_asf(ctx, src, no); }
  int toch(char* tgt, int maxlen, int no) { return rc = splits_toch( ctx, tgt, maxlen, no); }
  int asch(char* src, int no)             { return rc = splits_asch(ctx, src, no); }
};

#endif
