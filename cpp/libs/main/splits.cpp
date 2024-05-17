#include "splits.h"
#include "xstring.h"

char* splits(const char *readbuff, char delim, int fldc, const char** flds, int *no_flds)
{
  int i,j,n,m,quote_index,has_single,has_double;
  char* result = strdup(readbuff); // "a|b|c0" 0 is the null
  n = strlen(result);              // n=5
  while (result[n - 1] == '\n' || result[n - 1] == '\r')
    result[--n] = 0;
  strtrim(result);
  has_single = has_double = quote_index = 0;
  for (i=0; i<n; i++)
  {
    quote_index++;
    // if first char is single quote s == 1
    if (quote_index == 1 && result[i] == '\'')
    {
      has_single = 1;
      continue;
    }
    // if first char is double quote d == 1
    if (quote_index == 1 && result[i] == '"')
    {
      has_double = 1;
      continue;
    }
    // if looking for and find single quote s == 0
    if (has_single == 1 && result[i] == '\'')
    {
      has_single = 0;
      continue;
    }
    // if looking for and find double quote d == 0
    if (has_double == 1 && result[i] == '"')
    {
      has_double = 0;
      continue;
    }
    // if in looking for single or double quote
    if (has_single == 1 || has_double == 1)
      continue;
    // not looking for quotes check if delimiter
    if (result[i] == delim)
    {
      result[i] = 0;           // "a0b0c0"
      has_single = has_double = quote_index = 0;
    }
  }
  for (i=0; i<fldc; i++)
    flds[i] = &result[n];      // assuming fldc=3 all three point to last null terminator 
  flds[0] = result;            // "a0"
  for (i=1,j=0; i<fldc; i++,j++)  
  {
    m = strlen(flds[j])+1;     // m=2:m=2:m=2
    n -= m;                    // n=3:n=1:n=-1
    if (n < 1) 
      break;
    flds[i]=flds[j]+m;          // "b0":"c0" 
  }
  *no_flds = j+1;
  return result;
}

enum 
{
  _split_ok
, _no_split_ctx
, _split_no_out_of_range
};

const char* _split_errors[] =
{
  "No error"
  , "No split context available"
  , "Split no out of range"
};

int splits_init(Splits** ctxp, int fldc)
{
  Splits* ctx = (Splits*) malloc(sizeof(Splits));
  *ctxp = ctx;
  ctx->readbuff = 0;
  ctx->writebuff = 0;
  ctx->next = 0;
  ctx->left = 0;
  ctx->no_flds = fldc;
  ctx->fldc = fldc;
  ctx->flds = (char**)calloc(fldc, sizeof(char*));
  ctx->error_no = 0;
  ctx->last_no = 0;
  return _split_ok;
}

int splits_done(Splits** ctxp)
{
  Splits* ctx = *ctxp;
  free(ctx->flds);
  if (ctx->readbuff != 0)
    free(ctx->readbuff);
  if (ctx->writebuff != 0)
    free(ctx->writebuff);
  free(ctx);
  ctx = 0;
  return _split_ok;
}

int splits_read(Splits* ctx, const char* data, char delim)
{
  if (ctx == 0)
    return _no_split_ctx;
  if (ctx->readbuff != 0)
    free(ctx->readbuff);
  if (ctx->writebuff != 0)
  {
    free(ctx->writebuff);
    ctx->writebuff = 0;
  }
  ctx->last_no = 0;
  ctx->no_flds = 0;
  ctx->readbuff = splits(data, delim, ctx->fldc, (const char**)ctx->flds, &ctx->no_flds);
  return ctx->error_no = _split_ok;
}

struct auto_splits_make
{
  int len;
  char* data;
  auto_splits_make(size_t len) { data = new char[len]; this->len = len; }
  ~auto_splits_make() 
  { 
    delete [] data;
  }
  char* check(char* value, char* del)
  {
    if (value == 0)
      return "";
    if (value[0] == '"' || value[0] == '\'')
      return value;
    if (strchr(value, del[0]) == 0)
    {
      if (value[0] != '0')
        return value;
      int cnt = strspn(value, "0123456789");
      if (cnt != strlen(value))
        return value;
    }
    snprintf(data, len, "\"%s\"", value);
    return data;
  }
};

int splits_make(Splits* ctx, char* line, size_t line_len, char delim)
{
  int i, n;
  char del[2];
  del[0]=delim;
  del[1]=0;
  if (ctx == 0)
    return _no_split_ctx;
  n = line_len;
  auto_splits_make work(n);
  strncpy(line, work.check(ctx->flds[0], del), n);
  for (i=1; i<ctx->no_flds; i++)
  {
    strcat(line, del);
    n = line_len - strlen(line);
    strncat(line, work.check(ctx->flds[i], del), n);
  }
  return _split_ok;
}

