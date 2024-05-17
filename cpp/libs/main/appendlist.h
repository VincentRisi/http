/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2005/04/01
 Name   : AppendList
 Descr  : A vector type template class
==============================================================================*/

#ifndef __APPENDLIST_H_
#define __APPENDLIST_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(__APPENDLIST_H_);

#include "xcept.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>

/*-----------------------------------
   Exception object
-----------------------------------*/
class xAppendList : public xCept
{
   public:
      xAppendList(const char *file, const int line, const unsigned size) 
      : xCept(file, line, "AppendList", -1)
      {
         osErr << "Error allocating memory for size [" << size << "]" << std::endl << std::ends;
      }

      xAppendList(const xAppendList &x) : xCept(x) { }
};


/*-------------------------------------
   Template append list class
-------------------------------------*/
template <class type> class AppendList
{
   public :

      type           *_List;

                      AppendList();
      virtual        ~AppendList();

      void            Append(const type *Rec = NULL, const unsigned IncVal = 16);
      void            Append(const type &Rec, const unsigned IncVal = 16);
      void            Remove(const unsigned index);
      void            Remove();

      void            Reset();  // just resets our index back to zero for re - use
      void            Purge();  // totally removes all allocated elements

      unsigned        Count() const;
      unsigned        ArrCount() const;
      unsigned        MemUsed() const;
      unsigned        RecordSize() const { return sizeof(type); }

      void            Qsort(int (*sortFunc)(const type *Rec, const type *Rec2));
      type           *Bsearch(const type *SearchRec, int (*sortFunc)(const type *Rec, const type *Rec2)) const;

      type           &Last() const;
      type           &operator[](const unsigned index) const;
      type           *Get(const unsigned index) const;

      AppendList<type> &operator = (const AppendList<type> *InType);

   private :

      unsigned        _ArrayCnt;   // number of array elements
      unsigned        _ListCnt;    // number of used array elements
};

/*----------------------------------------------------------------------------
   functions for 'AppendList' template class
----------------------------------------------------------------------------*/
template <class type>
AppendList<type>::AppendList()
{
   _List      = NULL;
   _ArrayCnt  = 
   _ListCnt   = 0;
}


template <class type>
AppendList<type>::~AppendList()
{
   if (_List) 
      free(_List);
}


template <class type>
unsigned AppendList<type>::Count() const
{
   return _ListCnt;
}


template <class type>
unsigned AppendList<type>::ArrCount() const
{
   return _ArrayCnt;
}


template <class type>
unsigned AppendList<type>::MemUsed() const
{
   return _ArrayCnt * sizeof(type);
}


template <class type>
void AppendList<type>::Reset()
{
   _ListCnt = 0;
}


template <class type>
void AppendList<type>::Purge()
{
   if (_List)
   {
      free(_List);
      _List       = NULL;
      _ArrayCnt   = 
      _ListCnt    = 0;
   }
}

template <class type>
void AppendList<type>::Append(const type *Rec, const unsigned IncVal)
{
   if (_ListCnt == _ArrayCnt)
   {
      type *memPtr = _List;

      _ArrayCnt += IncVal;

      if ((_List = (type*) realloc(_List, sizeof(type) * _ArrayCnt)) == NULL)
      {
         _List = memPtr;
         throw xAppendList(__FILE__, __LINE__, sizeof(type) * _ArrayCnt);
      }
   }

   if (Rec)
      _List[_ListCnt++] = *Rec;
   else
      memset(&_List[_ListCnt++], NULL, sizeof(type));
}


template <class type>
void AppendList<type>::Append(const type &Rec, const unsigned IncVal)
{
   Append(&Rec, IncVal);
}


template <class type>
void AppendList<type>::Remove()
{
   Remove(_ListCnt - 1);
}


template <class type>
void AppendList<type>::Remove(const unsigned index)
{
   if (index == _ListCnt - 1)
   {
      _ListCnt--;
      return;
   }

   memcpy(&_List[index], &_List[index + 1], sizeof(type) * (_ListCnt - index - 1));
   _ListCnt--;
}


template <class type> 
type &AppendList<type>::operator[](const unsigned index) const
{
   return _List[index];
}

template <class type> 
type *AppendList<type>::Get(const unsigned index) const
{
   return &_List[index];
}

template <class type> 
type &AppendList<type>::Last() const
{
   return _List[_ListCnt - 1];
}

template <class type> 
AppendList<type> &AppendList<type>::operator = (const AppendList<type> *InType)
{
   Purge();

   if ((_List = (type*) realloc(_List, sizeof(type) * InType->_ArrayCnt)) == NULL)
   {
      throw xAppendList(__FILE__, __LINE__, sizeof(type) * InType->_ArrayCnt);
   }

   _ArrayCnt = InType->_ArrayCnt;
   _ListCnt  = InType->_ListCnt;

   memcpy(_List, InType->_List, sizeof(type) * _ListCnt);

   return *this;
}

template <class type> 
void AppendList<type>::Qsort(int (*sortFunc)(const type *Rec, const type *Rec2))
{
   qsort(_List,
         _ListCnt,
         sizeof(type),
         (int (*)(const void *, const void *)) sortFunc);
}
      
template <class type> 
type *AppendList<type>::Bsearch(const type *SearchRec, 
                                int (*sortFunc)(const type *Rec, const type *Rec2)) const
{
   return (type *) bsearch(SearchRec,
                           _List,
                           _ListCnt,
                           sizeof(type),
                           (int (*)(const void *, const void *)) sortFunc);
}

#endif

