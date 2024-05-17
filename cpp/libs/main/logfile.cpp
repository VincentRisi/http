/////////////////////////////////////////////////////////////////////
// VERSION CONTROL
// ---------------
// Version   | Date       | Description
// ----------+------------+------------------------------------------------
// v1.00.00  | 18/10/1995 | Version control implemented.
// v1.00.01  | 24/10/1995 | Put in #ifdef's to make windows compilable.
// v1.00.02  | 21/11/1995 | Added error handling.
// v1.00.02  | 23/12/1997 | Allowed lprintf to log big messages not only 200 chars M.Barnard
// v1.00.03  | 22/07/1998 | NR - Added Customstring to allow facility to put in
//           |            | logs (ie. by makeing customstring = "[PID]")
//           |            |
/////////////////////////////////////////////////////////////////////

#include "machine.h"
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdarg.h>

#include "xdir.h"
#include "logfile.h"

const char *LevelStr[] = { "DBG", "INF", "WRN", "ERR"};

XLogFileErr::XLogFileErr(const char *aFname, int aLine, int aError, const char *aMsg, const char *aMsg1, const char *aMsg2)
: XCept(aFname, aLine, "LogFile", aError)
{
  osErr << aFname << "(" << aLine << ")";
  if (aError)
    osErr << "Error(" << aError << ")";
  if (aMsg1)
  {
    osErr << std::endl;
    if (aMsg)
      osErr << "1) " << aMsg << std::endl;
    if (aMsg1)
      osErr << "2) " << aMsg1 << std::endl;
    if (aMsg2)
      osErr << "3) " << aMsg2 << std::endl;
  }
  else
    osErr << aMsg << std::endl;
  osErr << std::ends;
}

bool TLogFile::defRotate = true;
bool TLogFile::defDisplay = false;
int  TLogFile::defMaxFileSize = 0;

TLogFile *TLogFile::__logFile__;
TLogFile *TLogFile::singleton(const char *aFileName, eLevel aDoLevel, int aMaxFileSize, bool aDisplay, bool aRotate)
{
  if (__logFile__ == 0)
    __logFile__ = new TLogFile(aFileName, aDoLevel, aMaxFileSize);
  else
  {
    if (__logFile__->FileName.compare(aFileName) != 0)
      __logFile__->Switch(aFileName, aDoLevel, aMaxFileSize);
    else
    {
      __logFile__->doLevel = aDoLevel;
      __logFile__->maxFileSize = aMaxFileSize;
      __logFile__->Display = aDisplay;
      __logFile__->Rotate = aRotate;
    }
  }
  return __logFile__;
}

/*------------------------------------------------------------------------------
destroy singleton logfile
------------------------------------------------------------------------------*/
void SingletonLogFile::Destroy()
{
  if (TLogFile::__logFile__)
  {
    delete TLogFile::__logFile__;
    TLogFile::__logFile__ = 0;
  }
}

/*------------------------------------------------------------------------------
assign singleton logfile
------------------------------------------------------------------------------*/
void SingletonLogFile::Assign(TLogFile *logFile)
{
  if (TLogFile::__logFile__ == logFile)
    return;
  Destroy();
  TLogFile::__logFile__ = logFile;
}

TLogFile::TLogFile()
{
  doLevel = eLogDebug;
  Display = defDisplay;
  maxFileSize = defMaxFileSize;
  historyExtention = "hst";
  Rotate = defRotate;
  CustomString = "";
  fLog = 0;
  PrevDate = 99991231;
  memset(buffer, 0, sizeof(buffer));
}

TLogFile::TLogFile(const char *aFileName, eLevel aDoLevel, int aMaxFileSize, bool aDisplay, bool aRotate)
: doLevel(aDoLevel)
, Display(aDisplay)
, maxFileSize(aMaxFileSize)
, historyExtention("hst")
, Rotate(aRotate)
, CustomString("")
, fLog(0)
, PrevDate(99991231)
{
  memset(buffer, 0, sizeof(buffer));
  if (aFileName && aFileName[0])
    Open(aFileName);
#if defined(M_AIX) || defined(M_GNU)
  sem = semget(ftok(aFileName, 'A'), 1, 0666 | IPC_CREAT);
#endif
}

