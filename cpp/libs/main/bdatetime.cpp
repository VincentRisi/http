/*==============================================================================
 Author : Nicolas Milicevic
 Date   : 2006/09/01
 Name   : bdatetime
 Descr  : Base date time object, a simple date time class that does quite a 
          lot of useful date manipulation
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "xstring.h"
#include "bdatetime.h"

static char *WeekDays[] = 
{ 
   "Sunday", 
   "Monday", 
   "Tuesday", 
   "Wednesday", 
   "Thursday", 
   "Friday", 
   "Saturday" 
};

static char *Months[] = 
{
   "January",
   "February",
   "March",
   "April",
   "May",
   "June",
   "July",
   "August",
   "September",
   "October",
   "November",
   "December"
};

#ifdef M_STDLIB
using namespace std;
#endif

/*------------------------------------------------------------------------------
   xBdatetime Constructor
------------------------------------------------------------------------------*/
xBdatetime::xBdatetime(const char *file, const int line, const char *fmt, ...)
       :xCept((char *) file, (int) line, "xBdatetime", -1)
{
   va_list ap;
   char    jotter[1024];

   va_start(ap, fmt);

   vsnprintf(jotter, sizeof(jotter) - 1, fmt, ap);

   va_end(ap);

   osErr << jotter << std::endl << ends;
}


