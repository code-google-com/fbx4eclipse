#pragma once

#include <stdarg.h>
#include "utility.h"

template <typename T>
inline T AlignWord(T val)
{
	T m = (val%4) ? 1 : 0;
	return (val/4 + m) * 4;
}

#pragma region Text
struct TextHelper
{
	static CHAR* _Copy(CHAR* dst, const CHAR* src, size_t len) { 
		return strncpy(dst, src, len);
	}
	static WCHAR* _Copy(WCHAR* dst, const WCHAR* src, size_t len) { 
		return wcsncpy(dst, src, len);
	}
	static CHAR* _Copy(CHAR* dst, const WCHAR* src, size_t len) { 
		size_t slen = wcslen(src) + 1; len = min(slen, len);
		wcstombs(dst, src, len);
		return dst;
	}
	static WCHAR* _Copy(WCHAR* dst, const CHAR* src, size_t len) { 
		size_t slen = strlen(src) + 1; len = min(slen, len);
		mbstowcs(dst, src, len);
		return dst;
	}
	static size_t _Size(const CHAR* src) { return src ? strlen(src) : 0; }
	static size_t _Size(const WCHAR* src) { return src ? wcslen(src) : 0; }
	static CHAR* _Concat(CHAR* dst, const CHAR* src, size_t len) { 
		CHAR *p = strncpy(dst+_Size(dst), src, len); p[len] = 0; return p;
	}
	static WCHAR* _Concat(WCHAR* dst, const WCHAR* src, size_t len) { 
		WCHAR *p = wcsncpy(dst+_Size(dst), src, len);p[len] = 0; return p;
	}
	static CHAR* _Concat(CHAR* dst, const WCHAR* src, size_t len) { 
		size_t slen = wcslen(src) + 1; len = min(slen, len);
		CHAR* p = dst+_Size(dst); wcstombs(p, src, len); p[len] = 0;
		return dst;
	}
	static WCHAR* _Concat(WCHAR* dst, const CHAR* src, size_t len) { 
		size_t slen = strlen(src) + 1; len = min(slen, len);
		WCHAR *p = dst+_Size(dst); mbstowcs(p, src, len); p[len] = 0;
		return dst;
	}
	static int _Compare(const CHAR* src, const CHAR *dst, bool ignoreCase = false) { 
		return ignoreCase ? stricmp(src, dst) : strcmp(src, dst); 
	}
	static int _Compare(const WCHAR* src, const WCHAR *dst, bool ignoreCase = false) { 
		return ignoreCase ? wcsicmp(src, dst) : wcscmp(src, dst); 
	}
};
#pragma endregion

#pragma region Text
/////////////////////////////////////
// Text

template<class T>
class TextBase : TextHelper
{
private:

public:
	TextBase() : s(0), n(0) { }

	TextBase(const CHAR *string) : s(0), n(0) {
		assign(string);
	}

	TextBase(const CHAR *string, int len) : s(0), n(0) {
		assign(string, len);
	}

	TextBase(const WCHAR *string) : s(0), n(0) {
		assign(string);
	}

	TextBase(const WCHAR *string, int len) : s(0), n(0) {
		assign(string, len);
	}

	TextBase( const TextBase& other ) : s(0), n(0) {
		assign(other.s);
	}

	TextBase & operator=( const TextBase& other ) {
		assign(other.s);
		return *this;
	}
	TextBase & operator=( const CHAR * string ) {
		assign( string );
		return *this;
	}
	TextBase & operator=( const WCHAR * string ) {
		assign( string );
		return *this;
	}

	void assign( const CHAR * string )
	{
		size_t len = string ? strlen(string) + 1 : 0;
		assign(string, len);
	}

	void assign( const WCHAR * string )
	{
		size_t len = string ? wcslen(string) + 1: 0;
		assign(string, len);
	}

	void assign( const CHAR * string, size_t len )
	{
		if (len == 0) {
			clear();
		} else {
			reserve(len);
			_Copy(s, string, len);
		}
	}

	void assign( const WCHAR * string, size_t len )
	{
		if (len == 0) {
			clear();
		} else {
			reserve(len);
			_Copy(s, string, len);
		}
	}

	void append( const CHAR * string )
	{
		size_t slen = (string ? strlen(string) + 1 : 0);
		size_t len = slen + size();
		if (len == 0) {
			clear();
		} else if (slen != 0) {
			reserve(len);
			_Concat(s, string, slen);
		}
	}


	void append( const WCHAR * string )
	{
		size_t slen = (string ? wcslen(string) + 1 : 0);
		size_t len = slen + size();
		if (len == 0) {
			clear();
		} else if (slen != 0) {
			reserve(len);
			_Concat(s, string, slen);
		}
	}

	void append( CHAR c )
	{
		size_t len = 1 + size();
		if (len == 0) {
			clear();
		} else  {
			reserve(len);
			T *p = s + len - 1;
			*p++ = c;
			*p = 0;
		}
	}

	void append( WCHAR c )
	{
		size_t len = 1 + size();
		if (len == 0) {
			clear();
		} else  {
			reserve(len);
			T *p = s + len - 1;
			*p++ = c;
			*p = 0;
		}
	}

