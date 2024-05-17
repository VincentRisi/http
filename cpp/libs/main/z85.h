#ifndef _Z_85_H
#define _Z_85_H

#include "machine.h"
#include "tbuffer.h"

void z85_encode(TBUChar &buff, unsigned char* input, int data_size);
void z85_decode(TBUChar &buff, unsigned char *input, int data_size);

// Z85, the ZeroMQ base-85 encoding algorithm, is a string-safe variant of base85.
// By avoiding the double-quote, single-quote, and backslash characters,
// Z85-encoded data can be better embedded in command-line interpreter strings.
// Z85 uses the characters 0..9 a..z A..Z . - : + = ^ ! / * ? & < > ( ) [ ] { } @ % $ #
// See ascii85.h for discussion why base-85.

// A way to deal with the last 4 bytes being being padded with 1 2 or 3 extra zero bytes
// is to write the encoded with a extra starting '0' '1' '2' or '3' to indicate
// number of zeroes being padded. Then for decoding do not forget to skip this extra
// byte.

// Z85 would possibly be best for json based comms.

#endif