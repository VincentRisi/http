#include "ascii85.h"

void ascii85_encode(TBUChar& buff, unsigned char* input, int data_size)
{
    buff.clear();
    for (int i = 0; i < data_size; i += 4)
    {
        const char* p = (char*)(input + i);
        if (data_size >= 4 && strncmp(p, "    ", 4) == 0)
        {
            buff.append((unsigned char*)"y");
            continue;
        }
        const unsigned int* v = (unsigned int*)(input + i);
        if (data_size >= 4 && *v == 0)
        {
            buff.append((unsigned char*)"z");
            continue;
        }
        int n = i;
        unsigned int as = input[n++] << 24;
        if (n < data_size) as += input[n++] << 16;
        if (n < data_size) as += input[n++] << 8;
        if (n < data_size) as += input[n++];
        unsigned char outwork[5];
        outwork[4] = (as % 85)+33; as /= 85;
        outwork[3] = (as % 85)+33; as /= 85;
        outwork[2] = (as % 85)+33; as /= 85;
        outwork[1] = (as % 85)+33; as /= 85;
        outwork[0] = as + 33;
        buff.append(outwork, 5);
    }
}

void ascii85_decode(TBUChar& buff, unsigned char* input, int data_size)
{
    buff.clear();
    for (int i = 0; i < data_size; i++)
    {
        const char* p = (char*)(input + i);
        unsigned char outwork[4];
        if (*p == 'z')
        {
            memset(outwork, 0, sizeof(outwork));
            buff.append(outwork, 4);
            continue;
        }
        if (*p == 'y')
        {
            memset(outwork, ' ', sizeof(outwork));
            buff.append(outwork, 4);
            continue;
        }
        unsigned char inwork[5];
        memset(inwork, 0, sizeof(inwork));
        int n = i;
        i += 4;
        inwork[0] = input[n++] - 33;
        if (n < data_size) inwork[1] = input[n++] - 33;
        if (n < data_size) inwork[2] = input[n++] - 33;
        if (n < data_size) inwork[3] = input[n++] - 33;
        if (n < data_size) inwork[4] = input[n++] - 33;
        unsigned int as = (((((((inwork[0] * 85) + inwork[1]) * 85) + inwork[2]) * 85) + inwork[3]) * 85) + inwork[4];
        outwork[0] = (as & 0xFF000000) >> 24;
        outwork[1] = (as & 0x00FF0000) >> 16;
        outwork[2] = (as & 0x0000FF00) >> 8;
        outwork[3] = (as & 0x000000FF);
        buff.append(outwork, 4);
    }
}
