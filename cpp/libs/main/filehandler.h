#ifndef __FILE_HANDLE_H_
#define __FILE_HANDLE_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(__FILE_HANDLE_H_);

#include "xcept.h"
#include "bstring.h"
#include "tbuffer.h"

/*-----------------------------------
   FileHandler Excetion
-----------------------------------*/
class xFileHandler : public xCept
{
   public:
      xFileHandler(const char *file, const int line, const char *fmt, ...);

      xFileHandler(const xFileHandler &x) : xCept(x) { }
};


/*-------------------------------------
  FileHandler
-------------------------------------*/
class FileHandler
{
   public:

      typedef enum
      {
         Binary,
         Text
      } Mode;

                        FileHandler(void);
                       ~FileHandler(void);

     void               Open(const char *fileName, 
                             const Mode  mode);

     void               Create(const char  *fileName, 
                               const Mode   mode);

     unsigned           GetLength(void);

     void               Writef(const char *fmt, ...);

     void               Write(const char *str);

     bool               Read(void *dest, const unsigned len, const bool throwOnError = true);

     bool               ReadLine(char *dest, const int len);

     bool               ReadLine(bstring &dest);

     void               Write(const void *source, const unsigned len);

     void               Close(const bool checkForCloseError = true);

     FILE              *GetFilePointer(void) const;

     void               AssignFilePointer(FILE *fptr, const char *fileName);

     const bstring     &FileName() const;

     /*-------------------------------------
        static functions
     -------------------------------------*/
     static unsigned    GetLength(const char *fileName);

     static void        WriteStrToFile(const char *fileName, 
                                       const char *str);

     static char       *ReadFile(const char   *fileName,
                                 TBChar       *jotter);

     /*-----------------------------------
        template read and writes
     -----------------------------------*/
     template <class type>
     void tRead(type &t)
     {
        Read(&t, sizeof(t));
     }

     template <class type>
     void tWrite(const type &t)
     {
        Write(&t, sizeof(t));
     }

  private:

     FILE             *_fptr;
     bstring           _fileName;
};


#endif
