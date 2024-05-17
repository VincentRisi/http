// **********************************************************************
//                                                                       
//   System : NRW                                                        
//                                                                       
//   File   : inistr.Cpp                                                 
//                                                                       
//   Description : class for loading of parameters from an 'ini' file    
//                                                                       
//   Author : S. Shemesh                                                 
//                                                                       
//   Date   : Fri 3 Nov 2000                                             
//                                                                       
//   Version : 1.0.0                                                     
//                                                                       
//   Change History :                                                    
//                                                                       
// **********************************************************************

#include "ini.h"

#if !defined(M_GNU)
#include <io.h>
#endif
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _S_IFMT
#define _S_IFMT S_IFMT
#endif
#ifndef _S_IFREG
#define _S_IFREG S_IFREG
#endif

#define XINI_NAME "XINI"

XIni::XIni(pchar aFname, int aLine, int aError, pchar aMsg)
	: XCept(aFname, aLine, XINI_NAME, aError)
{
	osErr << aFname << "(" << aLine << ") ";
	if (aError)
		osErr << "Erc(" << aError << ") ";
	if (aMsg)
		osErr << aMsg << endl;
	osErr << ends;
}

// exception class for tINI
XIni::XIni(pchar aFname, int aLine, eError aError, pchar aIniFile, char* section, char* keyword) :
	XCept(aFname, aLine, XINI_NAME, aError)
{
	Construct(aError, section, keyword, aIniFile);
	//   osErr << endl << "INI file - " << aIniFile;
};

void XIni::Construct(eError aError, char* section, char* keyword, char* aIniFile)
{
	switch (aError)
	{
	case errOpenFailed:
		sprintf(errorStr, "Open INI file (%s) Faild", aIniFile);
		break;
	case errNoSection:
		sprintf(errorStr, "no matching section %s", section);
		break;
	case errNoKeyword:
		sprintf(errorStr, "no matching keyword %s in section %s", keyword, section);
		break;
	case errStatFailed:
		sprintf(errorStr, "unable to get file statistics for %s", aIniFile);
		break;
	case errStatNotRegular:
		sprintf(errorStr, "INI file %s must be a regular file", aIniFile);
		break;
	case errNoValue:
		sprintf(errorStr, "   No Value for keyword %s in section %s", keyword, section);
		break;
	}
}

TIni::TIni(char* aIniFile)
{
	hf = 0;
	pReadBuff = 0;
	BufferRetained = 0;
	NextByte = 0;  
	cbBuff = 0;
	strcpy(inifile, aIniFile);
	iniok = InitLoad(aIniFile); // open file
}

TIni::~TIni()
{
	TerminateLoad();
}

static char EMPTY_STRING[1];

void TIni::SetSection(pchar aSection)
{
	TString nextSection = EMPTY_STRING;
	/* set the section to given value */
	section = aSection;
	/* reset file pointer to begining */
	/* as SetSection cannot make      */
	/* assumptions about file pos     */
	if (fseek(hf, SEEK_SET, 0))
	{
		/* unable to set file pos back to start */
		/* close file and open again            */
		fclose(hf);
		hf = fopen(inifile, "r");
		if (!hf)
		{
			section = TString("Fail To Open Ini File ") + TString(inifile);
			throw XINI(errOpenFailed, (char*)section);
		}
	}
	/* check if section name is wrapped in brackets */
	/* if not wrap it up                            */
	if ((section[(size_t)0] != '[') && (section[section.Length()] != ']'))
	{
		section = TString("[") + aSection + TString("]");
	}
	/* get to the position in file for specified section */
	section.SetCompMode(TString::cmIgnore);        // change compare mode
	nextSection.SetCompMode(TString::cmIgnore);    // change compare mode
	for (;; )
	{
		/* get the next section from ini file */
		if (!GetNextSection(nextSection))
			throw XINIS(errNoSection, section);
		/* if next section in ini file matches the section set then break */
		if (section == nextSection) return;
	}
}

TString TIni::QueryNextSection(void)
{
	TString nextSection = EMPTY_STRING;
	GetNextSection(nextSection);
	return(nextSection);
}

TString TIni::QueryNextKeyword(TString& aKeyword)
{
	TString nextKeyword = EMPTY_STRING;
	TString nextValue = EMPTY_STRING;
	/* initialise aKeyword to nothing */
	aKeyword = EMPTY_STRING;
	/* get next keyword */
	for (;;)
	{
		if (GetNextKeyword(nextKeyword, nextValue))
		{
			aKeyword = nextKeyword;
			return nextValue;
		}
		else
			break;
	}
	/* not found so return empty value */
	return(nextValue);
}

void TIni::QueryValue(pchar aKeyword, TString& aValue, pchar aDefault, pchar aSection)
{
	bool rc = QueryValueDefault(aKeyword, aValue, aSection);
	if (!rc)
	{
		if (aDefault)
		{
			aValue = aDefault;
		}
		throw XINIF(errNoKeyword, section, aKeyword);
	}
}

