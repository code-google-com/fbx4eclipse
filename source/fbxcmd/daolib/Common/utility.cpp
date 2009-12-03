#include "stdafx.h"
#include "utility.h"

// Macro to create a dynamically allocated strdup on the stack
#define STRDUPA(p) (_tcscpy((TCHAR*)alloca((_tcslen(p)+1)*sizeof(*p)),p))


#ifdef _UNICODE
std::wistream &_tcin = std::wcin;
std::wostream &_tcout = std::wcout;
std::wostream &_tcerr = std::wcerr;
std::wostream &_tclog = std::wclog;
#else
std::istream &_tcin = std::cin;
std::ostream &_tcout = std::cout;
std::ostream &_tcerr = std::cerr;
std::ostream &_tclog = std::clog;
#endif


extern int wildcmp(const TCHAR *wild, const TCHAR *string);
extern int wildcmpi(const TCHAR *wild, const TCHAR *string);

// Original Source: Jack Handy www.codeproject.com
int wildcmp(const TCHAR *wild, const TCHAR *string) {
   const TCHAR *cp, *mp;

   while ((*string) && (*wild != '*')) {
      if ((*wild != *string) && (*wild != '?')) {
         return 0;
      }
      wild++;
      string++;
   }

   while (*string) {
      if (*wild == '*') {
         if (!*++wild) {
            return 1;
         }
         mp = wild;
         cp = string+1;
      } else if ((*wild == *string) || (*wild == '?')) {
         wild++;
         string++;
      } else {
         wild = mp;
         string = cp++;
      }
   }

   while (*wild == '*') {
      wild++;
   }
   return !*wild;
}

// Same as above but case insensitive
int wildcmpi(const TCHAR *wild, const TCHAR *string) {
   const TCHAR *cp, *mp;
   int f,l;
   while ((*string) && (*wild != '*')) {
      f = _totlower( (_TUCHAR)(*string) );
      l = _totlower( (_TUCHAR)(*wild) );
      if ((f != l) && (l != '?')) {
         return 0;
      }
      wild++, string++;
   }
   while (*string) {
      if (*wild == '*') {
         if (!*++wild) return 1;
         mp = wild, cp = string+1;
      } else {
         f = _totlower( (_TUCHAR)(*string) );
         l = _totlower( (_TUCHAR)(*wild) );
         if ((f == l) || (l == '?')) {
            wild++, string++;
         } else {
            wild = mp, string = cp++;
         }
      }
   }
   while (*wild == '*') wild++;
   return !*wild;
}

bool wildmatch(const _tstring& match, const _tstring& value) 
{
   return (wildcmpi(match.c_str(), value.c_str())) ? true : false;
}

bool wildmatch(const stringlist& matches, const _tstring& value)
{
   for (stringlist::const_iterator itr=matches.begin(), end=matches.end(); itr != end; ++itr){
      if (wildcmpi((*itr).c_str(), value.c_str()))
         return true;
   }
   return false;
}

// Tokenize a string using strtok and return it as a stringlist
stringvector TokenizeString(LPCTSTR str, LPCTSTR delims, bool trim)
{
   stringvector values;
   LPTSTR buf = STRDUPA(str);
   for (LPTSTR p = _tcstok(buf, delims); p && *p; p = _tcstok(NULL, delims)){
	   values.push_back(_tstring((trim) ? Trim(p) : p));
   }
   return values;
}
//
//
//TSTR& Trim(TSTR&p) { 
//	while(!p.isNull() && _istspace(p[0])) p.remove(0, 1); 
//	int len = p.Length()-1;
//	while (len >= 0 && _istspace(p[len])) p.remove(len--, 1);
//	return p;
//}

void ExpandFileList( stringlist& list )
{
	for (stringlist::iterator itr = list.begin(); itr != list.end(); ) 
	{
		if (NULL != _tcspbrk((*itr).c_str(), _T("?*")))
		{
			_tstring str = (*itr);
			itr = list.erase(itr);

			WIN32_FIND_DATA FindFileData;
			ZeroMemory(&FindFileData, sizeof(FindFileData));

			TCHAR path[_MAX_PATH], filebuf[_MAX_PATH];
			_tcscpy(path, str.c_str());
			PathRemoveFileSpec(path);

			HANDLE hFind = FindFirstFile(str.c_str(), &FindFileData);
			if (hFind != INVALID_HANDLE_VALUE) {
				for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
					PathCombine(filebuf, path, FindFileData.cFileName);
					itr = list.insert(itr, _tstring(filebuf));
				}
				FindClose(hFind);
			}
		}
		else
		{
			++itr;
		}
	}
}

bool FindFiles( const stringlist& list, const _tstring& filespec, stringlist& files )
{
	bool retval = false;
	TCHAR srcpath[MAX_PATH], filebuf[MAX_PATH];

	for (stringlist::const_iterator itr = list.begin(); itr != list.end(); ++itr) 
	{
		_tstring path = (*itr);
		PathCombine(srcpath, path.c_str(), filespec.c_str());

		WIN32_FIND_DATA FindFileData;
		ZeroMemory(&FindFileData, sizeof(FindFileData));

		HANDLE hFind = FindFirstFile(srcpath, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE) {
			for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
				PathCombine(filebuf, path.c_str(), FindFileData.cFileName);
				files.push_back(_tstring(filebuf));
				retval = true;
			}
			FindClose(hFind);
		}
	}
	return retval;
}