TLogFile::~TLogFile()
{
  if (fLog)
    fclose(fLog);
  fLog = 0;
}

void TLogFile::Switch(const char *aFileName)
{
  if (fLog) {
      fclose(fLog);
  }
  Open(aFileName);
}

void TLogFile::Switch(const char *aFileName, eLevel aDoLevel, int aMaxFileSize, bool aDisplay, bool aRotate)
{
  doLevel = aDoLevel;
  maxFileSize = aMaxFileSize;
  Display = aDisplay;
  FileName = aFileName;
  Rotate = aRotate;
  Switch(aFileName);
}

void TLogFile::SwitchOnDate()
{
  Open(FileName.c_str());
}

void TLogFile::GetDatedFilename(std::string &aFilename)
{
  time_t Now;
  tm     *NowTm;
  char   currentDate[20];
  time(&Now);
  NowTm = localtime(&Now);
  snprintf(currentDate,
    sizeof(currentDate) - 1,
    "_%04d%02d%02d",
    NowTm->tm_year + 1900,
    NowTm->tm_mon + 1,
    NowTm->tm_mday);
  char work[512];
  strncpy(work, aFilename.c_str(), sizeof(work)-1);
  char* pch = strrchr(work, '.');
  if (pch != 0)
    *pch = 0;
  aFilename = work;
  aFilename.append(currentDate);
  if (pch != 0)
  {
    *pch = '.';
    aFilename.append(pch);
  }
}

void TLogFile::Open(const char *fileName, const char *mode)
{
  char errorStr[4096];
  std::string TBFileName = fileName;
  FileName = fileName;
  if (Rotate)
    GetDatedFilename(TBFileName);
  if (fLog)
    fclose(fLog);
  fLog=fopen(TBFileName.c_str(), mode);
  if (!fLog)
  {
    snprintf(errorStr, sizeof(errorStr), "Failed to open file(%s)", TBFileName.c_str());
    throw XLOGFILE1(0, errorStr);
  }
}

void TLogFile::Log(const char *msg, eLevel msgLevel)
{
  LogToFile(msg, msgLevel);
}

void TLogFile::Log(XCept &x, eLevel msgLevel)
{
  lprintf(msgLevel, "Caught exception Name %s In %s(%d) Error : %s",
          x.Name(), x.FileName(), x.Line(), x.ErrorStr());
}

void TLogFile::LogToFile(const char *msg, eLevel msgLevel)
{
  time_t      now;
  tm          *nowTm;
  char        currentDate[13];
  char strBuff[512];
  const char *aCustomString = "";
  if (this && this->fLog) 
  {
   aCustomString = CustomString.c_str();
  }
  if (msgLevel < doLevel)
    return;
  time(&now);
  nowTm=localtime(&now);
  if (this && this->fLog && Rotate)
  {
    snprintf(currentDate,
            sizeof(currentDate) - 1,
            "%04d%02d%02d",
            nowTm->tm_year + 1900, 
            nowTm->tm_mon + 1,  
            nowTm->tm_mday);
    // If it is a new day, rotate the log from logfile.txt to logfile_YYYMMDD.txt and reopen logfile.txt
    // Previous date is initialised as 99999999 to prevent hitting this the first time, if the year is past 99999999, WOW!
    if (atoi(currentDate) > PrevDate)
      SwitchOnDate();
    //Make the previous date = current date
    PrevDate = atoi(currentDate);
  }
#ifdef WIN32
  _timeb curTime;
  _ftime(&curTime);
#else
  timeb curTime;
  ftime(&curTime);
#endif
  snprintf(strBuff, sizeof(strBuff), "%04d%02d%02d%02d%02d%02d.%03d %-4s %s",
          nowTm->tm_year+1900, 
          nowTm->tm_mon+1, 
          nowTm->tm_mday,
          nowTm->tm_hour, 
          nowTm->tm_min, 
          nowTm->tm_sec,
          curTime.millitm,
          LevelStr[(int)msgLevel],
          aCustomString);
  if (this && this->fLog)
  {
    fwrite(strBuff, 1, strlen(strBuff), fLog);
    fwrite(msg, 1, strlen(msg), fLog);
    fputc('\n', fLog);
    fflush(fLog);
    LimitSize(fLog);
    if (Display)
      std::cout << strBuff << msg << std::endl << std::flush;
  }
  else
    std::cout << strBuff << msg << std::endl << std::flush;
}

