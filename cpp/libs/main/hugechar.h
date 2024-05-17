#ifndef _HUGECHAR_H_
#define _HUGECHAR_H_
class tHugeCHAR
{
public:
    int32 length;
    char *data;
    tHugeCHAR();
    ~tHugeCHAR();
    operator char*() { return data; }
    tHugeCHAR& operator=(const tHugeCHAR& hugeCHAR);
    tHugeCHAR& operator=(const char* inData);
};
#endif
