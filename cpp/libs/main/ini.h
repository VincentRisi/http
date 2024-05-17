#ifndef _INI_H_
#define _INI_H_ 

// ***********************************************************************
//                                                                        
//   System : NRW                                                         
//                                                                        
//   File   : inistr.h                                                    
//                                                                        
//   Description : class for loading of parameters from an 'ini' file     
//                                                                        
//   Author : S. Shemesh                                                  
//                                                                        
//   Date   : Fri 3 Nov 2000                                              
//                                                                        
// ***********************************************************************

#include <stdio.h>

#include "xcept.h"
#include "tstring.h"
#include "tbuffer.h"

const int MEM_SIZE = 10240;

//--------------------------------------------------

class TIni
{
public:
  TIni(char * aInifile);                     // constructor
  ~TIni();                                   // destructor
  bool QueryValueDefault(TString aKeyword, TString & aValue, TString aSection);
  // following functions throw exceptions if keyword is not found
  void QueryValue(pchar aKeyword,   // get a value back for keyword
                  TString & aValue,    // and section - value as string
    pchar aDefault = 0,             // default value if not found
    pchar aSection = 0);            
  void QueryValue(pchar aKeyword,   // get a value back for keyword
    pchar &aValue,                  // and section - value as string
    pchar aDefault = 0,             // default value if not found
    pchar aSection = 0);            
  void QueryValue(pchar aKeyword,   // get a value back for keyword
                  int & aValue,        // and section - value as int
                  int * aDefault = 0,    // default value if not found
    pchar aSection = 0);            
  void QueryValue(pchar aKeyword,   // get a value back for keyword
                  long & aValue,       // and section - value as long
                  long * aDefault = 0,       // default value if not found
    pchar aSection = 0);            
  void QueryValue(pchar aKeyword,   // get a value back for keyword
                  double & aValue,     // and section - value as double
                  double * aDefault = 0,     // default value if not found
    pchar  aSection = 0);           
  void QueryValue(pchar aKeyword,   // get a value back for keyword
                  bool & aValue,       // and section - value as bool
    pchar aTrueValue,               // true value else false
                  bool * aDefault = 0,       // default value if not found
    pchar aSection = 0);
  void SetSection(pchar aSection);                // set the section
  TString  QueryNextKeyword(TString & aKeyword);  // get next keyword / value
  TString QueryNextSection(void);                // get next section
private:
  TIni(); // default constructor not used - causes link error if used
  TString  section;
  TString  keyword;
  short   iniok;
  char    inifile[500];
  FILE    *hf;
  int     BufferRetained;
  char    RetainBuffer[500];
  char    *pReadBuff;
  size_t  cbBuff;
  short   NextByte;
  void  Retain(char * aData);
  short InitLoad(char * aFileName);
  int   ReadData(FILE * aHf, char * aData);
  int   ReadConfig(FILE *aHf, char * aData);
  int   GetNextSection(TString & aSection);
  int   GetNextKeyword(TString & aKeyword, TString & aParam);
  void  TerminateLoad(void);
  int  InitSave(char * aFileName);
  void WriteKeyword(char * aKeyword, char * aValue);
  void WriteSection(char * aSection);
  void TerminateSave(void);
  void GetEnv(TString &aValue);
};


class XIni : public XCept
{
public:
  enum eError { errOpenFailed = 1,
                errNoSection = 2,
                errNoKeyword = 3,
                errStatFailed =4,
                errStatNotRegular =5,
                errNoValue = 6
               };
  XIni(pchar aFname, int aLine, int aError, pchar aMsg);
  XIni(pchar aFname, int aLine, eError aError, pchar aIniFile, pchar aSection = NULL, char * aKeyword = NULL);
  XIni(const XIni& x);
private:
  void Construct(eError aError,char *aSection=NULL, char *aKeyword=NULL, char *aFileName=NULL);
  char   m_keyword[50],
         m_section[50];
};

inline XIni::XIni(const XIni& x) : XCept(x) {}

#if defined(M_GNU)
		#define XINI(err, Msg) \
  XINI(__FILE__, __LINE__, XINI::err, Msg)
		#define XINIF(err, Section, Keyword) \
		  XINI(__FILE__, __LINE__, XINI::err, inifile, (char *)Section, (char *)Keyword)
		#define XINIS(err, Section) \
		  XINI(__FILE__, __LINE__, XINI::err, inifile, (char *)Section)
		#define XINIFILE(err, file) \
		  XINI(__FILE__, __LINE__, XINI::err, file)
#else
    #define XINI(err, Msg) \
      XIni(__FILE__, __LINE__, XIni::##err, Msg)
    #define XINIF(err, Section, Keyword) \
      XIni(__FILE__, __LINE__, XIni::##err, inifile, (char *)Section, (char *)Keyword)
    #define XINIS(err, Section) \
      XIni(__FILE__, __LINE__, XIni::##err, inifile, (char *)Section)
    #define XINIFILE(err, file) \
      XIni(__FILE__, __LINE__, XIni::##err, file)
#endif  
#endif

