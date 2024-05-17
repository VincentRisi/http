#ifndef __BDATE_TIME_H_
#define __BDATE_TIME_H_ "$Revision: 92 $ $Date: 2005-04-21 11:34:40 +0200 (Thu, 21 Apr 2005) $"
#include "versions.h"
HEADER_VERSION(__BDATE_TIME_H_);

#include <time.h>

#include "xcept.h"

/*-----------------------------------
   xBdatetime
-----------------------------------*/
class xBdatetime : public xCept
{
   public:

      xBdatetime(const char *file, const int line, const char *fmt, ...);

      xBdatetime(const xBdatetime &x) : xCept(x) { }
};

 
class bdatetime;

/*-----------------------------------
   bdate
-----------------------------------*/
class bdate
{
   friend class bdatetime;

   public:

      /*-------------------------------------
         bdate legends
      -------------------------------------*/
      typedef enum
      {
         days = 0,
         weeks,
         months,
         years
      } DateLegend;

      typedef enum
      {
         sunday = 0,
         monday,
         tuesday,
         wednesday,
         thursday,
         friday,
         saturday
      } DayLegend;

      // constructors 
                        bdate();
                        bdate(const bdate &inDate);
                        bdate(const struct tm &inDate);
                        bdate(const unsigned year, const unsigned month, const unsigned day);
                        bdate(const char *yyyymmdd);
                        bdate(const long yyyymmdd);
                       ~bdate();

      /*-------------------------------------
         manipulation functions
      -------------------------------------*/
      void              Add(unsigned amount, const DateLegend legend);
      void              Sub(unsigned amount, const DateLegend legend);
                       
      operator struct tm () const;

      void              BecomeSystemDate();
      void              SetDay(const unsigned day);
      void              SetMonth(const unsigned month);
      void              SetYear(const unsigned year);

      /*-------------------------------------
         display functions
      -------------------------------------*/
      unsigned          Year()  const { return _year; }
      unsigned          Month() const { return _month + 1; }
      unsigned          Day()   const { return _day; }
      DayLegend         DayType() const;
      unsigned          DaysInMonth() const;
      unsigned          DayOfYear() const;
      long              ToLong() const;
      long              JulDate() const;

      /*-------------------------------------
         assignment operators
      -------------------------------------*/
      bdate & operator = (const bdate &inDate);
      bdate & operator = (const struct tm &inDate);
      bdate & operator = (const char *yyyymmdd);
      bdate & operator = (const long yyyymmdd);

      bdate & operator += (const unsigned days);
      bdate & operator -= (const unsigned days);
      const bdate & operator ++ ();
      const bdate & operator -- ();
      bdate operator ++ (int);
      bdate operator -- (int);

      /*-------------------------------------
         Compare operators
      -------------------------------------*/
      friend bool operator <  (const bdate &date1, const bdate &date2);
      friend bool operator >  (const bdate &date1, const bdate &date2);
      friend bool operator <= (const bdate &date1, const bdate &date2);
      friend bool operator >= (const bdate &date1, const bdate &date2);
      friend bool operator == (const bdate &date1, const bdate &date2);
      friend bool operator != (const bdate &date1, const bdate &date2);

      short             Compare(const bdate *date2) const;

      /*-------------------------------------
         statics
      -------------------------------------*/
      static bool       IsLeapYear(const unsigned year);
      static bool       ValidDate(const unsigned year, const unsigned month, const unsigned day);
      static bool       ValidDate(const char *yyyymmdd);
      static bool       ValidDate(const long yyyymmdd);

   private:

      unsigned short    _year;
      unsigned char     _month;
      unsigned char     _day;

      static unsigned char monthEnds[];

      void              From(const struct tm *tmStruct);
      void              From(const unsigned year, const unsigned month, const unsigned day);
      void              From(const char *yyyymmdd);
      void              From();

      void              Validate();
};


/*-----------------------------------
   btime
-----------------------------------*/
class btime
{
   friend class bdatetime;

   public:

      /*-------------------------------------
         btime legends
      -------------------------------------*/
      typedef enum
      {
         hours,
         minutes,
         seconds
      } TimeLegend;

