#pragma once

#include <stdarg.h>

#pragma region Text
/////////////////////////////////////
// Text
class Text;

template <typename T>
inline T AlignWord(T val)
{
   T m = (val%4) ? 1 : 0;
   return (val/4 + m) * 4;
}

class Text
{
public:
   Text() : s(0), n(0) { }

   Text(const TCHAR *string) : s(0), n(0) {
      assign(string);
   }

   Text(const TCHAR *string, int len) : s(0), n(0) {
      assign(string, len);
   }

   Text( const Text& other ) : s(0), n(0) {
      assign(other.s);
   }

   Text & operator=( const Text& other ) {
      assign(other.s);
      return *this;
   }
   Text & operator=( const TCHAR * string ) {
      assign( string );
      return *this;
   }

   void assign( const TCHAR * string )
   {
      size_t len = string ? _tcslen(string) : 0;
      assign(string, len);
   }

   void assign( const TCHAR * string, size_t len )
   {
      if (len == 0) {
         clear();
      } else {
         reserve(len);
         _tcsncpy(s, string, len);
      }
   }

   void append( const TCHAR * string )
   {
      size_t len = (string ? _tcslen(string) : 0) +  (s ? _tcslen(s) : 0);
      if (len == 0) {
         clear();
      } else  {
         reserve(len);
         _tcscat(s, string);
      }
   }

   void append( TCHAR c )
   {
      size_t len = 1 + (s ? _tcslen(s) : 0);
      if (len == 0) {
         clear();
      } else  {
         reserve(len);
         TCHAR *p = s;
         *p++ = c;
         *p = 0;
      }
   }

   void append( TCHAR *string, size_t len )
   {
      size_t l = _tcslen(string) + size();
      if (l == 0) {
         clear();
      } else  {
         reserve(l);
         _tcscat(s, string);
      }
   }

   void reserve( size_t len )
   {
      if (len >= n)
      {
         TCHAR *olds = s;
         size_t oldn = n;
         size_t a = AlignWord(oldn * 3 / 2), b = AlignWord(len+1);
         n = max(a, b);
         s = (TCHAR*)calloc(n, sizeof(TCHAR));
         if (olds) {
            _tcsncpy(s, olds, oldn);
            free(olds);
         }
      }
   }

   void Format(const char *format, ...)
   {
      if (s) *s = 0;
      va_list args;
      va_start(args, format);
      AppendFormatV(format, args);
      va_end(args);
   }

   void AppendFormat(const char *format, ...)
   {
      va_list args;
      va_start(args, format);
      AppendFormatV(format, args);
      va_end(args);
   }

   void AppendFormatV(const char *format, va_list args)
   {
      TCHAR buffer[512];
      int nChars = _vsntprintf(buffer, _countof(buffer), format, args);
      if (nChars != -1) {
         append(buffer, nChars);
      } else {
         size_t Size = _vsctprintf(format, args);
         size_t Len = Length();
         Resize(Len + Size);
         nChars = _vsntprintf(data()+Len, capacity(), format, args);
      }
   }

   void Resize( size_t len )
   {
      reserve(len);
   }

   void remove( size_t off, size_t len = 1 )
   {
      if ( !isNull() ) {
         size_t n = Length();
         if ( off < n ) {
            size_t l = min(len, n-off);
            if (l > 0) {
               memmove(s+off, s+off+l, sizeof(TCHAR) * (n-off-l+1));
            }
         }
      }
   }

   void clear()
   {
      free(s);
      s = NULL;
      n = 0;
   }

   Text Substr(size_t off, size_t len)
   {
      if (isNull()) return Text();

      size_t l = Length();
      if ( off < l ) return Text();

      l = min(len, l-off);
      return Text(s+off, l);
   }

   void swap(Text& other)
   {
      size_t on = other.n; other.n = n; n = on;
      TCHAR *os = other.s; other.s = s; s = os;
   }

   size_t size() const {
      return (s ? _tcslen(s) : 0);
   }

   size_t Length() const {
      return size();
   }

   size_t capacity() const {
      return n;
   }

   bool isNull() const { return n == 0; }

   operator LPTSTR() { return s; }
   operator LPCTSTR() const { return s; }
   TCHAR* data() { return s; }
   const TCHAR *c_str() const { return s; }