void TLogFile::lprintf(eLevel msgLevel, const char *fmt, ...)
{
  va_list  argp;
  if (this) {
     va_start(argp, fmt);
     vsnprintf(buffer, LOGFILE_BUFFER_LENGTH, fmt, argp);
     LogToFile(buffer, msgLevel);
  }
  else {
     va_start(argp, fmt);
     vprintf(fmt, argp);
     va_end(argp);
  }
  va_end(argp);
}

void TLogFile::Progress(const char *fmt, ...)
{
  if (doLevel > eLogDebug)
    return;
  if (Display)
  {
    va_list  argp;
    va_start(argp, fmt);
    vprintf(fmt, argp);
    va_end(argp);
  }
}

#if defined(M_GNU)
struct autoWait
{
  int sem;
  autoWait(int aSem)
  : sem(aSem)
  {
    static struct sembuf acquire[] = // The possible semaphore
    { 0, 0, 0                        // Wait for semaphore == 0
    , 0, 1, SEM_UNDO                 // inc semaphore if crash undo
    };
    semop(sem, acquire, 2);
  }
  ~autoWait()
  {
    static struct sembuf release[] =
    { 0, -1, SEM_UNDO                // dec semaphore
    };
    semop(sem, release, 1);
  }
};
#endif

void TLogFile::LimitSize(FILE *fLog)
{
  char renFileName[DIRMAX_PATH];
  char dir[DIRMAX_DIR];
  char name[DIRMAX_NAME];
  char ext[DIRMAX_EXT];
  if (Rotate == true || maxFileSize < 1)
    return;
  int now = ftell(fLog);
  if (now > maxFileSize)
  {
#if defined(M_GNU)
    autoWait here(sem);
#endif
    fclose(fLog);
    FNameSplit(FileName.c_str(), dir, name, ext);
    FNameMerge(renFileName, dir, name, historyExtention.c_str());
    unlink(renFileName);
    rename(FileName.c_str(), renFileName);
    fLog=fopen(FileName.c_str(), "w");
  }
}

void TLogFile::LogLevel(const char *aStrLogLevel)
{
  char strLogLevel[50];
  strncpy(strLogLevel, aStrLogLevel, sizeof(strLogLevel) - 1);
  strLogLevel[sizeof(strLogLevel) - 1] = 0;
  strupr(strLogLevel);
  if (strcmp(strLogLevel, "DEBUG") == 0
  ||  strcmp(strLogLevel, "DBG") == 0)
    doLevel = eLogDebug;
  else if (strcmp(strLogLevel, "INFO") == 0
       ||  strcmp(strLogLevel, "INF") == 0)
    doLevel = eLogInfo;
  else if (strcmp(strLogLevel, "WARNING") == 0
       ||  strcmp(strLogLevel, "WRN") == 0)
    doLevel = eLogWarning;
  else if (strcmp(strLogLevel, "ERROR") == 0
       ||  strcmp(strLogLevel, "ERR") == 0)
    doLevel = eLogError;
  else
    doLevel = eLogDebug;
}



