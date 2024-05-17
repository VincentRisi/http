#ifndef __BSTRING_H_
#define __BSTRING_H_

#include "xcept.h"
#include "collection.h"

class XBstring : public XCept
{
   public:

      XBstring(const char *file, const int line, const char *fmt, ...);

      XBstring(const XBstring &x) : XCept(x) { }
};

class bstrings;

class bstring
{
   public:
                          bstring();
                          bstring(const char ch);
                          bstring(const char *inStr);
                          bstring(const bstring &inStr);
                          bstring(const int32 strVal);
                          bstring(const int64 strVal);
                          bstring(const unsigned strVal);
                          bstring(const double strVal);
                          bstring(const bool strVal);

                         ~bstring();

      const char         *c_str() const { return _str; }
      const unsigned      length() const { return _strLen; }
      void                shrink(const unsigned val);
      void                setLength(const unsigned length, const char ch = ' ');

      static char         dirSep();

      /*-----------------------------------
         Utilities
      -----------------------------------*/
      bool                isEmpty() const;
      int                 find(const bstring &inStr, const unsigned offset = 0) const;
      int                 rfind(const bstring &inStr, const unsigned offset = 0) const;
      unsigned            count(const bstring &inStr) const;

      bstring            &format(const char *fmt, ...);
      bstring            &lower();
      bstring            &upper();

      bstring             fileExtension(const char ext_ch = '.') const;
      bstring             splice(const int pos1, const int pos2 = 0) const;

      bstring            &rstrip(const bstring &inStr = "");
      bstring            &lstrip(const bstring &inStr = "");
      bstring            &strip(const bstring &inStr = "");

      bstring            &rjust(const unsigned size, const char ch = ' ');
      bstring            &ljust(const unsigned size, const char ch = ' ');
      bstring            &center(const unsigned size, const char ch = ' ');

      bstring            &replace(const bstring &oldStr, const bstring &newStr);
      bstring            &reverse();
      bstring            &makeDirectory();
      bstring            &capitalize();
      bstring            &title();

      unsigned            split(const bstring &sep, bstrings &dest) const;
      unsigned            splitLines(bstrings &dest) const;
      bool                endsWith(const bstring &inStr) const;
      bool                startsWith(const bstring &inStr) const;

      int                 compare(const bstring &cmpStr, const bool ignoreCase = false) const;


      /*-------------------------------------
         check methods
      -------------------------------------*/
      bool                isInt() const;
      bool                isLong() const;
      bool                isUnsigned() const;
      bool                isDouble() const;
      bool                isBool() const;
      bool                isAlpha() const;
      bool                isUpper() const;
      bool                isLower() const;

      int                 toInt() const;
      long                toLong() const;
      unsigned            toUnsigned() const;
      double              toDouble() const;
      bool                toBool() const;


      /*-------------------------------------
         assingment operators
      -------------------------------------*/
      operator const char* () const { return _str; }
      operator char* () const { return _str; }

      char & operator [] (const unsigned index)  { return _str[index]; }
      char & operator [] (const int index)       { return _str[index]; }

      bstring & operator = (const char ch);
      bstring & operator = (const char *inStr);
      bstring & operator = (const bstring &inStr);
      bstring & operator = (const int strVal);
      bstring & operator = (const unsigned strVal);
      bstring & operator = (const double strVal);
      bstring & operator = (const bool strVal);

      bstring & operator += (const char  ch);
      bstring & operator += (const char *inStr);
      bstring & operator += (const bstring &inStr);
      bstring & operator += (const int strVal);
      bstring & operator += (const unsigned strVal);
      bstring & operator += (const double strVal);
      bstring & operator += (const bool strVal);

      friend bstring operator + (const bstring &str, const bstring &inStr);

      friend bstring operator + (const bstring &str, const char ch);
      friend bstring operator + (const bstring &str, const char *inStr);
      friend bstring operator + (const bstring &str, const int strVal);
      friend bstring operator + (const bstring &str, const unsigned strVal);
      friend bstring operator + (const bstring &str, const double strVal);
      friend bstring operator + (const bstring &str, const bool strVal);

