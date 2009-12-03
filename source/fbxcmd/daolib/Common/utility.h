#pragma once

#ifndef _WINDOWS_
#  include <windows.h>
#endif
#include <tchar.h>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <map>

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <malloc.h>
#include <sstream>
#include <math.h>
#include "text.h"

template <class T> inline T TODEG(T x) { return (T)((double)x * 180.0 / M_PI); }
template <class T> inline T TORAD(T x) { return (T)((double)x * M_PI / 180.0); }

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > _tstring;
typedef std::vector<_tstring> _tstringlist;
typedef std::basic_stringstream<TCHAR, std::char_traits<char>, std::allocator<char> > _tstringstream;

#ifdef _UNICODE
extern std::wistream &_tcin ;
extern std::wostream &_tcout ;
extern std::wostream &_tcerr ;
extern std::wostream &_tclog ;
#else
extern std::istream &_tcin ;
extern std::ostream &_tcout ;
extern std::ostream &_tcerr ;
extern std::ostream &_tclog ;
#endif

// Trim whitespace before and after a string
inline TCHAR *Trim(TCHAR*&p) { 
   while(_istspace(*p)) *p++ = 0; 
   TCHAR *e = p + _tcslen(p) - 1;
   while (e > p && _istspace(*e)) *e-- = 0;
   return p;
}

// Case insensitive string equivalence test for collections
struct lText
{
   bool operator()(const TCHAR* s1, const TCHAR* s2) const
   { return _tcsicmp(s1, s2) < 0; }

   bool operator()(const _tstring& s1, const _tstring& s2) const
   { return _tcsicmp(s1.c_str(), s2.c_str()) < 0; }

   bool operator()(const _tstring& s1, const TCHAR * s2) const
   { return _tcsicmp(s1.c_str(), s2) < 0; }

   bool operator()(const TCHAR * s1, const _tstring& s2) const
   { return _tcsicmp(s1, s2.c_str()) >= 0; }
};

// Case insensitive string equivalence test for collections
struct ltstr
{
	bool operator()(const char* s1, const char* s2) const
	{ return stricmp(s1, s2) < 0; }

	bool operator()(const std::string& s1, const std::string& s2) const
	{ return stricmp(s1.c_str(), s2.c_str()) < 0; }

	bool operator()(const std::string& s1, const char * s2) const
	{ return stricmp(s1.c_str(), s2) < 0; }

	bool operator()(const char * s1, const std::string& s2) const
	{ return stricmp(s1, s2.c_str()) >= 0; }

	bool operator()(const WCHAR* s1, const WCHAR* s2) const
	{ return wcsicmp(s1, s2) < 0; }

	bool operator()(const std::wstring& s1, const std::wstring& s2) const
	{ return wcsicmp(s1.c_str(), s2.c_str()) < 0; }

	bool operator()(const std::wstring& s1, const WCHAR * s2) const
	{ return wcsicmp(s1.c_str(), s2) < 0; }

	bool operator()(const WCHAR * s1, const std::wstring& s2) const
	{ return wcsicmp(s1, s2.c_str()) >= 0; }

};

// Case insensitive string equivalence but numbers are sorted together
struct NumericStringEquivalence
{
	bool operator()(const TCHAR* s1, const TCHAR* s2) const
	{ return numstrcmp(s1, s2) < 0; }

	bool operator()(const _tstring& s1, const TCHAR* s2) const
	{ return numstrcmp(s1.c_str(), s2) < 0; }

	bool operator()(const TCHAR* s1, const _tstring& s2) const
	{ return numstrcmp(s1, s2.c_str()) < 0; }

	bool operator()(const _tstring& s1, const _tstring& s2) const
	{ return numstrcmp(s1.c_str(), s2.c_str()) < 0; }

	static int numstrcmp(const TCHAR *str1, const TCHAR *str2)
	{
		TCHAR *p1, *p2;
		int c1, c2, lcmp;
		for(;;)
		{
			c1 = tolower(*str1), c2 = tolower(*str2);
			if ( c1 == 0 || c2 == 0 )
				break;
			else if (isdigit(c1) && isdigit(c2))
			{			
				lcmp = _tcstol(str1, &p1, 10) - _tcstol(str2, &p2, 10);
				if ( lcmp == 0 )
					lcmp = (p2 - str2) - (p1 - str1);
				if ( lcmp != 0 )
					return (lcmp > 0 ? 1 : -1);
				str1 = p1, str2 = p2;
			}
			else
			{
				lcmp = (c1 - c2);
				if (lcmp != 0)
					return (lcmp > 0 ? 1 : -1);
				++str1, ++str2;
			}
		}
		lcmp = (c1 - c2);
		return ( lcmp < 0 ) ? -1 : (lcmp > 0 ? 1 : 0);
	}
};



typedef std::map<_tstring, _tstring, lText> NameValueCollection;
typedef std::pair<_tstring, _tstring> KeyValuePair;
typedef std::vector<_tstring> stringlist;
typedef std::vector<_tstring> stringvector;


extern int wildcmp(const TCHAR *wild, const TCHAR *string);
extern int wildcmpi(const TCHAR *wild, const TCHAR *string);

inline bool strmatch(const _tstring& lhs, const _tstring& rhs) {
   return (0 == _tcsicmp(lhs.c_str(), rhs.c_str()));
}
inline bool strmatch(const TCHAR* lhs, const _tstring& rhs) {
   return (0 == _tcsicmp(lhs, rhs.c_str()));
}
inline bool strmatch(const _tstring& lhs, const TCHAR* rhs) {
   return (0 == _tcsicmp(lhs.c_str(), rhs));
}
inline bool strmatch(const TCHAR* lhs, const TCHAR* rhs) {
   return (0 == _tcsicmp(lhs, rhs));
}

extern bool wildmatch(const _tstring& match, const _tstring& value);
extern bool wildmatch(const stringlist& matches, const _tstring& value);

extern stringlist TokenizeString(LPCTSTR str, LPCTSTR delims, bool trim=false);

// Generic IniFile reading routine
template<typename T, typename U>
inline T ConvertTo(U value){
   T v;
   stringstream istr;
   istr << value;

   stringstream ostr(istr.str());
   ostr >> v;
   return v;
}

extern bool FindFiles( const stringlist& list, const _tstring& filespec, stringlist& files );
