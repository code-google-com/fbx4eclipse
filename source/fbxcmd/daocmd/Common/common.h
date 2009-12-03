#pragma once

#include <string>
#include <sstream>
// Common collections
#include "utility.h"

//////////////////////////////////////////////////////////////////////////


// Generic IniFile reading routine
template<typename T>
inline T GetIniValue(LPCTSTR Section, LPCTSTR Setting, T Default, LPCTSTR iniFileName){
	T v = Default;
	TCHAR buffer[1024];
	std::stringstream sstr;
	sstr << Default;
	buffer[0] = 0;
	if (0 < GetPrivateProfileString(Section, Setting, sstr.str().c_str(), buffer, sizeof(buffer), iniFileName)){
		std::stringstream sstr(buffer);
		sstr >> v;
		return v;
	}
	return Default;
}

// Specific override for int values
template<>
inline int GetIniValue<int>(LPCTSTR Section, LPCTSTR Setting, int Default, LPCTSTR iniFileName){
	return GetPrivateProfileInt(Section, Setting, Default, iniFileName);
}

// Specific override for int values
template<>
inline bool GetIniValue<bool>(LPCTSTR Section, LPCTSTR Setting, bool Default, LPCTSTR iniFileName){
	return GetPrivateProfileInt(Section, Setting, Default, iniFileName) ? true : false;
}

// Specific override for string values
template<>
inline std::string GetIniValue<std::string>(LPCTSTR Section, LPCTSTR Setting, std::string Default, LPCTSTR iniFileName){
	TCHAR buffer[1024];
	buffer[0] = 0;
	if (0 < GetPrivateProfileString(Section, Setting, Default.c_str(), buffer, sizeof(buffer), iniFileName)){
		return std::string(buffer);
	}
	return Default;
}

// Generic IniFile reading routine
template<typename T>
inline void SetIniValue(LPCTSTR Section, LPCTSTR Setting, T value, LPCTSTR iniFileName){
	std::stringstream sstr;
	sstr << value;
	WritePrivateProfileString(Section, Setting, sstr.str().c_str(), iniFileName);
}

// Specific override for string values
template<>
inline void SetIniValue<std::string>(LPCTSTR Section, LPCTSTR Setting, std::string value, LPCTSTR iniFileName){
	WritePrivateProfileString(Section, Setting, value.c_str(), iniFileName);
}

// Specific override for TSTR values
template<>
inline void SetIniValue<LPTSTR>(LPCTSTR Section, LPCTSTR Setting, LPTSTR value, LPCTSTR iniFileName){
	WritePrivateProfileString(Section, Setting, value, iniFileName);
}

template<>
inline void SetIniValue<LPCTSTR>(LPCTSTR Section, LPCTSTR Setting, LPCTSTR value, LPCTSTR iniFileName){
	WritePrivateProfileString(Section, Setting, value, iniFileName);
}

//////////////////////////////////////////////////////////////////////////

extern void ExpandFileList( stringlist& list );
void ExpandFileList( stringlist& list );
extern std::string GetIndirectValue(LPCSTR path);
extern NameValueCollection ReadIniSection(LPCTSTR Section, LPCTSTR iniFileName);
extern std::string ExpandQualifiers(const std::string& src, const NameValueCollection& map, bool expandMissingToEmpty=true);
extern std::string ExpandEnvironment(const std::string& src);
extern void ReadEnvironment(LPCTSTR Section, LPCTSTR iniFileName, NameValueCollection& Environment);

extern stringlist TokenizeCommandLine(LPCTSTR str, bool trim);
extern std::string JoinCommandLine(stringlist args);
bool FindMatchingFiles( LPCTSTR match, stringlist& result );