      // constructors
                        btime();
                        btime(const struct tm &inTime);
                        btime(const unsigned hours, const unsigned minutes, const unsigned seconds);
                        btime(const char *hhmmss);
                        btime(const long hhmmss);
                       ~btime();

      /*-------------------------------------
         manipulation functions
      -------------------------------------*/
      void              Add(unsigned amount, const TimeLegend legend);
      void              Sub(unsigned amount, const TimeLegend legend);

      operator struct tm () const;

      void              BecomeSystemTime();
      void              SetHour(const unsigned hour);
      void              SetMinute(const unsigned minute);
      void              SetSecond(const unsigned second);

      /*-------------------------------------
         display functions
      -------------------------------------*/
      unsigned          Hour()  const { return _hour; }
      unsigned          Minute() const { return _minute; }
      unsigned          Second()   const { return _second; }
      bool              AM() const { return _hour < 12 ? true : false; }
      bool              PM() const { return _hour > 11 ? true : false; }
      long              ToLong() const;

      /*-------------------------------------
         assignment operators
      -------------------------------------*/
      btime & operator = (const btime &inTime);
      btime & operator = (const struct tm &inTime);
      btime & operator = (const char *hhmmss);
      btime & operator = (const long hhmmss);

      btime & operator += (const unsigned seconds);
      btime & operator -= (const unsigned seconds);
      const btime & operator ++ ();
      const btime & operator -- ();
      btime operator ++ (int);
      btime operator -- (int);

      /*-------------------------------------
         Compare operators
      -------------------------------------*/
      friend bool operator <  (const btime &time1, const btime &time2);
      friend bool operator >  (const btime &time1, const btime &time2);
      friend bool operator <= (const btime &time1, const btime &time2);
      friend bool operator >= (const btime &time1, const btime &time2);
      friend bool operator == (const btime &time1, const btime &time2);
      friend bool operator != (const btime &time1, const btime &time2);

      short             Compare(const btime *time2) const;

      /*-------------------------------------
         statics
      -------------------------------------*/
      static bool       ValidTime(const unsigned hours, const unsigned minutes, const unsigned seconds);
      static bool       ValidTime(const char *hhmmss);
      static bool       ValidTime(const long hhmmss);

   private:

      unsigned char     _hour;
      unsigned char     _minute;
      unsigned char     _second;

      void              From(const struct tm *tm_struct);
      void              From(const unsigned hours, const unsigned minutes, const unsigned seconds);
      void              From(const char *hhmmss);
      void              From();

      void              Validate();
};


/*-----------------------------------
   bdatetime
-----------------------------------*/
class bdatetime
{
   public  :
                    bdatetime();
                    bdatetime(bdate &date);
                    bdatetime(btime &time);
                    bdatetime(bdate &date, btime &time);
                    bdatetime(const struct tm &inDateTime);
                    bdatetime(const char *yyyymmdd, const char *hhmmss);
                    bdatetime(const long yyyymmdd, const long hhmmss);
                   ~bdatetime();


      /*-------------------------------------
         manipulation functions
      -------------------------------------*/
      void          BecomeSystemDateTime();

      char         *Format(char *dest, const char *fmt) const;

      operator struct tm () const;


      /*-------------------------------------
         assignment operators
      -------------------------------------*/
      bdatetime & operator = (const bdatetime &inDateTime);
      bdatetime & operator = (const bdate     &inDate);
      bdatetime & operator = (const btime     &inTime);
      bdatetime & operator = (const struct tm &inDateTime);

      /*-------------------------------------
         Compare operators
      -------------------------------------*/
      friend bool operator <  (const bdatetime &time1, const bdatetime &time2);
      friend bool operator >  (const bdatetime &time1, const bdatetime &time2);
      friend bool operator <= (const bdatetime &time1, const bdatetime &time2);
      friend bool operator >= (const bdatetime &time1, const bdatetime &time2);
      friend bool operator == (const bdatetime &time1, const bdatetime &time2);
      friend bool operator != (const bdatetime &time1, const bdatetime &time2);

      bdate         Date;
      btime         Time;

   private :
};


      


#endif
