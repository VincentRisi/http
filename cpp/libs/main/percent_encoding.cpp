#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>

#include "percent_encoding.h"

const reserved_set all_reserved_set =
{ {0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0} };

const reserved_set conservative_reserved_set =
{ {0,0,0,0,0,96,255,3,
  254,255,255,135,254,255,255,71} };

const reserved_set liberal_reserved_set =
{ {0,0,0,0,218,255,255,175,
  255,255,255,175,254,255,255,71} };

void mark_unreserved(reserved_set* set, char c)
{
    if (c > 127) return;
    int quot = c / 8;
    uint8_t rem = c % 8;
    set->set[quot] |= (1 << rem);
}

void mark_reserved(reserved_set* set, char c)
{
    if (c > 127) return;
    int quot = c / 8;
    int rem = c % 8;
    set->set[quot] &= ~(1 << rem);
}

bool is_reserved(const reserved_set* set, char c)
{
    if (c > 127) return true;
    int quot = c / 8;
    int rem = c % 8;
    uint8_t mask = ~(1 << rem);
    uint8_t bits = set->set[quot];
    return (bits & mask) == bits;
}


// If you need null termination, you'll need to account for that. We don't.
int max_encoded_buffer_size(int decoded_buffer_size)
{
    return 3 * decoded_buffer_size;
}

static char hex_encode(uint8_t half_byte)
{
    static char hex[] = "0123456789abcdef";
    return hex[half_byte & 15];
}

static char hex_decode_char(uint8_t c)
{
    return isdigit(c) ? c - '0' : c - 'a' + 10;
}

static char hex_decode(uint8_t hi, uint8_t lo)
{
    return (hex_decode_char(hi) << 4) + hex_decode_char(lo);
}

static bool is_hex_char(int c)
{
    return (isdigit(c) || (c <= 'f' && c >= 'a'));
}

// returns number of bytes used in output buffer.
// if the output buffer is not big enough, this returns
// -1.
int percent_encode_custom(const reserved_set* set, char* in_buff, int in_len, char* out_buff, int out_len)
{
    int in_pos, out_pos;
    in_pos = out_pos = 0;
    while (in_pos < in_len)
    {
        char c = in_buff[in_pos++];

        if (!is_reserved(set, c))
        {
            if (out_pos + 1 > out_len)
                return -1;
            out_buff[out_pos++] = c;
        }
        else
        {
            if (out_pos + 3 > out_len)
                return -1;
            out_buff[out_pos++] = '%';
            out_buff[out_pos++] = hex_encode(c >> 4);
            out_buff[out_pos++] = hex_encode(c);
        }
    }
    // return space used in out buffer
    return out_pos;
}

int percent_encode(char* in_buff, int in_len, char* out_buff, int out_len)
{
    return percent_encode_custom(&liberal_reserved_set, in_buff, in_len, out_buff, out_len);
}

// returns length used in the output buffer.
// returns -1 on error, either due to:
//  - malformed input
//  - unexpected end-of-buffer for input
//  - inexpected end-of-buffer for output 
int percent_decode(char* in_buff, int in_len, char* out_buff, int out_len)
{
    int in_pos, out_pos;
    in_pos = out_pos = 0;
    while (in_pos < in_len)
    {
        if (out_pos >= out_len)
            return -1;

        char c = in_buff[in_pos++];
        if (c == '%')
        {
            if (!(in_pos + 1 < in_len))
                return -1;
            char h_hi = tolower(in_buff[in_pos++]);
            char h_lo = tolower(in_buff[in_pos++]);
            if (!is_hex_char(h_hi) || !is_hex_char(h_lo))
                return -1;
            out_buff[out_pos++] = hex_decode(h_hi, h_lo);
        }
        else
        {
            out_buff[out_pos++] = c;
        }
    }
    return out_pos;
}

/* Tools to produce reserved_set constants
void print_reserved_set(FILE* f, reserved_set* set)
{
  fprintf(f, "Unreserved chars: ");
  for(int i = 0; i < 128; i++)
    {
      if(!is_reserved(set, i))
    {
      fprintf(f, "%c", i);
    }
    }
  fprintf(f, "\n");
}

void print_set_bytes(FILE* f, reserved_set* set)
{
  fprintf(f, "{");
  for(int i = 0; i < 15; i++)
    fprintf(f, "%i,", set->set[i]);
  fprintf(f, "%i}", set->set[15]);
}

void set_conservative_unreserved(reserved_set *set)
{
  char c;
  for(c = 'a'; c <= 'z'; c++)
    {
      mark_unreserved(set, c);
      mark_unreserved(set, toupper(c));
    }

  for(c = '0'; c <= '9'; c++)
    mark_unreserved(set, c);

  mark_unreserved(set, '-');
  mark_unreserved(set, '_');
  mark_unreserved(set, '.');
  mark_unreserved(set, '~');
}

void set_liberal_unreserved(reserved_set *set)
{
  mark_unreserved(set, '!');
  mark_unreserved(set, '*');
  mark_unreserved(set, '\'');
  mark_unreserved(set, '(');
  mark_unreserved(set, ')');
  mark_unreserved(set, ';');
  mark_unreserved(set, ':');
  mark_unreserved(set, '@');
  mark_unreserved(set, '&');
  mark_unreserved(set, '=');
  mark_unreserved(set, '+');
  mark_unreserved(set, '$');
  mark_unreserved(set, ',');
  mark_unreserved(set, '/');
  mark_unreserved(set, '?');
  mark_unreserved(set, '#');
  mark_unreserved(set, '[');
  mark_unreserved(set, ']');
}
*/