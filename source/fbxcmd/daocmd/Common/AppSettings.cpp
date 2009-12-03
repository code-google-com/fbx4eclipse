#include "stdafx.h"
#include "AppSettings.h"
#include <Shlobj.h>

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