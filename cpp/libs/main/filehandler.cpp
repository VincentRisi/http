/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2004/06/01
 Name   : FileHandler
 Descr  : A common file handling class
==============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "filehandler.h"

#ifdef M_STDLIB
using namespace std;
#endif

/*------------------------------------------------------------------------------
   xFileHandler Constructor
------------------------------------------------------------------------------*/
xFileHandler::xFileHandler(const char *file, const int line, const char *fmt, ...)
             :xCept(file, line, "xFileHandler", -1)
{
   va_list ap;
   char    jotter[1024];

   va_start(ap, fmt);

   vsnprintf(jotter, sizeof(jotter) - 1, fmt, ap);

   va_end(ap);

   osErr << jotter << std::endl << ends;
}


/*----------------------------------------------------------------------------
   constructor
----------------------------------------------------------------------------*/
FileHandler::FileHandler(void)
{
   _fptr     = NULL;
   _fileName = "";
}


/*----------------------------------------------------------------------------
   destructor
----------------------------------------------------------------------------*/
FileHandler::~FileHandler(void)
{
   Close(false);
}


/*----------------------------------------------------------------------------
   Opens a file for reading
----------------------------------------------------------------------------*/
void FileHandler::Open(const char *fileName, 
                       const Mode  openMeth)
{
   Close();

   if (!fileName || !fileName[0])
      throw xFileHandler(__FILE__, __LINE__, "Null filename specified");

   _fileName = fileName;

   if ((_fptr = fopen(fileName, openMeth == Binary ? "rb" : "rt")) == NULL)
      throw xFileHandler(__FILE__, __LINE__, "Error opening file (%s)", _fileName.c_str());
}


/*----------------------------------------------------------------------------
   Opens a file for writting
----------------------------------------------------------------------------*/
void FileHandler::Create(const char *fileName, 
                         const Mode  openMeth)
{
   Close();

   if (!fileName || !fileName[0])
      throw xFileHandler(__FILE__, __LINE__, "Null filename specified");

   _fileName = fileName;

   if ((_fptr = fopen(fileName, openMeth == Binary ? "wb" : "wt")) == NULL)
      throw xFileHandler(__FILE__, __LINE__, "Error creating file (%s)", _fileName.c_str());

   // set buffer size
   if (setvbuf(_fptr, NULL, _IOFBF, 4096))  
   {
      Close(false);
      throw xFileHandler(__FILE__, __LINE__, "Error error allocating memory (%s)", _fileName.c_str());
   }
}


/*----------------------------------------------------------------------------
   Closes the open file pointer
----------------------------------------------------------------------------*/
void FileHandler::Close(const bool checkForCloseError)
{
   if (_fptr)
   {
      fflush(_fptr);

      if (fclose(_fptr) == EOF)
      {
         _fptr = NULL;

         if (checkForCloseError)
            throw xFileHandler(__FILE__, __LINE__, "Error closing file (%s)", _fileName.c_str());
      }

      _fptr     = NULL;
      _fileName = "";
   }
}


/*----------------------------------------------------------------------------
   returns the file pointer
----------------------------------------------------------------------------*/
FILE *FileHandler::GetFilePointer(void) const
{
   return _fptr;
}


/*----------------------------------------------------------------------------
   returns the lenght of the currently open file, dont use this function
      when you have opened the file pointer for write
----------------------------------------------------------------------------*/
unsigned FileHandler::GetLength(void)
{
   long len;

   if (fseek(_fptr, 0L, SEEK_END) == -1 ||
       (len = ftell(_fptr))       == -1 ||
       fseek(_fptr, 0L, SEEK_SET) == -1)
      throw xFileHandler(__FILE__, __LINE__, "Error accessing file (%s)", _fileName.c_str());

   return (unsigned) len;
}


/*----------------------------------------------------------------------------
   prints a formatted string the open file
----------------------------------------------------------------------------*/
void FileHandler::Writef(const char *fmt, ...)
{
   va_list ap;

   va_start(ap, fmt);

   if (vfprintf(_fptr, fmt, ap) == EOF)
   {
      va_end(ap);
      throw xFileHandler(__FILE__, __LINE__, "Error writting format to file (%s)", _fileName.c_str());
   }

   va_end(ap);
}