static const size_t _writebuff_size_ = 4096;

static char* _get_writeBuff_(Splits *ctx)
{
  if (ctx->writebuff == 0)
  {
    ctx->writebuff = (char*) calloc(_writebuff_size_, 1);
    ctx->next = ctx->writebuff;
  }
  else
    ctx->next += strlen(ctx->next) + 1;
  ctx->left = _writebuff_size_ - (ctx->next - ctx->writebuff);
  return ctx->next;
}

int splits_tos(Splits* ctx, short* tgt, int no)
{
  if (ctx == 0)
    return _no_split_ctx;
  ctx->last_no = no;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  *tgt = (short) atoi(ctx->flds[no]);
  return ctx->error_no = _split_ok;
}

int splits_ass(Splits* ctx, short src, int no)
{
  char* tgt;
  if (ctx == 0)
    return _no_split_ctx;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  tgt = _get_writeBuff_(ctx);
  snprintf(tgt, ctx->left, "%d", (int)src);
  ctx->flds[no] = tgt;
  return _split_ok;
}

int splits_toi(Splits* ctx, int* tgt, int no)
{
  if (ctx == 0)
    return _no_split_ctx;
  ctx->last_no = no;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  *tgt = atoi(ctx->flds[no]);
  return ctx->error_no = _split_ok;
}

int splits_asi(Splits* ctx, int src, int no)
{
  char* tgt;
  if (ctx == 0)
    return _no_split_ctx;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  tgt = _get_writeBuff_(ctx);
  snprintf(tgt, ctx->left, "%d", src);
  ctx->flds[no] = tgt;
  return _split_ok;
}

int splits_tol(Splits* ctx, long long* tgt, int no)
{
  if (ctx == 0)
    return _no_split_ctx;
  ctx->last_no = no;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  *tgt = atoll(ctx->flds[no]);
  return ctx->error_no = _split_ok;
}

int splits_asl(Splits* ctx, long long src, int no)
{
  char* tgt;
  if (ctx == 0)
    return _no_split_ctx;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  tgt = _get_writeBuff_(ctx);
  snprintf(tgt, ctx->left, "%lld", src);
  ctx->flds[no] = tgt;
  return _split_ok;
}

int splits_tof(Splits* ctx, double* tgt, int no)
{
  if (ctx == 0)
    return _no_split_ctx;
  ctx->last_no = no;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  *tgt = atof(ctx->flds[no]);
  return ctx->error_no = _split_ok;
}

int splits_asf(Splits* ctx, double src, int no)
{
  char* tgt;
  if (ctx == 0)
    return _no_split_ctx;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  tgt = _get_writeBuff_(ctx);
  snprintf(tgt, ctx->left, "%lf", src);
  ctx->flds[no] = tgt;
  return _split_ok;
}

int splits_asf2(Splits* ctx, double src, int no)
{
  char* tgt;
  if (ctx == 0)
    return _no_split_ctx;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  tgt = _get_writeBuff_(ctx);
  snprintf(tgt, ctx->left, "%0.2f", src);
  ctx->flds[no] = tgt;
  return _split_ok;
}

int splits_toch(Splits* ctx, char* work, int worklen, int no)
{
  if (ctx == 0)
    return _no_split_ctx;
  ctx->last_no = no;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  strncpy(work, ctx->flds[no], worklen-1);
  work[worklen-1] = 0;
  return ctx->error_no = _split_ok;
}

int splits_asch(Splits* ctx, char* src, int no)
{
  char* tgt;
  char work[512];
  int n;
  if (ctx == 0)
    return _no_split_ctx;
  if (no >= ctx->no_flds)
    return ctx->error_no = _split_no_out_of_range;
  strncpy(work, src, sizeof(work)-1);
  work[sizeof(work)-1]=0;
  tgt = _get_writeBuff_(ctx);
  n = strlen(src)-1;
  while(work[n] == ' ' || work[n] == '\r' || work[n] == '\t')
  {
    work[n] = 0;
    n--;
  }
  snprintf(tgt, ctx->left, "%s", work);
  ctx->flds[no] = tgt;
  return _split_ok;
}

const char* splits_error(Splits* ctx)
{
  if (ctx == 0)
    return _split_errors[_no_split_ctx];
  return _split_errors[ctx->last_no];
}

int splits_last_no(Splits* ctx)
{
  if (ctx == 0)
    return -1;
  return ctx->last_no;
}

int splits_used(Splits* ctx)
{
  if (ctx == 0)
    return -1;
  return ctx->no_flds;
}