	void append( const CHAR * string, size_t len )
	{
		size_t slen = min(len, (string ? strlen(string) + 1 : 0));
		size_t vlen = slen + size();
		if (vlen == 0) {
			clear();
		} else if (slen != 0) {
			reserve(vlen);
			_Concat(s, string, slen);
		}
	}


	void append( const WCHAR * string, size_t len )
	{
		size_t slen = min(len, (string ? wcslen(string) + 1 : 0));
		size_t vlen = slen + size();
		if (vlen == 0) {
			clear();
		} else if (slen != 0) {
			reserve(vlen);
			_Concat(s, string, slen);
		}
	}

	void reserve( size_t len )
	{
		if (len >= n)
		{
			T *olds = s;
			size_t oldn = n;
			size_t a = AlignWord(oldn * 3 / 2), b = AlignWord(len+1);
			n = max(a, b);
			s = (T*)calloc(n, sizeof(T));
			if (olds) {
				_Copy(s, olds, oldn);
				free(olds);
			}
		}
	}

	void Format(const CHAR *format, ...)
	{
		if (s) *s = 0;
		va_list args;
		va_start(args, format);
		AppendFormatV(format, args);
		va_end(args);
	}

	void Format(const WCHAR *format, ...)
	{
		if (s) *s = 0;
		va_list args;
		va_start(args, format);
		AppendFormatV(format, args);
		va_end(args);
	}

	void AppendFormat(const CHAR *format, ...)
	{
		va_list args;
		va_start(args, format);
		AppendFormatV(format, args);
		va_end(args);
	}

	void AppendFormat(const WCHAR *format, ...)
	{
		va_list args;
		va_start(args, format);
		AppendFormatV(format, args);
		va_end(args);
	}

	void AppendFormatV(const CHAR *format, va_list args)
	{
		CHAR buffer[512];
		int nChars = _vsnprintf(buffer, _countof(buffer), format, args);
		if (nChars != -1) {
			append(buffer, nChars);
		} else {
			size_t Size = _vscprintf(format, args);
			size_t Len = Length();
			Resize(Len + Size);
#ifdef _UNICODE
			CHAR *buf = (CHAR *)_alloca((Size+1)*sizeof(CHAR));
			nChars = _vsnprintf(buf, Size+1, format, args);
			mbstowcs(data()+Len, buf, Size+1);
#else
			nChars = _vsnprintf(data()+Len, capacity(), format, args);
#endif
		}
	}

