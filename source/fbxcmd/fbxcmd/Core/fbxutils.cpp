#include "stdafx.h"
#include "fbxutils.h"

// Macro to create a dynamically allocated strdup on the stack
#define STRDUPA(p) (_tcscpy((TCHAR*)alloca((_tcslen(p)+1)*sizeof(*p)),p))

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

bool wildmatch(const std::string& match, const std::string& value) 
{
   return (wildcmpi(match.c_str(), value.c_str())) ? true : false;
}

bool wildmatch(const stringlist& matches, const std::string& value)
{
   for (stringlist::const_iterator itr=matches.begin(), end=matches.end(); itr != end; ++itr){
      if (wildcmpi((*itr).c_str(), value.c_str()))
         return true;
   }
   return false;
}

// Tokenize a string using strtok and return it as a stringlist
stringlist TokenizeString(LPCTSTR str, LPCTSTR delims)
{
   stringlist values;
   LPTSTR buf = STRDUPA(str);
   for (LPTSTR p = _tcstok(buf, delims); p && *p; p = _tcstok(NULL, delims)){
      values.push_back(std::string(p));
   }
   return values;
}

// Enumeration Support
Text EnumToString(int value, const EnumLookupType *table) {
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (itr->value == value) return Text(itr->name);
	}
	return FormatText("%x", value);
}

int StringToEnum(Text value, const EnumLookupType *table) {
	Trim(value);
	if (value.isNull()) return 0;

	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (0 == _tcsicmp(value, itr->name)) return itr->value;
	}
	char *end = NULL;
	return (int)strtol(value, &end, 0);
}

Text FlagsToString(int value, const EnumLookupType *table) {
	Text sstr;
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (itr->value && (itr->value & value) == itr->value) {
			if (!sstr.isNull()) sstr += " | ";
			sstr += itr->name;
			value ^= itr->value;
		}
	}
	if (value == 0 && sstr.isNull()) {
		return EnumToString(value, table);
	}
	if (value != 0) {
		if (!sstr.isNull()) sstr += "|";
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


void ExpandFileList( stringlist& list )
{
	for (stringlist::iterator itr = list.begin(); itr != list.end(); ) 
	{
		if (NULL != strpbrk((*itr).c_str(), "?*"))
		{
			std::string str = (*itr);
			itr = list.erase(itr);

			WIN32_FIND_DATA FindFileData;
			ZeroMemory(&FindFileData, sizeof(FindFileData));

			char path[_MAX_PATH], filebuf[_MAX_PATH];
			strcpy(path, str.c_str());
			PathRemoveFileSpec(path);

			HANDLE hFind = FindFirstFile(str.c_str(), &FindFileData);
			if (hFind != INVALID_HANDLE_VALUE) {
				for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
					PathCombine(filebuf, path, FindFileData.cFileName);
					itr = list.insert(itr, std::string(filebuf));
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

bool FindFiles( const stringlist& list, const std::string& filespec, stringlist& files )
{
	bool retval = false;
	TCHAR srcpath[MAX_PATH], filebuf[MAX_PATH];

	for (stringlist::const_iterator itr = list.begin(); itr != list.end(); ++itr) 
	{
		std::string path = (*itr);
		PathCombine(srcpath, path.c_str(), filespec.c_str());

		WIN32_FIND_DATA FindFileData;
		ZeroMemory(&FindFileData, sizeof(FindFileData));

		HANDLE hFind = FindFirstFile(srcpath, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE) {
			for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
				PathCombine(filebuf, path.c_str(), FindFileData.cFileName);
				files.push_back(std::string(filebuf));
				retval = true;
			}
			FindClose(hFind);
		}
	}
	return retval;
}