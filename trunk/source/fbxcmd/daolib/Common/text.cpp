#include <stdafx.h>
#include "stdio.h"
#include "Text.h"

// sprintf for Text without having to worry about buffer size.
Text FormatText(const TCHAR* format,...)
{
   TCHAR buffer[512];
   Text text;
   va_list args;
   va_start(args, format);
   int nChars = _vsntprintf(buffer, _countof(buffer), format, args);
   if (nChars != -1) {
      text = buffer;
   } else {
      size_t Size = _vsctprintf(format, args);
      text.Resize(int(Size));
      nChars = _vsntprintf(text.data(), Size, format, args);
   }
   va_end(args);
   return text;
}

Text& Trim(Text&p) { 
   while(!p.isNull() && _istspace(p[0])) p.remove(0, 1); 
   int len = p.Length()-1;
   while (len >= 0 && _istspace(p[len])) p.remove(len--, 1);
   return p;
}



// Enumeration Support
Text EnumToString(int value, const EnumLookupType *table) {
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (itr->value == value) return Text(itr->name);
	}
	return FormatText(_T("%.4X"), value);
}

int StringToEnum(Text value, const EnumLookupType *table) {
	Trim(value);
	if (value.isNull()) return 0;

	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (0 == _tcsicmp(value, itr->name)) return itr->value;
	}
	TCHAR *end = NULL;
	return (int)_tcstol(value, &end, 0);
}

Text FlagsToString(int value, const EnumLookupType *table) {
	Text sstr;
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (itr->value && (itr->value & value) == itr->value) {
			if (!sstr.isNull()) sstr += _T(" | ");
			sstr += itr->name;
			value ^= itr->value;
		}
	}
	if (value == 0 && sstr.isNull()) {
		return EnumToString(value, table);
	}
	if (value != 0) {
		if (!sstr.isNull()) sstr += _T("|");
		sstr += EnumToString(value, table);
	}
	return sstr;
}

int StringToFlags(Text value, const EnumLookupType *table) {
	int retval = 0;
	LPCTSTR start = value.data();
	LPCTSTR end = value.data() + value.Length();
	while(start < end) {
		LPCTSTR bar = _tcschr(start, '|');
		int len = (bar != NULL) ?  bar-start : end-start;
		Text subval = value.Substr(start-value.data(), len);
		retval |= StringToEnum(subval, table);
		start += (len + 1);
	}
	return retval;
}


// sprintf for std::string without having to worry about buffer size.
std::string FormatStringA(const CHAR* format,...)
{
	CHAR buffer[512];
	std::string text;
	va_list args;
	va_start(args, format);
	int nChars = _vsnprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		text = buffer;
	} else {
		size_t Size = _vscprintf(format, args);
		CHAR* pbuf = (CHAR*)_alloca(Size);
		nChars = _vsnprintf(pbuf, Size, format, args);
		text = pbuf;
	}
	va_end(args);
	return text;
}

std::wstring FormatStringW(const WCHAR* format,...)
{
	WCHAR buffer[512];
	std::wstring text;
	va_list args;
	va_start(args, format);
	int nChars = _vsnwprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		text = buffer;
	} else {
		size_t Size = _vscwprintf(format, args);
		WCHAR* pbuf = (WCHAR*)_alloca(Size);
		nChars = _vsnwprintf(pbuf, Size, format, args);
		text = pbuf;
	}
	va_end(args);
	return text;
}


// sprintf for _tstring without having to worry about buffer size.
_tstring FormatString(const char* format,...)
{
	char buffer[512];
	_tstring text;
	va_list args;
	va_start(args, format);
	int nChars = _vsnprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
#ifdef _UNICODE
		++nChars;
		wchar_t* pbuf = (wchar_t*)_alloca(nChars * sizeof(wchar_t));
		mbstowcs(pbuf, buffer, nChars);
		text = pbuf;
#else
		text = buffer;
#endif
	} else {
		size_t Size = _vscprintf(format, args) + 1;
		char* pbuf = (char*)_alloca(Size * sizeof(char));
		nChars = _vsnprintf(pbuf, Size, format, args);
#ifdef _UNICODE
		wchar_t* pbuf2 = (wchar_t*)_alloca(Size * sizeof(wchar_t));
		mbstowcs(pbuf2, pbuf, Size);
		text = pbuf2;
#else
		text = pbuf;
#endif
	}
	va_end(args);
	return text;
}

_tstring FormatString(const wchar_t* format,...)
{
	wchar_t buffer[512];
	_tstring text;
	va_list args;
	va_start(args, format);
	int nChars = _vsnwprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
#ifdef _UNICODE
		text = buffer;
#else
		++nChars;
		char* pbuf = (char*)_alloca(nChars * sizeof(char));
		wcstombs(pbuf, buffer, nChars);
		text = pbuf;
#endif
	} else {
		size_t Size = _vscwprintf(format, args);
		wchar_t* pbuf = (wchar_t*)_alloca(Size * sizeof(wchar_t));
		nChars = _vsnwprintf(pbuf, Size, format, args);
#ifdef _UNICODE
		text = pbuf;
#else
		char* pbuf2 = (char*)_alloca(Size * sizeof(char));
		wcstombs(pbuf2, pbuf, Size);
		text = pbuf2;
#endif
	}
	va_end(args);
	return text;
}
