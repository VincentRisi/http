#ifndef _HANDLES_H_
#define _HANDLES_H_

template <class T, int range = 8192, int size = 32> 
class tHandle
{
private:

  struct tList
  {
    T   element;
    int used;
  } list[size];

public:
  tHandle()
  {
    memset(list, 0, sizeof(list));
  }

  int Create(T element)
  {
    for (int i=0; i < size; i++)
      if (list[i].used == 0)
      {
        list[i].element = element;
        list[i].used = 1;
        return long(i)+range;
      }
    return -1;
  }

  T Use(long handle)
  {
    int i = handle-range;
    if (i < 0 || i >= size)
      return 0;
    if (list[i].used == 1)
      return list[i].element;
    return 0;
  }

  int Release(long handle, int doFree=1)
  {
    int i = handle-range;
    if (i < 0 || i >= size)
      return -1;
    if (list[i].used == 1)
    {
      if (list[i].element != 0 && doFree == 1)
        delete list[i].element;
      list[i].element = 0;
      list[i].used = 0;
      return 0;
    }
    return -1;
  }
};

#endif

