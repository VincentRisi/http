#include "z85.h"

const char* z85code = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/*?&<>()[]{}@%$#";

void z85_encode(TBUChar &buff, unsigned char *input, int data_size)
{
    buff.clear();
    for (int i = 0; i < data_size; i += 4)
    {
        int n = i;
        unsigned int as = input[n++] << 24;
        if (n < data_size) as += input[n++] << 16;
        if (n < data_size) as += input[n++] << 8;
        if (n < data_size) as += input[n++];
        unsigned char outwork[5];
        outwork[4] = z85code[as % 85]; as /= 85;
        outwork[3] = z85code[as % 85]; as /= 85;
        outwork[2] = z85code[as % 85]; as /= 85;
        outwork[1] = z85code[as % 85]; as /= 85;
        outwork[0] = z85code[as];
        buff.append(outwork, 5);
    }
}

void z85_decode(TBUChar& buff, unsigned char* input, int data_size)
{
    buff.clear();
    char k[2] = ".";
    for (int i = 0; i < data_size; i += 5)
    {
        unsigned char inwork[5] = {0, 0, 0, 0, 0};
        int n = i;
        k[0] = input[n++]; inwork[0] = (unsigned char)strcspn(z85code, k);
        if (n < data_size) k[0] = input[n++]; inwork[1] = (unsigned char)strcspn(z85code, k);
        if (n < data_size) k[0] = input[n++]; inwork[2] = (unsigned char)strcspn(z85code, k);
        if (n < data_size) k[0] = input[n++]; inwork[3] = (unsigned char)strcspn(z85code, k);
        if (n < data_size) k[0] = input[n++]; inwork[4] = (unsigned char)strcspn(z85code, k);
        unsigned int as = (((((((inwork[0] * 85) + inwork[1]) * 85) + inwork[2]) * 85) + inwork[3]) * 85) + inwork[4];
        unsigned char outwork[4];
        outwork[0] = (as & 0xFF000000) >> 24;
        outwork[1] = (as & 0x00FF0000) >> 16;
        outwork[2] = (as & 0x0000FF00) >> 8;
        outwork[3] = (as & 0x000000FF);
        buff.append(outwork, 4);
    }
}

