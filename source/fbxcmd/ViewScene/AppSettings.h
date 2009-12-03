/**********************************************************************
*<
FILE: AppSettings.h

DESCRIPTION:	AppSetting helper class for managing game specific
file settings.

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma  once

#include <tchar.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <sstream>

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

	bool operator()(const wchar_t* s1, const wchar_t* s2) const
	{ return wcsicmp(s1, s2) < 0; }

	bool operator()(const std::wstring& s1, const std::wstring& s2) const
	{ return wcsicmp(s1.c_str(), s2.c_str()) < 0; }

	bool operator()(const std::wstring& s1, const wchar_t * s2) const
	{ return wcsicmp(s1.c_str(), s2) < 0; }

	bool operator()(const wchar_t * s1, const std::wstring& s2) const
	{ return wcsicmp(s1, s2.c_str()) >= 0; }
};

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > _tstring;
typedef std::vector<_tstring> _tstringlist;
typedef std::map<_tstring, _tstring, ltstr> NameValueCollection;
typedef std::basic_stringstream<TCHAR, std::char_traits<char>, std::allocator<char> > _tstringstream;

struct IniSetting
{
	_tstring val;
	bool iscomplex;
	bool ismodified;

	IniSetting() {}
	IniSetting(const _tstring& value) { val = value; }
	operator const _tstring& () const { return val; }
	operator _tstring () { return val; }
	IniSetting &operator=( const _tstring & value ) { 
		ismodified = (val != value);
		val = value; 
		return *this;
	}
};

struct IniSettingNameEquivalence : public ltstr
{
	bool operator()(const IniSetting& n1, const IniSetting& n2) const { 
		return ltstr::operator()(n1, n2);
	}
	bool operator()(const _tstring& n1, const IniSetting& n2) const { 
		return ltstr::operator()(n1, n2);
	}
	bool operator()(const IniSetting& n1, const _tstring& n2) const { 
		return ltstr::operator()(n1, n2);
	}
};

typedef std::map<_tstring, IniSetting, ltstr> IniSettingCollection;

class AppSettings
{
public:
	AppSettings(const _tstring& name) 
		: Name(name)
	{}

	_tstring Name;
	IniSettingCollection Environment;

	virtual void ReadSettings(const _tstring& iniFile);
	virtual void ReadSettings(const _tstring& iniFile, const NameValueCollection& environment);

	virtual void WriteSettings(const _tstring& iniFile);

	virtual bool CopyTo(NameValueCollection& env);

	#pragma region Get Value Helpers
	template<typename T>
	inline T GetSetting(const _tstring& setting){
		T v;
		IniSettingCollection::iterator itr = Environment.find(setting);
		if (itr != Environment.end()){
			_tstringstream sstr((*itr).second.val);
			sstr >> v;
		}
		return v;
	}
	template<>
	inline _tstring GetSetting(const _tstring& setting){
		IniSettingCollection::iterator itr = Environment.find(setting);
		if (itr != Environment.end())
			return (*itr).second.val;
		return _tstring();
	}
	template<>
	inline bool GetSetting(const _tstring& setting){
		return 0 != GetSetting<int>(setting);
	}

	template<typename T>
	inline T GetSetting(const _tstring& setting, T Default){
		IniSettingCollection::iterator itr = Environment.find(setting);
		if (itr != Environment.end()){
			T v;
			_tstringstream sstr((*itr).second.val);
			sstr >> v;
			return v;
		}
		return Default;
	}
	template<>
	inline _tstring GetSetting(const _tstring& setting, _tstring Default){
		IniSettingCollection::iterator itr = Environment.find(setting);
		if (itr != Environment.end())
			return (*itr).second.val;
		return Default;
	}
	template<>
	inline bool GetSetting(const _tstring& setting, bool Default){
		return 0 != GetSetting<int>(setting, Default ? 1 : 0);
	}

	#pragma endregion


	#pragma region Try Get Value Helpers
	template<typename T>
	inline bool TryGetSetting(const TCHAR * setting, T& v){
		IniSettingCollection::iterator itr = Environment.find(setting);
		if (itr != Environment.end()){
			_tstringstream sstr((*itr).second.val);
			sstr >> v;
			return true;
		}
		return false;
	}
	template<>
	inline bool TryGetSetting(const TCHAR * setting, _tstring& value){
		IniSettingCollection::iterator itr = Environment.find(setting);
		if (itr != Environment.end()) {
			value = (*itr).second.val;
			return true;
		}
		return false;
	}
	template<>
	inline bool TryGetSetting(const TCHAR * setting, bool& value){
		int ival;
		if ( TryGetSetting<int>(setting, ival)) {
			value = (ival != 0);
			return true;
		}
		return false;
	}
	#pragma endregion

	#pragma region Set Value Helpers
	template<typename T>
	inline void SetSetting(const _tstring& setting, T value){
		_tstringstream sstr;
		sstr << value;
		_tstring result = sstr.str();

		IniSettingCollection::iterator itr = Environment.find(setting);
		if (itr != Environment.end()) {
			(*itr).second = result;
		} else {
			Environment.insert(IniSettingCollection::value_type(setting, IniSetting(result)));
		}
	}
	template<>
	inline void SetSetting(const _tstring& setting, bool value){
		SetSetting<int>(setting, value ? 1 : 0);
	}

	#pragma endregion

	void SetAllUnmodified()
	{
		for (IniSettingCollection::iterator itr = Environment.begin(); itr != Environment.end(); ++itr) {
			itr->second.ismodified = false;
		}
	}
};

typedef std::list<AppSettings> AppSettingsMap;

struct AppSettingsNameEquivalence : public ltstr
{
	bool operator()(const AppSettings& n1, const AppSettings& n2) const { 
		return ltstr::operator()(n1.Name, n2.Name);
	}
	bool operator()(const _tstring& n1, const AppSettings& n2) const { 
		return ltstr::operator()(n1, n2.Name);
	}
	bool operator()(const AppSettings& n1, const _tstring& n2) const { 
		return ltstr::operator()(n1.Name, n2);
	}
};

// The Global Map
//  Global so that I dont have to parse the texture directories on every import
extern AppSettingsMap TheAppSettings;

inline AppSettings* FindAppSetting(const _tstring& name){
	AppSettingsNameEquivalence equiv;
	for (AppSettingsMap::iterator itr=TheAppSettings.begin(), end = TheAppSettings.end(); itr != end; ++itr){
		if (!equiv(*itr, name) && !equiv(name, *itr))
			return &(*itr);
	}
	return NULL;
}

_tstringlist TokenizeString(const TCHAR * str, const TCHAR * delims, bool trim);