/*-------------------------------------
   bdate static members
-------------------------------------*/
unsigned char bdate::monthEnds[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


/*----------------------------------------------------------------------------
   bdate constructors
----------------------------------------------------------------------------*/
bdate::bdate()
{
   From();
}


bdate::bdate(const unsigned year, 
             const unsigned month, 
             const unsigned day)
{
   From(year, month, day);
}


bdate::bdate(const struct tm &inDate)
{
   From(&inDate);
}



bdate::bdate(const bdate &inDate)
{
   memcpy(this, &inDate, sizeof(bdate));
}


bdate::bdate(const char *yyyymmdd)
{
   From(yyyymmdd);
}


bdate::bdate(const long yyyymmdd)
{
   char jotter[32];

   snprintf(jotter, sizeof(jotter) - 1, "%ld", yyyymmdd);

   From(jotter);
}


/*----------------------------------------------------------------------------
   bdate destructor
----------------------------------------------------------------------------*/
bdate::~bdate()
{
}


/*----------------------------------------------------------------------------
   From functions
----------------------------------------------------------------------------*/
void bdate::From(const struct tm *tmStruct)
{
   _year           = (unsigned short) tmStruct->tm_year + 1900;
   _month          = (unsigned char)  tmStruct->tm_mon;
   _day            = (unsigned char)  tmStruct->tm_mday;

   Validate();
}


void bdate::From(const unsigned year, 
                 const unsigned month, 
                 const unsigned day)
{
   _day      = (unsigned char) day;
   _month    = (unsigned char) month - 1;
   _year     = (unsigned short) year;

   Validate();
}


void bdate::From(const char *yyyymmdd)
{
   if (strlen(yyyymmdd) < 8)
      throw xBdatetime(__FILE__, __LINE__, "Invalid date string passed '%s'", yyyymmdd);

   char jotter[9];

   memcpy(jotter, yyyymmdd, 8);
   jotter[8] = 0;

   _day      = atoi(jotter + 6);
   jotter[6] = 0;

   _month    = atoi(jotter + 4) - 1;
   jotter[4] = 0;

   _year     = atoi(jotter);

   Validate();
}


void bdate::From()
{
   _day          = 0;
   _month        = 0;
   _year         = 0;
}


/*----------------------------------------------------------------------------
   validates this a valid date
----------------------------------------------------------------------------*/
void bdate::Validate()
{
   if (_year > 9999)
      throw xBdatetime(__FILE__, __LINE__, "'%u' is not a valid year", _year);

   if (_month > 11)
      throw xBdatetime(__FILE__, __LINE__, "'%u' is not a valid month", _month + 1);

   unsigned char endOfMonth = monthEnds[_month] + ((_month == 1 && IsLeapYear(_year)) ? 1 : 0);
   if (_day > endOfMonth)
      throw xBdatetime(__FILE__, __LINE__, "'%u' is not a valid day", _day);
}


/*------------------------------------------------------------------------------
   Static Validation Methods
------------------------------------------------------------------------------*/
bool bdate::ValidDate(const unsigned year, const unsigned month, const unsigned day)
{
   try
   {
      bdate date(year, month, day);
   }
   catch (xBdatetime)
   {
      return false;
   }

   return true;
}


bool bdate::ValidDate(const char *yyyymmdd)
{
   try
   {
      bdate date(yyyymmdd);
   }
   catch (xBdatetime)
   {
      return false;
   }

   return true;
}
      

bool bdate::ValidDate(const long yyyymmdd)
{
   try
   {
      bdate date(yyyymmdd);
   }
   catch (xBdatetime)
   {
      return false;
   }

   return true;
}


/*----------------------------------------------------------------------------
   assigment operators
----------------------------------------------------------------------------*/
bdate & bdate::operator = (const bdate &inDate)
{
   memcpy(this, &inDate, sizeof(*this));
   return *this;
}


bdate & bdate::operator = (const struct tm &inDate)
{
   From(&inDate);
   return *this;
}


bdate & bdate::operator = (const char *yyyymmdd)
{
   From(yyyymmdd);
   return *this;
}


bdate & bdate::operator = (const long yyyymmdd)
{
   char jotter[32];

   snprintf(jotter, sizeof(jotter) - 1, "%ld", yyyymmdd);

   From(jotter);

   return *this;
}


const bdate & bdate::operator ++ ()
{
   Add(1, days);
   return *this;
}


const bdate & bdate::operator -- ()
{
   Sub(1, days);
   return *this;
}


bdate bdate::operator ++ (int)
{
   bdate temp = *this;
   Add(1, days);
   return temp;
}


bdate bdate::operator -- (int)
{
   bdate temp = *this;
   Sub(1, days);
   return temp;
}


bdate &bdate::operator += (const unsigned numDays)
{
   Add(numDays, days);
   return *this;
}


bdate &bdate::operator -= (const unsigned numDays)
{
   Sub(numDays, days);
   return *this;
}


/*----------------------------------------------------------------------------
   Adds onto the date
----------------------------------------------------------------------------*/
void bdate::Add(unsigned amount, const DateLegend legend)
{
   switch (legend)
   {
      case weeks :
         amount *= 7;

      case days :
         {
            unsigned leap_check = (_month == 1 && IsLeapYear(_year)) ? 1 : 0;

            for (; amount; amount--)
               if (_day == (unsigned char) (monthEnds[_month] + leap_check))
               {
                  _day = 1;
                  if (_month == 11)
                  {
                     _month = 0;
                     _year++;
                  }
                  else
                     _month++;

                  leap_check = (_month == 1 && IsLeapYear(_year)) ? 1 : 0;
               }
               else
                  _day++;
         }
         break;

      case months : 
         for (; amount; amount--)
            if (_month == 11)
            {
               _month = 0;
               _year++;
            }
            else
               _month++;

         // Adjust day is differant month end date
         if (_day > monthEnds[_month])
         {
            _day = monthEnds[_month];
            if (_month == 1 && IsLeapYear(_year))
               _day++;
         }
         break;

      case years :
         _year += amount;

         // Check for the February hazard when doing years
         if (_month == 1 && _day == 29 && !IsLeapYear(_year))
            _day = 28;
         break;
   }
}


/*----------------------------------------------------------------------------
   Subtracts From a date
----------------------------------------------------------------------------*/
void bdate::Sub(unsigned amount, const DateLegend legend)
{
   switch (legend)
   {
      case weeks :
         amount *= 7;

      case days :
         for (; amount; amount--)
            if (_day == 1)
            {
                // check month
                if (_month)
                   _month--;
                else
                {
                   _month = 11;
                   _year--;
                }
                _day = monthEnds[_month] + ((_month == 1 && IsLeapYear(_year)) ? 1 : 0);
            }
            else
               _day--;
         break;

      case months :
         for (; amount; amount--)
            if (_month)
               _month--;
            else
            {
               _month = 11;
               _year--;
            }

         // Adjust day is differant month end date
         if (_day > monthEnds[_month])
         {
            _day = monthEnds[_month];
            if (_month == 1 && IsLeapYear(_year))
               _day++;
         }
            
         break;

      case years :
         _year -= amount;

         // Check for the February hazard when doing years
         if (_month == 1 && _day == 29 && !IsLeapYear(_year))
            _day = 28;
         break;
   }
}


/*----------------------------------------------------------------------------
   checks for leap year
----------------------------------------------------------------------------*/
bool bdate::IsLeapYear(const unsigned year)
{
   if (!(year % 400))
      return true;

   if (!(year % 100))
      return false;

   if (!(year % 4))
      return true;

   return false;
}


/*----------------------------------------------------------------------------
   Returns the number of days in this month
----------------------------------------------------------------------------*/
unsigned bdate::DaysInMonth() const
{
   return monthEnds[_month] + ((_month == 1 && IsLeapYear(_year)) ? 1 : 0);
}


/*------------------------------------------------------------------------------
   Returns the day of the year, 000 - 365
------------------------------------------------------------------------------*/
unsigned bdate::DayOfYear() const
{
   struct tm tm_rec;

   memset(&tm_rec, 0, sizeof(tm_rec));

   tm_rec.tm_year  = _year - 1900;
   tm_rec.tm_mon   = _month;
   tm_rec.tm_mday  = _day;

   mktime(&tm_rec);

   return (unsigned) tm_rec.tm_yday;
}


/*------------------------------------------------------------------------------
   Converts the date to a long
------------------------------------------------------------------------------*/
long bdate::ToLong() const
{
   return (_year * 10000) + ((_month + 1) * 100) + _day;
}


/*------------------------------------------------------------------------------
   Converts to Julian date
------------------------------------------------------------------------------*/
long bdate::JulDate() const
{
   unsigned short year;
   unsigned short day;
   unsigned short month;
   unsigned short centuary;
   unsigned short cy;

   year = _year < 1 ? 1 : _year;
   day  = (unsigned short) _day;

   if (_month + 1 > 2)
      month = _month - 2;
   else
   {
      month = _month + 10;
      year--;
   }

   centuary = (unsigned short) (year * 0.01);
   cy       = year - 100 * centuary;

   return (long) ((146097 * centuary * 0.25)  + 
                  (1461   * cy       * 0.25)  + 
                  ((153   * month + 2) / 5)   + 
                  day                         + 
                  1721119);
}


/*----------------------------------------------------------------------------
   returns the day type, either a weekday, sunday or saturday
----------------------------------------------------------------------------*/
bdate::DayLegend bdate::DayType() const
{
   struct tm tm_rec;

   memset(&tm_rec, 0, sizeof(tm_rec));

   tm_rec.tm_year  = _year - 1900;
   tm_rec.tm_mon   = _month;
   tm_rec.tm_mday  = _day;

   mktime(&tm_rec);

   return (DayLegend) tm_rec.tm_wday; // 0 = Sunday, .... 6 = Saturday
}


/*----------------------------------------------------------------------------
   Becomes the date of the local pc
----------------------------------------------------------------------------*/
void bdate::BecomeSystemDate()
{
   time_t      now;
   struct tm  *tm_rec;

   time(&now);
   tm_rec = localtime(&now);

   From(tm_rec);
}


/*----------------------------------------------------------------------------
   assign functions
----------------------------------------------------------------------------*/
void bdate::SetDay(const unsigned day)
{
   _day = (unsigned char ) day;   
   Validate();
}


void bdate::SetMonth(const unsigned month)
{
   _month = (unsigned char) month;
   Validate();
}


void bdate::SetYear(const unsigned year)
{
   _year = year;
   Validate();
}


/*----------------------------------------------------------------------------
   Compare functions
----------------------------------------------------------------------------*/
short bdate::Compare(const bdate *date2) const
{
   if (_year < date2->_year)
      return -1;

   if (_year > date2->_year)
      return +1;

   if (_month < date2->_month)
      return -1;

   if (_month > date2->_month)
      return +1;

   if (_day < date2->_day)
      return -1;

   if (_day > date2->_day)
      return +1;

   return 0;
}


/*----------------------------------------------------------------------------
   Compare operators - bdate &
----------------------------------------------------------------------------*/
bool operator < (const bdate &date1, const bdate &date2)
{
   return date1.Compare(&date2) < 0 ? true : false;
}

bool operator > (const bdate &date1, const bdate &date2)
{
   return date1.Compare(&date2) > 0 ? true : false;
}

bool operator <= (const bdate &date1, const bdate &date2)
{
   return date1.Compare(&date2) <= 0 ? true : false;
}

bool operator >= (const bdate &date1, const bdate &date2)
{
   return date1.Compare(&date2) >= 0 ? true : false;
}

bool operator == (const bdate &date1, const bdate &date2)
{
   return date1.Compare(&date2) == 0 ? true : false;
}

bool operator != (const bdate &date1, const bdate &date2)
{
   return date1.Compare(&date2) != 0 ? true : false;
}


/*----------------------------------------------------------------------------
   casts
----------------------------------------------------------------------------*/
bdate::operator struct tm () const
{
   struct tm tm_rec;

   memset(&tm_rec, 0, sizeof(struct tm));

   tm_rec.tm_year  = _year - 1900;
   tm_rec.tm_mon   = _month;
   tm_rec.tm_mday  = _day;

   return tm_rec;
}




/*----------------------------------------------------------------------------
   btime constructors
----------------------------------------------------------------------------*/
btime::btime()
{
   From();
}

btime::btime(const struct tm &in_date)
{
   From(&in_date);
}


btime::btime(const unsigned hours, const unsigned minutes, const unsigned seconds)
{
   From(hours, minutes, seconds);
}


btime::btime(const char *hhmmss)
{
   From(hhmmss);
}


btime::btime(const long hhmmss)
{
   char jotter[32];

   snprintf(jotter, sizeof(jotter) - 1, "%06ld", hhmmss);

   From(jotter);
}


btime::~btime()
{
}


/*----------------------------------------------------------------------------
   validates this a valid time
----------------------------------------------------------------------------*/
void btime::Validate()
{
   if (_hour > 23)
      throw xBdatetime(__FILE__, __LINE__, "'%u' is not a valid hour", _hour);

   if (_minute > 59)
      throw xBdatetime(__FILE__, __LINE__, "'%u' is not a valid minute", _minute);

   if (_second > 59)
      throw xBdatetime(__FILE__, __LINE__, "'%u' is not a valid second", _second);
}


/*------------------------------------------------------------------------------
   Static Validation Methods
------------------------------------------------------------------------------*/
bool btime::ValidTime(const unsigned hours, const unsigned minutes, const unsigned seconds)
{
   try
   {
      btime btime(hours, minutes, seconds);
   }
   catch (xBdatetime)
   {
      return false;
   }

   return true;
}


bool btime::ValidTime(const char *hhmmss)
{
   try
   {
      btime btime(hhmmss);
   }
   catch (xBdatetime)
   {
      return false;
   }

   return true;
}
      

bool btime::ValidTime(const long hhmmss)
{
   try
   {
      btime btime(hhmmss);
   }
   catch (xBdatetime)
   {
      return false;
   }

   return true;
}


/*----------------------------------------------------------------------------
   From functions
----------------------------------------------------------------------------*/
void btime::From(const struct tm *tm_struct)
{
   _hour           = (unsigned char)  tm_struct->tm_hour;
   _minute         = (unsigned char)  tm_struct->tm_min;
   _second         = (unsigned char)  tm_struct->tm_sec;

   Validate();
}


void btime::From(const unsigned hours, const unsigned minutes, const unsigned seconds)
{
   _hour   = (unsigned char) hours;
   _minute = (unsigned char) minutes;
   _second = (unsigned char) seconds;

   Validate();
}


void btime::From(const char *hhmmss)
{
   char *ptr = (char *) hhmmss;
   char  tmp[3];
   char  done = 0;

   while (*ptr)
   {
      if (done == 3)
      {
         Validate();
         return;
      }

      if (ptr[0] >= '0' && ptr[0] <= '9')
      {
         if (ptr[1] < '0' || ptr[1] > '9')
            throw xBdatetime(__FILE__, __LINE__, "'%s' is not a valid time", hhmmss);

         tmp[0] = ptr[0];
         tmp[1] = ptr[1];
         tmp[2] = 0;

         switch (done++)
         {
            case 0 : _hour   = (unsigned char) atoi(tmp); break;
            case 1 : _minute = (unsigned char) atoi(tmp); break;
            case 2 : _second = (unsigned char) atoi(tmp); break;
         }
      }

      ptr++;
   }

   throw xBdatetime(__FILE__, __LINE__, "'%s' is not a valid time", hhmmss);
}


void btime::From()
{
   _hour   = 0;
   _minute = 0;
   _second = 0;
}


/*----------------------------------------------------------------------------
   becomes the system time
----------------------------------------------------------------------------*/
void btime::BecomeSystemTime()
{
   time_t      now;
   struct tm  *tm_rec;

   time(&now);
   tm_rec = localtime(&now);

   From(tm_rec);
}


void btime::SetHour(const unsigned hour)
{
   _hour = (unsigned char) hour;
   Validate();
}


void btime::SetMinute(const unsigned minute)
{
   _minute = (unsigned char) minute;
   Validate();
}


void btime::SetSecond(const unsigned second)
{
   _second = (unsigned char) second;
   Validate();
}


/*----------------------------------------------------------------------------
   Add function
----------------------------------------------------------------------------*/
void btime::Add(unsigned amount, const TimeLegend legend)
{
   switch (legend)
   {
      case seconds :

         for (; amount; amount--)
            if (_second == 59)
            {
               _second = 0;

               if (_minute == 59)
               {
                  _minute  = 0;

                  if (_hour == 23)
                     _hour = 0;
               }
               else
                  _minute++;
            }
            else
               _second++;

         break;

      case minutes :

         for (; amount; amount--)
            if (_minute == 59)
            {
               _minute = 0;

               if (_hour == 23)
                  _hour = 0;
            }
            else
               _minute++;

         break;

      case hours :

         for (; amount; amount--)
            if (_hour == 23)
               _hour = 0;
            else
               _hour++;
   }
}


/*----------------------------------------------------------------------------
   Sub function
----------------------------------------------------------------------------*/
void btime::Sub(unsigned amount, const TimeLegend legend)
{
   switch (legend)
   {
      case seconds :

         for (; amount; amount--)
            if (_second == 0)
            {
               _second = 59;

               if (_minute == 0)
               {
                  _minute  = 59;

                  if (_hour == 0)
                     _hour = 59;
               }
               else
                  _minute--;
            }
            else
               _second--;

         break;

      case minutes :

         for (; amount; amount--)
            if (_minute == 0)
            {
               _minute = 59;

               if (_hour == 0)
                  _hour = 23;
            }
            else
               _minute--;

         break;

      case hours :

         for (; amount; amount--)
            if (_hour == 0)
               _hour = 23;
            else
               _hour--;
   }
}


/*------------------------------------------------------------------------------
   Converts the to,e to a long
------------------------------------------------------------------------------*/
long btime::ToLong() const
{
   return (_hour * 10000) + (_minute * 100) + _second;
}


/*----------------------------------------------------------------------------
   assigment operators
----------------------------------------------------------------------------*/
btime &btime::operator = (const btime &inTime)
{
   memcpy(this, &inTime, sizeof(*this));
   return *this;
}


btime &btime::operator = (const struct tm &inTime)
{
   From(&inTime);
   return *this;
}


btime &btime::operator = (const char *hhmmss)
{
   From(hhmmss);
   return *this;
}


btime &btime::operator += (const unsigned seconds)
{
   Add(seconds, btime::seconds);
   return *this;
}


btime &btime::operator -= (const unsigned seconds)
{
   Sub(seconds, btime::seconds);
   return *this;
}


const btime &btime::operator ++ ()
{
   Add(1, seconds);
   return *this;
}


const btime &btime::operator -- ()
{
   Sub(1, seconds);
   return *this;
}


btime btime::operator ++ (int)
{
   btime tmp = *this;
   Add(1, seconds);
   return tmp;
}


btime btime::operator -- (int)
{
   btime tmp = *this;
   Sub(1, seconds);
   return tmp;
}


/*----------------------------------------------------------------------------
   Compare functions
----------------------------------------------------------------------------*/
short btime::Compare(const btime *time2) const
{
   if (_hour < time2->_hour)
      return -1;

   if (_hour > time2->_hour)
      return +1;

   if (_minute < time2->_minute)
      return -1;

   if (_minute > time2->_minute)
      return +1;

   if (_second < time2->_second)
      return -1;

   if (_second > time2->_second)
      return +1;

   return 0;
}


/*----------------------------------------------------------------------------
   comparison functions - btime &
----------------------------------------------------------------------------*/
bool operator <  (const btime &time1, const btime &time2)
{
   return time1.Compare(&time2) < 0 ? true : false;
}


bool operator >  (const btime &time1, const btime &time2)
{
   return time1.Compare(&time2) > 0 ? true : false;
}


bool operator <= (const btime &time1, const btime &time2)
{
   return time1.Compare(&time2) <= 0 ? true : false;
}


bool operator >= (const btime &time1, const btime &time2)
{
   return time1.Compare(&time2) >= 0 ? true : false;
}


bool operator == (const btime &time1, const btime &time2)
{
   return time1.Compare(&time2) == 0 ? true : false;
}


bool operator != (const btime &time1, const btime &time2)
{
   return time1.Compare(&time2) != 0 ? true : false;
}


/*----------------------------------------------------------------------------
   casts
----------------------------------------------------------------------------*/
btime::operator struct tm () const
{
   struct tm tm_rec;

   memset(&tm_rec, 0, sizeof(struct tm));

   tm_rec.tm_hour = _hour;
   tm_rec.tm_min  = _minute;
   tm_rec.tm_sec  = _second;

   return tm_rec;
}



/*------------------------------------------------------------------------------
   bdatetime Constructors
------------------------------------------------------------------------------*/
bdatetime::bdatetime()
{
}
                    

bdatetime::bdatetime(bdate &date)
{
   Date = date;
}


bdatetime::bdatetime(btime &time)
{
   Time = time;
}


bdatetime::bdatetime(bdate &date, btime &time)
{
   Date = date;
   Time = time;
}


bdatetime::bdatetime(const struct tm &inDateTime)
{
   Date = inDateTime;
   Time = inDateTime;
}


bdatetime::bdatetime(const char *yyyymmdd, const char *hhmmss)
{
   Date = yyyymmdd;
   Time = hhmmss;
}


/*------------------------------------------------------------------------------
   bdatetime Destructor
------------------------------------------------------------------------------*/
bdatetime::~bdatetime()
{
}


/*----------------------------------------------------------------------------
   assigment operators
----------------------------------------------------------------------------*/
bdatetime &bdatetime::operator = (const bdatetime &inDateTime)
{
   Date = inDateTime.Date;
   Time = inDateTime.Time;

   return *this;
}


bdatetime &bdatetime::operator = (const bdate &inDate)
{
   Date = inDate;

   return *this;
}


bdatetime &bdatetime::operator = (const btime &inTime)
{
   Time = inTime;

   return *this;
}


bdatetime &bdatetime::operator = (const struct tm &inDateTime)
{
   Date = inDateTime;
   Time = inDateTime;

   return *this;
}


/*----------------------------------------------------------------------------
   comparison functions - bdatetime &
----------------------------------------------------------------------------*/
bool operator < (const bdatetime &datetime1, const bdatetime &datetime2)
{
   short rc = datetime1.Date.Compare(&datetime2.Date);

   if (rc == 0)
      return datetime1.Time.Compare(&datetime2.Time) < 0 ? true : false;

   return rc < 0 ? true : false;
}


bool operator >  (const bdatetime &datetime1, const bdatetime &datetime2)
{
   short rc = datetime1.Date.Compare(&datetime2.Date);

   if (rc == 0)
      return datetime1.Time.Compare(&datetime2.Time) > 0 ? true : false;

   return rc > 0 ? true : false;
}


bool operator <= (const bdatetime &datetime1, const bdatetime &datetime2)
{
   short rc = datetime1.Date.Compare(&datetime2.Date);

   if (rc == 0)
      return datetime1.Time.Compare(&datetime2.Time) <= 0 ? true : false;

   return rc <= 0 ? true : false;
}


bool operator >= (const bdatetime &datetime1, const bdatetime &datetime2)
{
   short rc = datetime1.Date.Compare(&datetime2.Date);

   if (rc == 0)
      return datetime1.Time.Compare(&datetime2.Time) >= 0 ? true : false;

   return rc >= 0 ? true : false;
}


bool operator == (const bdatetime &datetime1, const bdatetime &datetime2)
{
   short rc = datetime1.Date.Compare(&datetime2.Date);

   if (rc == 0)
      return datetime1.Time.Compare(&datetime2.Time) == 0 ? true : false;

   return rc == 0 ? true : false;
}


bool operator != (const bdatetime &datetime1, const bdatetime &datetime2)
{
   short rc = datetime1.Date.Compare(&datetime2.Date);

   if (rc == 0)
      return datetime1.Time.Compare(&datetime2.Time) != 0 ? true : false;

   return rc != 0 ? true : false;
}


/*------------------------------------------------------------------------------
   becomes the system date and time
------------------------------------------------------------------------------*/
void bdatetime::BecomeSystemDateTime()
{
   Date.BecomeSystemDate();
   Time.BecomeSystemTime();
}


/*----------------------------------------------------------------------------
   casts
----------------------------------------------------------------------------*/
bdatetime::operator struct tm () const
{
   struct tm tm_rec;

   memset(&tm_rec, 0, sizeof(struct tm));

   tm_rec.tm_year  = Date.Year() - 1900;
   tm_rec.tm_mon   = Date.Month() - 1;
   tm_rec.tm_mday  = Date.Day();

   tm_rec.tm_hour = Time.Hour();
   tm_rec.tm_min  = Time.Minute();
   tm_rec.tm_sec  = Time.Second();

   return tm_rec;
}


/*------------------------------------------------------------------------------
   %a abbreviated weekday name
   %A full weekday name
   %b abbreviated month name
   %B full month name
   %d day of month as decimal [01,31]
   %H Hour (24-hour clock) as a decimal number [00,23]
   %I Hour (12-hour clock) as a decimal number [01,12]
   %j Day of the year as a decimal number [001,366]
   %m Month as a decimal number [01,12]
   %M Minute as a decimal number [00,59]
   %p Locale's equivalent of either AM or PM
   %S Second as a decimal number [00,61]
   %w Weekday as a decimal number [0(Sunday),6]
   %y Year without century as a decimal number [00,99]
   %Y Year as a decimal number [0000,9999]
   %% A literal "%" character
------------------------------------------------------------------------------*/
char *bdatetime::Format(char *dest, const char *fmt) const
{
   *dest = 0;

   if (*fmt == 0)
      return dest;

   char   *start = dest;
   char   *pos;
   char    jotter[32];
   int     var;

   for (pos = (char *) fmt; *pos; pos++, dest++)
   {
      if (*pos != '%')
      {
         *dest = *pos;
         continue;
      }

      pos++;

      switch (*pos)
      {
         case 'y' :
            sprintf(jotter, "%4.4d", Date._year);

            *dest = jotter[2];
            dest++;
            *dest = jotter[4];
            break;

         case 'Y' :
            sprintf(dest, "%4.4d", Date._year);
            dest += 3;
            break;

         case 'd' :
            sprintf(dest, "%2.2u", Date._day);
            dest += 1;
            break;

         case 'm' :
            sprintf(dest, "%2.2d", Date._month + 1);
            dest += 1;
            break;

         case 'M' :
            sprintf(dest, "%2.2d", Time._minute);
            dest += 1;
            break;

         case 'a' :
            var = (int) Date.DayType();
            memcpy(dest, WeekDays[var], 3);
            dest += 2;
            break;

         case 'A':
            var = (int) Date.DayType();
            strcpy(dest, WeekDays[var]);
            dest += strlen(WeekDays[var]);
            break;

         case 'b' :
            memcpy(dest, Months[Date._month], 3);
            dest += 2;
            break;

         case 'B' :
            strcpy(dest, Months[Date._month]);
            dest += strlen(Months[Date._month]) - 1;
            break;

         case 'H' :
            sprintf(dest, "%2.2u", Time._hour);
            dest += 1;
            break;

         case 'I' :
            var = Time._hour;

            if (var > 11)
               var -= 12;

            sprintf(dest, "%2.2d", var);
            dest += 1;
            break;

         case 'j' :
            sprintf(dest, "%3.3d", Date.DayOfYear() + 1);
            dest += 2;
            break;

         case 'p' :
            if (Time._hour > 11)
               strcpy(dest, "PM");
            else
               strcpy(dest, "AM");

            dest += 1;
            break;

         case 'S' :
            sprintf(dest, "%2.2d", Time._second);
            dest += 1;
            break;

         case 'w' :
            sprintf(dest, "%d", (int) Date.DayType());
            break;

         case '%' :
            *dest = '%';
            break;

         default :
            *dest = '?';
            break;
      }
   }

   *dest = 0;

   return start;
}