/*----------------------------------------------------------------------------
   writes a straight str to the file
----------------------------------------------------------------------------*/
void FileHandler::Write(const char *str)
{
   if (fwrite(str, strlen(str), 1, _fptr) != 1)
      throw xFileHandler(__FILE__, __LINE__, "Error writting to file (%s)", _fileName.c_str());
}


/*----------------------------------------------------------------------------
   assigns a new file pointer
----------------------------------------------------------------------------*/
void FileHandler::AssignFilePointer(FILE *fptr, const char *fileName)
{
   if (!fileName || !fileName[0])
      throw xFileHandler(__FILE__, __LINE__, "Null filename specified");

   _fptr     = fptr;
   _fileName = fileName;
}


/*----------------------------------------------------------------------------
   writes memory straight to the file
----------------------------------------------------------------------------*/
void FileHandler::Write(const void     *source, 
                        const unsigned  len)
{
   if (!len || !source)
      return;

   if (fwrite(source, len, 1, _fptr) != 1)
      throw xFileHandler(__FILE__, __LINE__, "Error writting to file (%s/%u)", _fileName.c_str(), len);
}


/*------------------------------------------------------------------------------
   reads data from file, throws and error if len is not read   
------------------------------------------------------------------------------*/
bool FileHandler::Read(void *dest, const unsigned len, const bool throwOnError)
{
   if (!len || !dest)
      return false;

   if (fread(dest, len, 1, _fptr) != 1)
   {
      //Added by: Dieter
      //Fixed another one of Nic's fuckups
      if (feof(_fptr))
      {
      //  SLOG->lprintf(eLogDebug, "End of file reached on fread, returning true"); 
        return true;
      }
      
      if (throwOnError)
         throw xFileHandler(__FILE__, __LINE__, "Error reading from file (%s/%u)", _fileName.c_str(), len);

      return false;
   }

   return true;
}


/*------------------------------------------------------------------------------
   reads line, or up to max size, returns numbers of bytes read
------------------------------------------------------------------------------*/
bool FileHandler::ReadLine(char *dest, const int len)
{
   if (!len || !dest)
      return false;

   if (fgets(dest, len, _fptr) == 0)
      return false;

   return true;
}


/*------------------------------------------------------------------------------
   reads a line, maxes out at 4096
------------------------------------------------------------------------------*/
bool FileHandler::ReadLine(bstring &dest)
{
   char jotter[4096];

   if (fgets(jotter, sizeof(jotter) - 1, _fptr) == 0)
      return false;

   dest = jotter;
   
   return true;
}


/*----------------------------------------------------------------------------
   returns the length of the file specified
----------------------------------------------------------------------------*/
unsigned FileHandler::GetLength(const char *fileName)
{
   FileHandler fh;

   fh.Open(fileName, FileHandler::Binary);

   return fh.GetLength();
}


/*----------------------------------------------------------------------------
   writes a string to the specified file
----------------------------------------------------------------------------*/
void FileHandler::WriteStrToFile(const char *fileName, 
                                 const char *str)
{
   FileHandler fh;

   fh.Create(fileName, Text);

   if (str && str[0])
      fh.Write(str);
}


/*----------------------------------------------------------------------------
   loads the file into memory as ascii
----------------------------------------------------------------------------*/
char *FileHandler::ReadFile(const char    *fileName,
                            TBChar        *jotter)
{
   unsigned       fileLen;
   FileHandler    fh;

   fh.Open(fileName, FileHandler::Binary);

   if ((fileLen = fh.GetLength()) == 0L)
      return "";

   jotter->sizeto(fileLen + 1);

   fh.Read(jotter->data, fileLen);

   return jotter->data;
}


/*------------------------------------------------------------------------------
   FileName
------------------------------------------------------------------------------*/
const bstring &FileHandler::FileName() const
{
   return _fileName;
}