	void AppendFormatV(const WCHAR *format, va_list args)
	{
		WCHAR buffer[512];
		int nChars = _vsnwprintf(buffer, _countof(buffer), format, args);
		if (nChars != -1) {
			append(buffer, nChars);
		} else {
			size_t Size = _vsctprintf(format, args);
			size_t Len = Length();
			Resize(Len + Size);
#ifdef _UNICODE
			nChars = _vsnwprintf(data()+Len, capacity(), format, args);
#else
			WCHAR *buf = (WCHAR *)_alloca((Size+1)*sizeof(WCHAR));
			nChars = _vsnwprintf(buf, Size+1, format, args);
			wcstombs(data()+Len, buf, Size+1);
#endif
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
					memmove(s+off, s+off+l, sizeof(T) * (n-off-l+1));
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

	TextBase Substr(size_t off, size_t len)
	{
		if (isNull()) return TextBase();

		size_t l = Length();
		if ( off < l ) return TextBase();

		l = min(len, l-off);
		return TextBase(s+off, l);
	}

	void swap(TextBase& other)
	{
		size_t on = other.n; other.n = n; n = on;
		T *os = other.s; other.s = s; s = os;
	}

	size_t size() const {
		return _Size(s);
	}

	size_t Length() const {
		return size();
	}

	size_t capacity() const {
		return n;
	}

	bool isNull() const { return (n == 0) || (s == NULL) || (s[0] == 0); }

	operator LPTSTR() { return s; }
	operator LPCTSTR() const { return s; }
	T* data() { return s; }
	const T *c_str() const { return s; }

	TextBase& operator+=( CHAR c ) { append(c); return *this; }
	TextBase& operator+=( LPCSTR s ) { append(s); return *this; }
	TextBase& operator+=( WCHAR c ) { append(c); return *this; }
	TextBase& operator+=( LPCWSTR s ) { append(s); return *this; }
	TextBase& operator+=( const TextBase& t ) { append(t); return *this; }

	T& operator[](int idx) { return s[idx]; }
	T operator[](int idx) const { return s[idx]; }

	void toUpper() {
		for (size_t i=0; i<n; ++i)
			s[i] = toupper(s[i]);
	}
	void toLower() {
		for (size_t i=0; i<n; ++i)
			s[i] = tolower(s[i]);
	}
	void toMixed() {
		bool nextUpper = true;
		for (size_t i=0; i<n; ++i) {
			T val = s[i];
			if (isalpha(val)) {
				if (nextUpper) {
					s[i] = toupper(s[i]);
					nextUpper = false;
				} else {
					s[i] = tolower(s[i]);
				}
			} else {
				nextUpper = true;
			}
		}
	}
	void removeChar(T value) {
		for (size_t i=0; i<n; ) {
			if (s[i] == value) {
				remove(i);
			} else {
				++i;
			}
		}
	}
	void Trim() { 
		while(!isNull() && _istspace(s[0])) remove(0, 1); 
		int len = Length()-1;
		while (len >= 0 && _istspace(s[len])) remove(len--, 1);
	}
	int Compare(const T* value, bool ignoreCase = false) const {
		return _Compare(s, value, ignoreCase);
	}

private:
	T *s;
	size_t n;
};

#pragma endregion

typedef TextBase<TCHAR> Text;
typedef TextBase<CHAR> CText;
typedef TextBase<WCHAR> WText;

#ifdef _UTILITY_
_STD_BEGIN
template <> inline void swap<CText>(CText& lhs, CText& rhs) { lhs.swap(rhs); }
template <> inline void swap<WText>(WText& lhs, WText& rhs) { lhs.swap(rhs); }
_STD_END
#endif

#pragma region FixedString
/////////////////////////////////////
// FixedString

template<class T, size_t size, int padding='\0'>
class FixedString : TextHelper
{
public:
	FixedString() {
		memset(v_, padding, size*sizeof(T));
		v_[size] = 0;
	}
	FixedString(const T *string) {
		assign(string);
	}
	FixedString( const FixedString& other ) {
		memcpy(v_, other.v_, (size+1)*sizeof(T));
	}
	FixedString & operator=( const FixedString& other ) {
		memcpy(v_, other.v_, (size+1)*sizeof(T));
		return *this;
	}
	FixedString & operator=( const T * string ) {
		assign( string );
		return *this;
	}
	void assign( const char * string )
	{
		size_t len = string ? strlen(string) : 0;
		// assert(len <= size);
		len = min(len, size);
		_Copy(v_, string, len);
		memset(v_+len, padding, (size-len)*sizeof(T));
		v_[size] = 0;
	}
	void assign( const wchar_t * string )
	{
		size_t len = string ? wcslen(string) : 0;
		// assert(len <= size);
		len = min(len, size);
		_Copy(v_, string, len);
		memset(v_+len, padding, (size-len)*sizeof(T));
		v_[size] = 0;
	}
	void clear()
	{
		memset(v_, padding, size*sizeof(T));
		v_[size] = 0;
	}
	operator T*() { return v_; }
	operator const T*() const { return v_; }
	T* data() { return v_; }
	const T *c_str() const { return v_; }

	size_t max_size() const { return size; }

	int Compare(const T* value, bool ignoreCase = false) const {
		return _Compare(v_, value, ignoreCase);
	}

private:
	T v_[size+1];  //! Very important that this is only data for this class.
};

#if 0
template<size_t fixedsize, int padding>
inline bool operator==( FixedString<fixedsize, padding>& lhs, const TCHAR *rhs) {
	return (_tcsncmp(lhs.data(), rhs, fixedsize) == 0) ? true : false;
}

template<size_t fixedsize, int padding>
inline bool operator==( const TCHAR *lhs, FixedString<fixedsize, padding>& rhs) {
	return (_tcsncmp(lhs, rhs.data(), fixedsize) == 0) ? true : false;
}

template<size_t fixedsize, int padding>
inline bool operator==( FixedString<fixedsize, padding>& lhs, FixedString<fixedsize, padding>& rhs) {
	return (_tcsncmp(lhs.data(), rhs.data(), fixedsize) == 0) ? true : false;
}

template<size_t fixedsize, int padding>
inline bool operator!=( FixedString<fixedsize, padding>& lhs, const TCHAR *rhs) {
	return (_tcsncmp(lhs.data(), rhs, fixedsize) == 0) ? false : true ;
}

template<size_t fixedsize, int padding>
inline bool operator!=( const TCHAR *lhs, FixedString<fixedsize, padding>& rhs) {
	return (_tcsncmp(lhs, rhs.data(), fixedsize) == 0) ? false : true;
}

template<size_t fixedsize, int padding>
inline bool operator!=( FixedString<fixedsize, padding>& lhs, FixedString<fixedsize, padding>& rhs) {
	return (_tcsncmp(lhs.data(), rhs.data(), fixedsize) == 0) ? false : true;
}
#endif

#pragma endregion


#pragma region Helpers
// sprintf for Text without having to worry about buffer size.
extern Text FormatText(const TCHAR* format,...);
extern Text& Trim(Text&p);

// Enumeration support
typedef struct EnumLookupType {
	int value;
	const TCHAR *name;
} EnumLookupType;

extern Text EnumToString(int value, const EnumLookupType *table);
extern int StringToEnum(Text value, const EnumLookupType *table);
extern Text FlagsToString(int value, const EnumLookupType *table);
extern int StringToFlags(Text value, const EnumLookupType *table);


typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > _tstring;

// sprintf for std::string without having to worry about buffer size.
extern std::string FormatStringA(const CHAR* format,...);
extern std::wstring FormatStringW(const WCHAR* format,...);
extern _tstring FormatString(const char* format,...);
extern _tstring FormatString(const WCHAR* format,...);

#pragma endregion
