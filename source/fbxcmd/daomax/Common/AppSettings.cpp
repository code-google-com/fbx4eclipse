#include "AppSettings.h"
#include <Shlobj.h>
#include <malloc.h>

#define STRDUPA(p) (_tcscpy((TCHAR*)alloca((_tcslen(p)+1)*sizeof(*p)),p))

// Parse and ini file section and return the results as s NameValueCollection.
NameValueCollection ReadIniSection(LPCTSTR Section, LPCTSTR iniFileName )
{
	NameValueCollection map;
	DWORD len = 2048 * sizeof(TCHAR);
	TCHAR* buf = (TCHAR*)calloc(len+2, 1);
	while(NULL != buf) {
		DWORD rlen = GetPrivateProfileSection(Section, buf, len, iniFileName);
		if (rlen != (len-2)) break;
		len += 2;
		buf = (TCHAR*)realloc(buf, len);
	}
	if (NULL != buf) 
	{
		for (TCHAR* line = buf, *next = line + _tcslen(line) + 1
			; *line
			; line = next, next = line + _tcslen(line) + 1)
		{
			Trim(line);
			if (line[0] == ';' || line[0] == 0) 
				continue;
			if (TCHAR* equals = _tcschr(line, TEXT('='))){
				*equals++ = 0;
				Trim(line), Trim(equals);
				map[_tstring(line)] = _tstring(equals);
			}
		}
	}
	return map;
}

_tstringlist TokenizeString(LPCTSTR str, LPCTSTR delims, bool trim)
{
	_tstringlist values;
	LPTSTR buf = STRDUPA(str);
	for (LPTSTR p = _tcstok(buf, delims); p && *p; p = _tcstok(NULL, delims)){
		values.push_back(_tstring((trim) ? Trim(p) : p));
	}
	return values;
}

// Expand Qualifiers in _tstring using a ${Name} syntax.  Name will be looked up in the
//    NameValueCollection and expand in place.  Missing names will expand to empty.
//    - Please dont give self-referential strings
_tstring ExpandQualifiers(const _tstring& src, const NameValueCollection& map, bool expandMissingToEmpty)
{
	_tstring value;
	bool hasPercent = false;
	_tstring::size_type end = src.length();
	value.reserve(src.length());
	for (_tstring::size_type i = 0; i < end; ++i) {
		if (src[i] == TEXT('$')) {
			if (++i < end) {
				if (src[i] == TEXT('$')){
					value.append(1, src[i]);
				} else if (src[i] == TEXT('{')) {
					_tstring::size_type term = src.find_first_of(TEXT('}'), i);
					if (term == _tstring::npos) {
						i = end;
					} else {
						_tstring key = src.substr(i+1, term-i-1);
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
					_tstring::size_type term = src.find_first_of(TEXT(')'), i);
					if (term == _tstring::npos) {
						i = end;
					} else {
						_tstring key = src.substr(i+1, term-i-1);
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


// Call ExpandEnvironmentStrings but with std _tstring instead
_tstring ExpandEnvironment(const _tstring& src)
{
	DWORD Size = ExpandEnvironmentStrings(src.c_str(), NULL, 0);
	if (TCHAR* pbuf = (TCHAR*)_alloca(Size+2)) {
		pbuf[0] = 0;
		ExpandEnvironmentStrings(src.c_str(), pbuf, Size+2);
		return _tstring(pbuf);
	}
	return src;
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
//  Currently only supports registry _tstring values using '[HKEY\Key]=@"Value"' 
//  where  HKEY is HKLM,HKCU,HKCR  
//         Key is the registry key to lookup
//         Value is the data value to lookup.  
_tstring GetIndirectValue(LPCSTR path)
{
	if (!path || !*path)
		return _tstring();
	_tstring value;
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



void ReadEnvironment(LPCTSTR Section, LPCTSTR iniFileName, NameValueCollection& Environment)
{
	NameValueCollection settings = ReadIniSection(Section, iniFileName);

	// expand indirect values first
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = GetIndirectValue(itr->second.c_str());

	// next expand qualifiers
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = ExpandQualifiers(itr->second.c_str(), settings, true);

	// finally expand environment variables, last because it clobbers my custom qualifier expansion
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = ExpandEnvironment(itr->second);

	Environment.swap(settings);
}


template<typename T>
inline T GetSetting(const _tstring& setting, NameValueCollection& settings){
	T v;
	NameValueCollection::iterator itr = settings.find(setting);
	if (itr != settings.end()){
		_tstringstream sstr((*itr).second);
		sstr >> v;
	}
	return v;
}
template<>
inline _tstring GetSetting(const _tstring& setting, NameValueCollection& settings){
	NameValueCollection::iterator itr = settings.find(setting);
	if (itr != settings.end())
		return (*itr).second;
	return _tstring();
}
template<>
inline bool GetSetting(const _tstring& setting, NameValueCollection& settings){
	return 0 != GetSetting<int>(setting, settings);
}

template<typename T>
inline T GetSetting(const _tstring& setting, T Default, NameValueCollection& settings){
	NameValueCollection::iterator itr = settings.find(setting);
	if (itr != settings.end()){
		T v;
		_tstringstream sstr((*itr).second);
		sstr >> v;
		return v;
	}
	return Default;
}
template<>
inline _tstring GetSetting(const _tstring& setting, _tstring Default, NameValueCollection& settings){
	NameValueCollection::iterator itr = settings.find(setting);
	if (itr != settings.end())
		return (*itr).second;
	return Default;
}

template<>
inline bool GetSetting(const _tstring& setting, bool Default, NameValueCollection& settings){
	return 0 != GetSetting<int>(setting, Default ? 1 : 0, settings);
}


void AppSettings::ReadSettings(const _tstring& iniFile)
{
	NameValueCollection envSettings;
	ReadSettings(iniFile, envSettings);
}

void AppSettings::ReadSettings(const _tstring& iniFile, const NameValueCollection& envSettings)
{
	NameValueCollection settings = ReadIniSection(Name.c_str(), iniFile.c_str());

	// expand indirect values first
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = GetIndirectValue(itr->second.c_str());

	// next expand local qualifiers
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = ExpandQualifiers(itr->second.c_str(), settings, false);

	// next expand system qualifiers
	if (!envSettings.empty()) {
		for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
			itr->second = ExpandQualifiers(itr->second.c_str(), envSettings, true);
	}

	// finally expand environment variables, last because it clobbers my custom qualifier expansion
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr)
		itr->second = ExpandEnvironment(itr->second);

	//std::swap(Environment, settings);

	// next expand system qualifiers
	for (NameValueCollection::iterator itr = settings.begin(), end = settings.end(); itr != end; ++itr) {
		Environment[(*itr).first] = (*itr).second;
	}
}


void AppSettings::WriteSettings(const _tstring& iniFile)
{
}

bool AppSettings::CopyTo(NameValueCollection& value)
{
	value.clear();

	// next expand system qualifiers
	for (IniSettingCollection::iterator itr = Environment.begin(), end = Environment.end(); itr != end; ++itr) {
		value[(*itr).first] = (*itr).second.val;
	}
	return !value.empty();
}