void TIni::QueryValue(pchar aKeyword, pchar& aValue, pchar aDefault, pchar aSection)
{
	TString Value = aValue;
	bool rc = QueryValueDefault(aKeyword, Value, aSection);
	if (!rc)
	{
		if (aDefault == 0)
		{
			throw XINIF(errNoKeyword, section, aKeyword);
		}
		else
		{
			aValue = strdup(aDefault);
			return;
		}
	}
	if (Value && Value.Length() > 0)
	{
		aValue = strdup((char*)Value);
	}
	else
	{
		if (Value.Length() == 0)
		{
			throw XINIF(errNoValue, section, aKeyword);
		}
	}
}

void TIni::QueryValue(pchar aKeyword, int& aValue, int* aDefault, pchar aSection)
{
	TString Value = EMPTY_STRING;

	/* use QueryValue for a tString */
	bool rc = QueryValueDefault(aKeyword, Value, aSection);

	/* if value was found */
	if (rc)
	{
		/* convert to short */
		pchar tmp = (char*)Value;
		aValue = atoi(tmp);
	}
	else
	{
		if (aDefault)
		{
			aValue = *aDefault;
		}
		else
		{
			throw XINIF(errNoKeyword, section, aKeyword);
		}
	}
}

void TIni::QueryValue(pchar aKeyword, long& aValue, long* aDefault, pchar aSection)
{
	TString Value = EMPTY_STRING;
	/* use QueryValue for a tString */
	bool rc = QueryValueDefault(aKeyword, Value, aSection);

	/* if value was found */
	if (rc)
	{
		/* convert to short */
		aValue = atol((const char*)Value);
	}
	else
	{
		if (aDefault)
		{
			aValue = *aDefault;
		}
		else
		{
			throw XINIF(errNoKeyword, section, aKeyword);
		}
	}
}

void TIni::QueryValue(pchar aKeyword, double& aValue, double* aDefault, pchar aSection)
{
	TString Value = EMPTY_STRING;
	/* use QueryValue for a tString */
	bool rc = QueryValueDefault(aKeyword, Value, aSection);
	/* if value was found */
	if (rc)
	{
		/* convert to short */
		aValue = atof((const char*)Value);
	}
	else
	{
		if (aDefault)
		{
			aValue = *aDefault;
		}
		else
		{
			throw XINIF(errNoKeyword, section, aKeyword);
		}
	}
}

void TIni::QueryValue(pchar aKeyword, bool& aValue, pchar aTrueValue, bool* aDefault, pchar aSection)
{
	TString Value = EMPTY_STRING;
	TString TrueValue = aTrueValue;
	Value.SetCompMode(TString::cmIgnore);        // change compare mode
	TrueValue.SetCompMode(TString::cmIgnore);    // change compare mode
	bool rc = QueryValueDefault(aKeyword, Value, aSection);
	if (rc == 0)
	{
		if (aDefault)
		{
			aValue = *aDefault;
		}
		else
		{
			throw XINIF(errNoKeyword, section, aKeyword);
		}
		return;
	}

	if (Value == TrueValue)
	{
		aValue = true;
	}
	else
	{
		aValue = false;
	}
}

void TIni::GetEnv(TString& aValue)
{
	TBChar work1, work2, work3;
	work1.set(aValue);
	char* p = work1.data, * q, * r;
	while (p[0] && (q = strstr(p, "$(")) != 0)
	{
		r = strstr(q, ")");
		int n = (int)(r - q - 2);
		if (r == 0 || n <= 0)
			break;
		if (p != q)
			work2.append(p, q - p);
		work3.set(q + 2, n);
		char* e = getenv(work3);
		if (e)
			work2.append(e);
		p = r + 1;
	}
	if (p == work1.data)
		return;
	work2.append(p);
	aValue = work2.data;
}

bool TIni::QueryValueDefault(TString aKeyword, TString& aValue, TString aSection)
{
	TString nextSection = EMPTY_STRING;
	TString nextKeyword = EMPTY_STRING;
	TString nextValue = EMPTY_STRING;

	// if section was not specfified then default to current
	if (!aSection.Length())
	{
		nextSection = section;
	}
	else
	{
		/* set to section specified */
		nextSection = aSection;
	}
	// start at the top of the section again
	SetSection(nextSection);
	aKeyword.SetCompMode(TString::cmIgnore);     // change compare mode
	nextValue.SetCompMode(TString::cmIgnore);    // change compare mode
	// get to keyword and value
	for (;; )
	{
		if (GetNextKeyword(nextKeyword, nextValue))
		{
			if (aKeyword == nextKeyword)
			{
				/* matched keyword */
				aValue = nextValue;
				if (aValue.Length() == 0)
				{
					return true;
				}
				GetEnv(aValue);
				return true;
			}
		}
		else // end of ini section keyword not matched
			break;
	}
	//   aValue = aDefault;
	//   GetEnv(aValue);
	return false;
}

int TIni::ReadData(FILE* aHf, char* aData)
{
	if ((size_t)NextByte < cbBuff)
	{
		aData[0] = pReadBuff[NextByte];
		NextByte++;
		return(1);
	}
	NextByte = 0;
	cbBuff = 0;
	cbBuff = fread(pReadBuff, 1, MEM_SIZE, aHf);
	if (!cbBuff)
	{
		return(0);
	}
	aData[0] = pReadBuff[NextByte];
	NextByte++;
	return(1);
}

