/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2005/10/01
 Name   : Collection
 Descr  : A vector type template class that is class safe
==============================================================================*/

#ifndef _COLLECTION_H_
#define _COLLECTION_H_

#include "xcept.h"

#include <memory.h>
#include <stddef.h>
#include <stdlib.h>

class XCollection : public XCept
{
   public:
      XCollection(const char *file, const int line, const unsigned size) 
      : XCept(file, line, "xCollection", -1)
      {
         osErr << "Error allocating memory for size [" << size << "]" << std::endl << std::ends;
      }

      XCollection(const XCollection &x) : XCept(x) { }
};


#ifdef Swap
#undef Swap
#endif

template <class type> class Collection
{
   public:

      type            **_List;

                      Collection(void);
      virtual        ~Collection(void);

      type           *Append(type *rec = NULL, const unsigned incVal = 16);
      void            Swap(const unsigned index1, const unsigned index2);
      void            Remove(const unsigned index);
      void            Remove(void);

      void            Reset(void);  // deletes all elements in the list
      void            Purge(void);  // totally removes all allocated elements

      unsigned        Count(void) const;
      unsigned        ArrCount(void) const;
      unsigned        MemUsed(void) const;

      void            Qsort(int (*sortFunc)(const type **Rec, const type **Rec2));
      type           *Bsearch(const type *SearchRec, int (*sortFunc)(const type **Rec, const type **Rec2));


      type           &Last(void) const;

      type           &operator[](const unsigned index) const;

      type           *Get(const unsigned index) const;

   private :

      unsigned        _ListArrCnt;   // number of array elements
      unsigned        _ListCnt;      // number of used array elements
};


/*------------------------------------------------------------------------------
   Collection Constructor
------------------------------------------------------------------------------*/
template <class type> 
Collection<type>::Collection(void)
{
   _List       = NULL;
   _ListArrCnt = 
   _ListCnt    = 0;
}


/*------------------------------------------------------------------------------
   Collection Destructor
------------------------------------------------------------------------------*/
template <class type> 
Collection<type>::~Collection(void)
{
   Purge();
}


/*------------------------------------------------------------------------------
   Returns the number of records in use by the collection   
------------------------------------------------------------------------------*/
template <class type> 
unsigned Collection<type>::Count(void) const
{
   return _ListCnt;
}


/*------------------------------------------------------------------------------
   Returns the size of the collection
------------------------------------------------------------------------------*/
template <class type> 
unsigned Collection<type>::ArrCount(void) const
{
   return _ListArrCnt;
}


/*------------------------------------------------------------------------------
   Returns the size of the total memory used by the collections
------------------------------------------------------------------------------*/
template <class type> 
unsigned Collection<type>::MemUsed(void) const
{
   return _ListArrCnt * (sizeof(type *) + sizeof(type));
}


/*------------------------------------------------------------------------------
   Deletes all the objects in the collection, memory is saved for re use
------------------------------------------------------------------------------*/
template <class type> 
void Collection<type>::Reset(void)
{
   for (unsigned index = 0; index < _ListCnt; index++)
      if (_List[index])
      {
         delete _List[index];
         _List[index] = NULL;
      }

   _ListCnt = 0;
}


/*------------------------------------------------------------------------------
   Completely wacks the object
------------------------------------------------------------------------------*/
template <class type> 
void Collection<type>::Purge(void)
{
   Reset();

   if (_List)
   {
      free(_List);
      _List       = NULL;
      _ListArrCnt = 
      _ListCnt    = 0;
   }
}


/*------------------------------------------------------------------------------
   Adds a new object to the collection, eg   ColOjb.Append(new thing());
------------------------------------------------------------------------------*/
template <class type> 
type *Collection<type>::Append(type *rec, const unsigned incVal)
{
   if (_ListCnt == _ListArrCnt)
   {
      type **memPtr = _List;

      _ListArrCnt += incVal;
      if ((_List = (type**) realloc(_List, sizeof(type*) * _ListArrCnt)) == NULL)
      {
         _List = memPtr;
         throw XCollection(__FILE__, __LINE__, sizeof(type*) * _ListArrCnt);
      }
   }

   if (rec)
      _List[_ListCnt++] = rec;
   else
      _List[_ListCnt++] = NULL;

   return rec;
}


/*------------------------------------------------------------------------------
   Swaps the position of two objects
------------------------------------------------------------------------------*/
template <class type> 
void Collection<type>::Swap(const unsigned index1, const unsigned index2)
{
   type *tmp = _List[index1];

   _List[index1] = _List[index2];
   _List[index2] = tmp;
}


/*------------------------------------------------------------------------------
   Removes an object from the back of the list
------------------------------------------------------------------------------*/
template <class type> 
void Collection<type>::Remove(void)
{
   Remove(_ListCnt - 1);
}


/*------------------------------------------------------------------------------
   Removes an object at a specific index
------------------------------------------------------------------------------*/
template <class type> 
void Collection<type>::Remove(const unsigned index)
{
   delete _List[index];
   _List[index] = NULL;

   if (index == _ListCnt - 1)
   {
      _ListCnt--;
      return;
   }

   memcpy(&_List[index], &_List[index + 1], sizeof(type*) * (_ListCnt - index - 1));
   _ListCnt--;
}


/*------------------------------------------------------------------------------
   [] operator   
------------------------------------------------------------------------------*/
template <class type> 
type &Collection<type>::operator[](const unsigned index) const
{
   return *_List[index];
}


/*------------------------------------------------------------------------------
   Returns the last object in the list
------------------------------------------------------------------------------*/
template <class type> 
type &Collection<type>::Last(void) const
{
   return *_List[_ListCnt - 1];
}


/*------------------------------------------------------------------------------
   Returns a pointer to an object at a specific position
------------------------------------------------------------------------------*/
template <class type> 
type *Collection<type>::Get(const unsigned index) const
{
   return _List[index];
}


/*------------------------------------------------------------------------------
   Qsort
------------------------------------------------------------------------------*/
template <class type>
void Collection<type>::Qsort(int (*sortFunc)(const type **Rec, const type **Rec2))
{
  qsort(_List, 
         _ListCnt, 
         sizeof(type *), 
         (int (*)(const void *, const void *)) sortFunc);
}


/*------------------------------------------------------------------------------
   Bsearch
------------------------------------------------------------------------------*/
template <class type> 
type *Collection<type>::Bsearch(const type *SearchRec, 
                                int (*sortFunc)(const type **Rec, const type **Rec2))
{
   type **result = (type **) bsearch(&SearchRec,
                                     _List,
                                     _ListCnt,
                                     sizeof(type *),
                                     (int (*)(const void *, const void *)) sortFunc);

   if (result)
      return *result;

   return 0;
}

#endif
