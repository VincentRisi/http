#ifndef _ASCII_85_H
#define _ASCII_85_H

#include "machine.h"
#include "tbuffer.h"

void ascii85_encode(TBUChar& buff, unsigned char* input, int data_size);
void ascii85_decode(TBUChar& buff, unsigned char* input, int data_size);

// Z85 would possibly be best for json based comms.

// Four bytes can represent 2^32 = 4,294,967,296 possible values.
// Five radix - 85 digits provide 85^5 = 4,437,053,125 possible values, 
// enough to provide for a unique representation for each possible 32 - bit value.
// Because five radix - 84 digits only provide 84^5 = 4,182,119,424 representable values,
// 85 is the minimum possible integral base that will represent four bytes in five characters,
// hence its choice.

// When encoding, each group of 4 bytes is taken as a 32 - bit binary number, 
// most significant byte first(Ascii85 uses a big - endian convention).
// This is converted, by repeatedly dividing by 85 and taking the remainder, 
// into 5 radix - 85 digits.Then each digit(again, most significant first) is 
// encoded as an ASCII printable character by adding 33 to it, giving the
// ASCII characters 33 ("!") through 117 ("u").

// Because all - zero data is quite common, an exception is made for the sake
// of data compression, and an all - zero group is encoded as a single
// character "z" instead of "!!!!!".

// Also a group of 4 spaces is encoded as a single character "y" instead of 
// some common 5 bytes, which is moot.

// A way to deal with the last 4 bytes being being padded with 1 2 or 3 extra zero bytes
// is to write the encoded with a extra starting '0' '1' '2' or '3' to indicate
// number of zeroes being padded. Then for decoding do not forget to skip this extra
// byte.

#endif