int TIni::ReadConfig(FILE* aHf, char* aData)
{
	short i = 0;
	char  Jotter[500];
	if (BufferRetained)
	{
		strcpy(aData, RetainBuffer);
		BufferRetained = 0;
		return(1);
	}
	for (;; )
	{
		if (!ReadData(aHf, &Jotter[i])) break;
		if (Jotter[i] == 0x0A)
		{
			if (i < 2)
			{
				i = 0;
				continue;
			}
			Jotter[i] = 0x00;
			strcpy(aData, Jotter);
			return(1);
		}
		i++;
		//      if(i > 80) i = 0;     // M.Barnard 30 Dec 97 See Comment
		if (i > 500) return(1);
	}
	return(0);
}

void TIni::Retain(char* aData)
{
	BufferRetained = 1;
	strcpy(RetainBuffer, aData);
}

short TIni::InitLoad(char* aFileName)
{
	struct stat buf;
	// S Sena Check that path refers to a regular text file 15/1/95
	if (stat(aFileName, &buf) < 0)
		throw XINIFILE(errStatFailed, aFileName);
	else
		if ((buf.st_mode & _S_IFMT) != _S_IFREG)
			throw XINIFILE(errStatNotRegular, aFileName);
	hf = fopen(aFileName, "rt");
	if (!hf) {
		throw XINIFILE(errOpenFailed, aFileName);
	}
	/* set our private file name to the param */
	strcpy(inifile, aFileName);
	pReadBuff = (char*)malloc(MEM_SIZE);
	if (!pReadBuff) return 1;
	memset(pReadBuff, 0, MEM_SIZE);
	cbBuff = 0;
	NextByte = 0;
	return 0;
}

void TIni::TerminateLoad(void)
{
	if (hf) fclose(hf);
	if (pReadBuff) free(pReadBuff);
}


int TIni::GetNextSection(TString& aSection)
{
	char data[500];
	for (;; )
	{
		if (!ReadConfig(hf, data))
		{
			return(0);
		}
		if (data[0] != '[') continue;
		/*                                        */
		/* We have found the next Section. Return */
		/* the section details.                   */
		/*                                        */
		aSection = data;
		return(1);
	}
}

int TIni::GetNextKeyword(TString& aKeyword, TString& aParam) {

	char  data[500];
	char  keyword[500];
	char  param[500];
	short i = 0, bound = 0;
	for (;; )
	{
		if (!ReadConfig(hf, data)) return(0);
		if (data[0] == '[')
		{
			/*                                 */
			/* We have hit the next section.   */
			/* retain the current buffer for   */
			/* the next read.                  */
			/*                                 */
			Retain(data);
			return(0);
		}
		if (data[0] != '{') continue;
		/*                                        */
		/* We have found the next keyword. Return */
		/* the keyword and the parameter.         */
		/*                                        */
		bound = -1;
		for (i = 1; i < (short)strlen(data); i++)
		{
			if (data[i] != '}') continue;
			bound = i;
			break;
		}
		if (bound == -1)
		{
			/*                                    */
			/* Invalid Syntax, ignore the line    */
			/*                                    */
			continue;
		}
		/*                                       */
		/* Extract the Keyword                   */
		/*                                       */
		memcpy(keyword, data, bound + 1);
		keyword[bound + 1] = 0x00;
		/*                                       */
		/* Extract the Parameter                 */
		/*                                       */
		memcpy(param, &data[bound + 1], strlen(data) - (bound + 1));
		param[strlen(data) - (bound + 1)] = 0x00;
		aKeyword = keyword;
		aParam = param;
		return(1);
	}
}

/**************************************************************************/
/*                                                                        */
/* Saving Procedures.                                                     */
/*                                                                        */
/**************************************************************************/

int TIni::InitSave(char* aFileName)
{
	hf = fopen(aFileName, "w");
	if (!hf)
	{
		/* stuffed */
		return(0);
	}
	return(1);
}

void TIni::WriteKeyword(char* aKeyword, char* aValue)
{
	char  newLine[5];
	newLine[0] = 0x0D;
	newLine[1] = 0x0A;
	newLine[2] = 0x00;
	fwrite(aKeyword, strlen(aKeyword), 1, hf);
	fwrite(aValue, strlen(aValue), 1, hf);
	fwrite(newLine, strlen(newLine), 1, hf);
}

void TIni::WriteSection(char* aSection)
{
	char  newLine[5];
	newLine[0] = 0x0D;
	newLine[1] = 0x0A;
	newLine[2] = 0x00;
	fwrite(newLine, strlen(newLine), 1, hf);
	fwrite(aSection, strlen(aSection), 1, hf);
	fwrite(newLine, strlen(newLine), 1, hf);
	fwrite(newLine, strlen(newLine), 1, hf);
}

void TIni::TerminateSave(void)
{
	fclose(hf);
}
