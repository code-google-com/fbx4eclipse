#include "StdAfx.h"
#include "Common.h"
#include <malloc.h>

using namespace std;

// Macro to create a dynamically allocated strdup on the stack
#define STRDUPA(p) (_tcscpy((TCHAR*)alloca((_tcslen(p)+1)*sizeof(*p)),p))

NameValueCollection Environment;


//************************************
// Method:    ExpandFileList
// FullName:  ExpandFileList
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: stringlist & list
//************************************
void ExpandFileList( stringlist& list )
{
	for (stringlist::iterator itr = list.begin(); itr != list.end(); ) 
	{
		if (NULL != strpbrk((*itr).c_str(), "?*"))
		{
			string str = (*itr);
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
					itr = list.insert(itr, string(filebuf));
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

bool FindMatchingFiles( LPCTSTR match, stringlist& result )
{
	bool ok = false;
	TCHAR resultbuf[MAX_PATH];
	if (NULL != strpbrk(match, "?*"))
	{
		WIN32_FIND_DATA FindFileData;
		ZeroMemory(&FindFileData, sizeof(FindFileData));

		TCHAR path[_MAX_PATH], filebuf[_MAX_PATH];
		strcpy(path, match);
		PathRemoveFileSpec(path);

		bool ok = false;
		HANDLE hFind = FindFirstFile(match, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE) {
			for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
				PathCombine(filebuf, path, FindFileData.cFileName);
				GetFullPathName(filebuf, _countof(resultbuf), resultbuf, NULL);
				result.push_back(_tstring(resultbuf));
				ok = true;
			}
			FindClose(hFind);
		}
	}
	else
	{
		if ( 0 == _taccess(match, 04) )
		{
			GetFullPathName(match, _countof(resultbuf), resultbuf, NULL);
			result.push_back(_tstring(resultbuf));
			ok = true;
		}
	}
	return ok;
}



// Parse and ini file section and return the results as s NameValueCollection.
NameValueCollection ReadIniSection(LPCTSTR Section, LPCTSTR iniFileName )
{
	NameValueCollection map;
	DWORD len = 2048 * sizeof(TCHAR);
	LPTSTR buf = (LPTSTR)calloc(len+2, 1);
	while(NULL != buf) {
		DWORD rlen = GetPrivateProfileSection(Section, buf, len, iniFileName);
		if (rlen != (len-2)) break;
		len += 2;
		buf = (LPTSTR)realloc(buf, len);
	}
	if (NULL != buf) {
		for (LPTSTR line = buf, next = line + strlen(line) + 1; *line; line = next, next = line + strlen(line) + 1){
			Trim(line);
			if (line[0] == ';' || line[0] == 0) 
				continue;
			if (LPTSTR equals = _tcschr(line, TEXT('='))){
				*equals++ = 0;
				Trim(line), Trim(equals);
				map[string(line)] = string(equals);
			}
		}
	}
	return map;
}


// Expand Qualifiers in string using a ${Name} syntax.  Name will be looked up in the
//    NameValueCollection and expand in place.  Missing names will expand to empty.
//    - Please dont give self-referential strings
string ExpandQualifiers(const string& src, const NameValueCollection& map, bool expandMissingToEmpty)
{
	string value;
	bool hasPercent = false;
	string::size_type end = src.length();
	value.reserve(src.length());
	for (string::size_type i = 0; i < end; ++i) {
		if (src[i] == TEXT('$')) {
			if (++i < end) {
				if (src[i] == TEXT('$')){
					value.append(1, src[i]);
				} else if (src[i] == TEXT('{')) {
					string::size_type term = src.find_first_of(TEXT('}'), i);
					if (term == string::npos) {
						i = end;
					} else {
						string key = src.substr(i+1, term-i-1);
						NameValueCollection::const_iterator kvp = map.find(key);
						if (kvp != map.end()) {
							value.append(ExpandQualifiers(kvp->second, map, expandMissingToEmpty));
						} else if (!expandMissingToEmpty) {
							value.append("${");
							value.append(key);
							value.append("}");
						} else {

						}
						i = term;
					}
				} else if (src[i] == TEXT('(')) {
					string::size_type term = src.find_first_of(TEXT(')'), i);
					if (term == string::npos) {
						i = end;
					} else {
						string key = src.substr(i+1, term-i-1);
						NameValueCollection::const_iterator kvp = map.find(key);
						if (kvp != map.end()) {
							value.append(ExpandQualifiers(kvp->second, map, expandMissingToEmpty));
						} else if (!expandMissingToEmpty) {
							value.append("$(");
							value.append(key);
							value.append(")");
						} else {

						}
						i = term;
					}
				}
			} else {
				value.append(1, TEXT('$'));
			}
		} else {
			value.append(1, src[i]);
		}
	}
	return value;
}



// Call ExpandEnvironmentStrings but with std string instead
string ExpandEnvironment(const string& src)
{
	DWORD Size = ExpandEnvironmentStrings(src.c_str(), NULL, 0);
	if (TCHAR* pbuf = (TCHAR*)_alloca(Size+2)) {
		pbuf[0] = 0;
		ExpandEnvironmentStrings(src.c_str(), pbuf, Size+2);
		return string(pbuf);
	}
	return src;
}

void ReadEnvironment(LPCTSTR Section, LPCTSTR iniFileName, NameValueCollection& Environment)
{
	NameValueCollection settings = ReadIniSection(Section, iniFileName);

	// expand indirect values first
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = GetIndirectValue(itr->second.c_str());

	// next expand qualifiers
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = ExpandQualifiers(itr->second.c_str(), settings);

	// finally expand environment variables, last because it clobbers my custom qualifier expansion
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = ExpandEnvironment(itr->second);

	Environment.swap(settings);
}


// Helper struct and method for dealing with standard registry handles
struct str2hdl
{
	const TCHAR *str;
	HANDLE key;
};
const static struct str2hdl RegKeyMap[] = {
	{TEXT("HKLM"), HKEY_LOCAL_MACHINE},
	{TEXT("HKCU"), HKEY_CURRENT_USER},
	{TEXT("HKCR"), HKEY_CLASSES_ROOT},
	{TEXT("HKEY_LOCAL_MACHINE"), HKEY_LOCAL_MACHINE},
	{TEXT("HKEY_CURRENT_USER"), HKEY_CURRENT_USER},
	{TEXT("HKEY_CLASSES_ROOT"), HKEY_CLASSES_ROOT},
};
static HANDLE GetRegKey(LPCTSTR key) {
	for (int i=0; i<_countof(RegKeyMap); ++i)
		if (0 == _tcscmp(RegKeyMap[i].str, key))
			return RegKeyMap[i].key;
	return 0;
}

// Returns value from indirect source
//  Currently only supports registry string values using '[HKEY\Key]=@"Value"' 
//  where  HKEY is HKLM,HKCU,HKCR  
//         Key is the registry key to lookup
//         Value is the data value to lookup.  
string GetIndirectValue(LPCSTR path)
{
	if (!path || !*path)
		return string();
	string value;
	LPTSTR p = STRDUPA(path);
	Trim(p);
	if (*p == '['){
		LPTSTR end = _tcschr(++p, ']');
		if (end != NULL) {
			*end++ = 0;
			// Advance unsafely past unnecessary qualifiers
			LPTSTR valueName = end;
			end = valueName + strlen(end) - 1;
			if (*valueName == '=') ++valueName;
			if (*valueName == '@') ++valueName;
			if (*valueName == '\"' || *valueName == '\'') ++valueName;
			if (*end == '\"' || *end == '\'') *end-- = 0;
			Trim(valueName);

			LPTSTR keyEnd = _tcschr(p, '\\');
			if (keyEnd != NULL) {
				*keyEnd++ = 0;
				HANDLE hRoot = GetRegKey(p);
				if (hRoot != 0) {
					HKEY hKey = NULL;
					if (ERROR_SUCCESS == RegOpenKeyEx((HKEY)hRoot, keyEnd, 0, KEY_READ, &hKey)) {
						BYTE buffer[MAX_PATH*sizeof(*path)];
						DWORD dwLen = _countof(buffer);
						if (ERROR_SUCCESS == RegQueryValueEx(hKey, valueName, NULL, NULL, (LPBYTE)buffer, &dwLen) && dwLen > 0) {
							value = (TCHAR*)buffer;
						}
						RegCloseKey(hKey);
					}
				}
			}
		}
	} else {
		value = path;
	}
	return value;
}

// Original Source: Jack Handy www.codeproject.com
int wildcmp(const TCHAR *wild, const TCHAR *string) {
	const TCHAR *cp = NULL, *mp = NULL;
	if (wild == NULL) return (string == NULL);
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
			cp = string;
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
	const TCHAR *cp = NULL, *mp = NULL;
	int f,l;
	if (wild == NULL) return (string == NULL);
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
			mp = wild, cp = string;
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

bool wildmatch(const TCHAR* match, const TCHAR* value)
{
	return (wildcmpi(match, value)) ? true : false;
}

bool wildmatch(const string& match, const std::string& value) 
{
	return (wildcmpi(match.c_str(), value.c_str())) ? true : false;
}

bool wildmatch(const stringlist& matches, const TCHAR* value)
{
	for (stringlist::const_iterator itr=matches.begin(), end=matches.end(); itr != end; ++itr){
		if (wildcmpi((*itr).c_str(), value))
			return true;
	}
	return false;
}

bool wildmatch(const stringlist& matches, const std::string& value)
{
	return wildmatch(matches, value.c_str());
}

// routine for parsing white space separated lines.  Handled like command line parameters w.r.t quotes.
static void parse_line (
						const char *start,
						char **argv,
						char *args,
						int *numargs,
						int *numchars
						)
{
	const char NULCHAR    = '\0';
	const char SPACECHAR  = ' ';
	const char TABCHAR    = '\t';
	const char RETURNCHAR = '\r';
	const char LINEFEEDCHAR = '\n';
	const char DQUOTECHAR = '\"';
	const char SLASHCHAR  = '\\';
	const char *p;
	int inquote;                    /* 1 = inside quotes */
	int copychar;                   /* 1 = copy char to *args */
	unsigned numslash;              /* num of backslashes seen */

	*numchars = 0;
	*numargs = 0;                   /* the program name at least */

	p = start;

	inquote = 0;

	/* loop on each argument */
	for(;;) 
	{
		if ( *p ) { while (*p == SPACECHAR || *p == TABCHAR || *p == RETURNCHAR || *p == LINEFEEDCHAR) ++p; }

		if (*p == NULCHAR) break; /* end of args */

		/* scan an argument */
		if (argv)
			*argv++ = args;     /* store ptr to arg */
		++*numargs;

		/* loop through scanning one argument */
		for (;;) 
		{
			copychar = 1;
			/* Rules: 2N backslashes + " ==> N backslashes and begin/end quote
			2N+1 backslashes + " ==> N backslashes + literal "
			N backslashes ==> N backslashes */
			numslash = 0;
			while (*p == SLASHCHAR) 
			{
				/* count number of backslashes for use below */
				++p;
				++numslash;
			}
			if (*p == DQUOTECHAR) 
			{
				/* if 2N backslashes before, start/end quote, otherwise copy literally */
				if (numslash % 2 == 0) {
					if (inquote) {
						if (p[1] == DQUOTECHAR)
							p++;    /* Double quote inside quoted string */
						else        /* skip first quote char and copy second */
							copychar = 0;
					} else
						copychar = 0;       /* don't copy quote */

					inquote = !inquote;
				}
				numslash /= 2;          /* divide numslash by two */
			}

			/* copy slashes */
			while (numslash--) {
				if (args)
					*args++ = SLASHCHAR;
				++*numchars;
			}

			/* if at end of arg, break loop */
			if (*p == NULCHAR || (!inquote && (*p == SPACECHAR || *p == TABCHAR || *p == RETURNCHAR || *p == LINEFEEDCHAR)))
				break;

			/* copy character into argument */
			if (copychar) 
			{
				if (args)
					*args++ = *p;
				++*numchars;
			}
			++p;
		}

		/* null-terminate the argument */
		if (args)
			*args++ = NULCHAR;          /* terminate string */
		++*numchars;
	}
	/* We put one last argument in -- a null ptr */
	if (argv)
		*argv++ = NULL;
	++*numargs;
}

// Tokenize a string using strtok and return it as a stringlist
stringlist TokenizeString(LPCTSTR str, LPCTSTR delims, bool trim)
{
	stringlist values;
	if (str) {
		LPTSTR buf = STRDUPA(str);
		for (LPTSTR p = _tcstok(buf, delims); p && *p; p = _tcstok(NULL, delims)){
			values.push_back(string((trim) ? Trim(p) : p));
		}
	}
	return values;
}

// Tokenize a string using strtok and return it as a stringlist
stringlist TokenizeCommandLine(LPCTSTR str, bool trim)
{
	stringlist values;
	int nargs = 0, nchars = 0;
	parse_line( str, NULL, NULL, &nargs, &nchars);
	char **largv = (char **)_alloca(nargs * sizeof(TCHAR *) + nchars * sizeof(TCHAR));
	parse_line( str, largv, ((TCHAR*)largv) + nargs * sizeof(TCHAR*), &nargs, &nchars);
	for (int i=0; i<nargs; ++i) {
		LPTSTR p = largv[i];
		if (p == NULL) continue;
		values.push_back(string((trim) ? Trim(p) : p));
	}
	return values;
}

string JoinCommandLine(stringlist args)
{
	std::stringstream str;
	for (stringlist::iterator itr = args.begin(); itr != args.end(); ++itr) {
		if (itr != args.begin()) str << ' ';
		str << (*itr);
	}
	return str.str();
}
