#include "StdAfx.h"
#include "Common.h"
#include <malloc.h>
#include <io.h>

using namespace std;

// Macro to create a dynamically allocated strdup on the stack
#define STRDUPA(p) (_tcscpy((TCHAR*)alloca((_tcslen(p)+1)*sizeof(*p)),p))

NameValueCollection Environment;


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

		HANDLE hFind = FindFirstFile(match, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE) {
			for (BOOL next = TRUE ; next ; next = FindNextFile(hFind, &FindFileData)) {
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