   Text& operator+=( TCHAR c ) { append(c); return *this; }
   Text& operator+=( LPCTSTR s ) { append(s); return *this; }
   Text& operator+=( const Text& t ) { append(t); return *this; }

private:
   TCHAR *s;
   size_t n;
};


inline bool operator==( Text& lhs, const TCHAR *rhs) {
   return (strcmp(lhs.data(), rhs) == 0) ? true : false;
}

inline bool operator==( const TCHAR *lhs, Text& rhs) {
   return (strcmp(lhs, rhs.data()) == 0) ? true : false;
}

inline bool operator==( Text& lhs, Text& rhs) {
   return (strcmp(lhs.data(), rhs.data()) == 0) ? true : false;
}

inline bool operator!=( Text& lhs, const TCHAR *rhs) {
   return (strcmp(lhs.data(), rhs) == 0) ? false : true ;
}

inline bool operator!=( const TCHAR *lhs, Text& rhs) {
   return (strcmp(lhs, rhs.data()) == 0) ? false : true;
}

inline bool operator!=( Text& lhs, Text& rhs) {
   return (strcmp(lhs.data(), rhs.data()) == 0) ? false : true;
}

#ifdef _UTILITY_
_STD_BEGIN
template <>
inline void swap<Text>(Text& lhs, Text& rhs)
{
   lhs.swap(rhs);
}
_STD_END
#endif

#pragma endregion


#pragma region FixedString
/////////////////////////////////////
// FixedString

template<size_t size, int padding='\0'>
class FixedString
{
public:
   FixedString() {
      memset(v_, padding, size);
      v_[size] = 0;
   }
   FixedString(const char *string) {
      assign(string);
   }
   FixedString( const FixedString& other ) {
      memcpy(v_, other.v_, size+1);
   }
   FixedString & operator=( const FixedString& other ) {
      memcpy(v_, other.v_, size+1);
      return *this;
   }
   FixedString & operator=( const char * string ) {
      assign( string );
      return *this;
   }
   void assign( const char * string )
   {
      size_t len = string ? strlen(string) : 0;
      // assert(len <= size);
      len = min(len, size);
      _tcsncpy(v_, string, len);
      memset(v_+len, padding, size-len);
      v_[size] = 0;
   }
   void clear()
   {
      memset(v_, padding, size);
      v_[size] = 0;
   }
   operator LPTSTR() { return v_; }
   operator LPCTSTR() const { return v_; }
   char* data() { return v_; }
   const char *c_str() const { return v_; }

private:
   char v_[size+1];  //! NWArray of data.  Very important that this is only data for this class.
};

template<size_t fixedsize, int padding>
inline bool operator==( FixedString<fixedsize, padding>& lhs, const char *rhs) {
   return (strncmp(lhs.data(), rhs, fixedsize) == 0) ? true : false;
}

template<size_t fixedsize, int padding>
inline bool operator==( const char *lhs, FixedString<fixedsize, padding>& rhs) {
   return (strncmp(lhs, rhs.data(), fixedsize) == 0) ? true : false;
}

template<size_t fixedsize, int padding>
inline bool operator==( FixedString<fixedsize, padding>& lhs, FixedString<fixedsize, padding>& rhs) {
   return (strncmp(lhs.data(), rhs.data(), fixedsize) == 0) ? true : false;
}

template<size_t fixedsize, int padding>
inline bool operator!=( FixedString<fixedsize, padding>& lhs, const char *rhs) {
   return (strncmp(lhs.data(), rhs, fixedsize) == 0) ? false : true ;
}

template<size_t fixedsize, int padding>
inline bool operator!=( const char *lhs, FixedString<fixedsize, padding>& rhs) {
   return (strncmp(lhs, rhs.data(), fixedsize) == 0) ? false : true;
}

template<size_t fixedsize, int padding>
inline bool operator!=( FixedString<fixedsize, padding>& lhs, FixedString<fixedsize, padding>& rhs) {
   return (strncmp(lhs.data(), rhs.data(), fixedsize) == 0) ? false : true;
}
#pragma endregion


#pragma region Helpers
// sprintf for Text without having to worry about buffer size.
extern Text FormatText(const TCHAR* format,...);
extern Text& Trim(Text&p);

#pragma endregion