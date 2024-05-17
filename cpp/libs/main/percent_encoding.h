#ifndef H_PERCENT_ENCODING
#define H_PERCENT_ENCODING
#include "machine.h"

#include <stdint.h>
#include <stdbool.h>

struct reserved_set
{
    uint8 set[16];
};

int max_encoded_buffer_size(int decoded_buffer_size);
int percent_encode(char *in_buff, int in_len, char* out_buff, int out_len);
int percent_encode_custom(const reserved_set *set, char *in_buff, int in_len, char* out_buff, int out_len);

int percent_decode(char *in_buff, int in_len, char* out_buff, int out_len);

extern const reserved_set all_reserved_set;
extern const reserved_set conservative_reserved_set;
extern const reserved_set liberal_reserved_set;

void mark_unreserved(reserved_set* set, char c);
void mark_reserved(reserved_set* set, char c);
bool is_reserved(const reserved_set* set, char c);

#endif