      friend bstring operator + (const char ch,         const bstring &str);
      friend bstring operator + (const char *inStr,     const bstring &str);
      friend bstring operator + (const int strVal,      const bstring &str);
      friend bstring operator + (const unsigned strVal, const bstring &str);
      friend bstring operator + (const double strVal,   const bstring &str);
      friend bstring operator + (const bool strVal,     const bstring &str);

      /*-------------------------------------
         comparison operators
      -------------------------------------*/
      friend bool operator <  (const bstring &str, const char ch);
      friend bool operator >  (const bstring &str, const char ch);
      friend bool operator <= (const bstring &str, const char ch);
      friend bool operator >= (const bstring &str, const char ch);
      friend bool operator == (const bstring &str, const char ch);
      friend bool operator != (const bstring &str, const char ch);

      friend bool operator <  (const bstring &str, const char *inStr);
      friend bool operator >  (const bstring &str, const char *inStr);
      friend bool operator <= (const bstring &str, const char *inStr);
      friend bool operator >= (const bstring &str, const char *inStr);
      friend bool operator == (const bstring &str, const char *inStr);
      friend bool operator != (const bstring &str, const char *inStr);

      friend bool operator <  (const bstring &str, const bstring &inStr);
      friend bool operator >  (const bstring &str, const bstring &inStr);
      friend bool operator <= (const bstring &str, const bstring &inStr);
      friend bool operator >= (const bstring &str, const bstring &inStr);
      friend bool operator == (const bstring &str, const bstring &inStr);
      friend bool operator != (const bstring &str, const bstring &inStr);

      friend bool operator <  (const bstring &str, const int strVal);
      friend bool operator >  (const bstring &str, const int strVal);
      friend bool operator <= (const bstring &str, const int strVal);
      friend bool operator >= (const bstring &str, const int strVal);
      friend bool operator == (const bstring &str, const int strVal);
      friend bool operator != (const bstring &str, const int strVal);

      friend bool operator <  (const bstring &str, const unsigned strVal);
      friend bool operator >  (const bstring &str, const unsigned strVal);
      friend bool operator <= (const bstring &str, const unsigned strVal);
      friend bool operator >= (const bstring &str, const unsigned strVal);
      friend bool operator == (const bstring &str, const unsigned strVal);
      friend bool operator != (const bstring &str, const unsigned strVal);

      friend bool operator <  (const bstring &str, const double strVal);
      friend bool operator >  (const bstring &str, const double strVal);
      friend bool operator <= (const bstring &str, const double strVal);
      friend bool operator >= (const bstring &str, const double strVal);
      friend bool operator == (const bstring &str, const double strVal);
      friend bool operator != (const bstring &str, const double strVal);

      friend bool operator == (const bstring &str, const bool strVal);
      friend bool operator != (const bstring &str, const bool strVal);

   private:

                        bstring(const char *str1, const char *str2);
                        bstring(const char *str1, const unsigned len);

      char             *_str;    
      unsigned          _strLen;

      bstring          *newStr(const char *str);
      bstring          *appendStr(const char *extra_str);
      void              freeStr();
      void              replaceStr(const bstring &dest, const bstring &source);
      void              stripChars(const bool left, const bool right, char *chars);


      static bool       isTrue(const char *str);
      static bool       isFalse(const char *str);

};


/*-----------------------------------
   bstrings   
-----------------------------------*/
class bstrings : public Collection<bstring>
{
   public :

              bstrings();
     virtual ~bstrings();
};


/*-----------------------------------
   bstringlink
-----------------------------------*/
class bstringlink
{
   public :

                   bstringlink();
                  
      virtual     ~bstringlink();
                  
      void         set(const bstring &link, const bstring &value);
                  
      void         remove(const bstring &link);

      void         remove(const unsigned index);
                  
      void         clear();
               
      bstring     *get(const bstring &link, const bool ignoreCase = false) const;
               
      bstring     *get(const unsigned index) const;
               
      bstring     *getLink(const unsigned index) const;

      void         linksToUpper();

      unsigned     count() const { return _links.Count(); }

      unsigned     getIndex(const bstring &link, const bool ignoreCase = false) const;


   private :

      bstrings   _links;
      bstrings   _values;
};


#endif
