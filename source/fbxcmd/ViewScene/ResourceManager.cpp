//#include "StdAfx.h"
#include "ResourceManager.h"
#include "AppSettings.h"
#include <Shlwapi.h>
#include <Shlobj.h>
#include <io.h>
#include <algorithm>
#include <functional>

//#include <windows.h>
//#include "../fbxcmn/KFbxLog.h"

struct ERFHeader
{
	wchar_t hdr[4];
	wchar_t type[4];
	unsigned long nfiles;
	unsigned long buildYear;
	unsigned long buildDay;
	unsigned long descriptionStrRef;
};

struct FileReference
{
	wchar_t filename[0x20];
	unsigned long offset;
	unsigned long length;
};

typedef std::vector<FileReference> FileReferenceVector;
typedef FileReferenceVector::iterator FileReferenceIterator;
typedef std::pair< FileReferenceIterator, FileReferenceIterator> FileReferenceRange ;


struct FileReferenceEquivalence
{
	bool operator()(const WCHAR* s1, const WCHAR* s2) const
	{ return wcsicmp(s1, s2) < 0; }

	bool operator()(const FileReference& s1, const FileReference& s2) const
	{ return wcsicmp(s1.filename, s2.filename) < 0; }

	bool operator()(const FileReference& s1, const WCHAR * s2) const
	{ return wcsicmp(s1.filename, s2) < 0; }

	bool operator()(const WCHAR * s1, const FileReference& s2) const
	{ return wcsicmp(s1, s2.filename) >= 0; }

};


bool FindMatchingFiles( LPCTSTR match, _tstringlist& result )
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


struct ResourceResolver
{
	bool initialized;
	_tstring iniFile;
	NameValueCollection env;
	AppSettings environmentSettings;
	AppSettings erfExtensionMap;
	AppSettings erfOverrideDirs;

	ResourceResolver()
		: environmentSettings(_T("Environment"))
		, erfExtensionMap(_T("ERFExtensionMap"))
		, erfOverrideDirs(_T("ERFOverrideDirs"))
		, initialized(false)
	{
	}

	void LoadResources()
	{
		if (initialized) return;
		initialized = true;

		_tstringlist results;
		TCHAR inipath[MAX_PATH], inifile[MAX_PATH];
		GetModuleFileName(NULL, inipath, _countof(inipath));
		PathRenameExtension(inipath, ".ini");
		iniFile = inipath;

		if ( FindMatchingFiles(inipath, results) ) {
			iniFile = results[0];
		} else {
			PathCombine(inifile, "plugcfg", PathFindFileName(inipath));
			PathRemoveFileSpec(inipath); // remove the file part
			PathAppend(inipath, inifile);
			if ( FindMatchingFiles(inipath, results) ) {
				iniFile = results[0];
			} else {
				PathRemoveFileSpec(inipath); // remove the file part
				PathAppend(inipath, inifile);
				if ( FindMatchingFiles(inipath, results) ) {
					iniFile = results[0];
				} else {
					//if (0 == _tcsicmp( PathFindFileName(inipath), "Debug" ) )
					//	PathRemoveFileSpec(inipath);
					//if (0 == _tcsicmp( PathFindFileName(inipath), "Release" ) )
					//	PathRemoveFileSpec(inipath);
					//if (0 == _tcsicmp( PathFindFileName(inipath), "Staging" ) )
					//	PathRemoveFileSpec(inipath);
					//PathAppend(inipath, "plugcfg");
					//PathAppend(inipath, "fbxdao.ini");
				}
			}
		}

		LoadEnvironment();
		LoadSettings(erfExtensionMap);
		LoadSettings(erfOverrideDirs);	
	}

	void LoadSettings( AppSettings& settings )
	{
		settings.ReadSettings(iniFile, env);
	}

	void LoadEnvironment()
	{
		//////////////////////////////////////////////////////////////////////////
		// Special overrides
		NameValueCollection settings;
		if (settings.find(_T("USERPROFILE_DOCUMENTS")) == settings.end()) {
			TCHAR pszPath[_MAX_PATH];
			if (S_OK == SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, (HANDLE)NULL, SHGFP_TYPE_CURRENT, pszPath) ) {
				settings[_T("USERPROFILE_DOCUMENTS")] = pszPath;
			}
		}
		if (settings.find(_T("USER_LOCALE")) == settings.end()) {
			TCHAR lpLCData[0x100];
			_tstring value;
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, lpLCData, _countof(lpLCData));
			value.append(lpLCData);
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, lpLCData, _countof(lpLCData));
			value.append(1, '-');
			value.append(lpLCData);
			settings[_T("USER_LOCALE")] = value.c_str();
		}

		environmentSettings.ReadSettings(iniFile, settings);
		environmentSettings.CopyTo(env);
	}


	_tstring FindFile(LPCTSTR fileName)
	{
		// first load
		TCHAR buffer[MAX_PATH];
		if ( 0 == _taccess(fileName, 04) )
		{
			GetFullPathName(fileName, _countof(buffer), buffer, NULL);
			return _tstring(buffer);
		}
		// next search overrides
		LPCTSTR pszFilePart = PathFindFileName(fileName);
		_tstringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>("OverrideDirs").c_str(), ",;", true);
		_tstring filePath = FindFile(pszFilePart, searchPath);
		if (!filePath.empty() && ( 0 == _taccess(filePath.c_str(), 04) ) )
		{
			GetFullPathName(filePath.c_str(), _countof(buffer), buffer, NULL);
			return _tstring(buffer);
		}
		_tstring erfFilePartList;
		LPCTSTR pszFileExt = PathFindExtension(pszFilePart);
		if ( erfExtensionMap.TryGetSetting(pszFileExt, erfFilePartList) ) {
			_tstringlist erfFileParts = TokenizeString(erfFilePartList.c_str(), ",;", true);
			for (_tstringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				// user specified unpack location in INI
				_tstring erfFilePart = (*itr);
				_tstring erfOverride = erfOverrideDirs.GetSetting(erfFilePart, erfFilePart);
				erfOverride += _T("\\");
				erfOverride += pszFilePart;
				searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>(_T("ERFDirs")).c_str(), _T(",;"), true);
				filePath = FindFile(erfOverride, searchPath);
				if (!filePath.empty() && ( 0 == _taccess(filePath.c_str(), 04) ) )
				{
					GetFullPathName(filePath.c_str(), _countof(buffer), buffer, NULL);
					return _tstring(buffer);
				}
			}
		}
		return _tstring(fileName);	
	}

	bool FindFiles(LPCTSTR fileName, _tstringlist& result)
	{
		// First check for relative paths
		bool ok = FindMatchingFiles(fileName, result);

		// next search override directories
		LPCTSTR pszFilePart = PathFindFileName(fileName);
		_tstringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>("OverrideDirs").c_str(), ",;", true);
		_tstring filePath = FindFile(pszFilePart, searchPath);
		ok |= FindMatchingFiles(filePath.c_str(), result);

		// now check out the ERF folders and likely unpack locations
		_tstring erfFilePartList;
		LPCTSTR pszFileExt = PathFindExtension(pszFilePart);
		if ( erfExtensionMap.TryGetSetting(pszFileExt, erfFilePartList) ) {
			_tstringlist erfFileParts = TokenizeString(erfFilePartList.c_str(), ",;", true);
			for (_tstringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				// user specified unpack location in INI
				_tstring erfFilePart = (*itr);
				_tstring erfOverride = erfOverrideDirs.GetSetting(erfFilePart, erfFilePart);
				erfOverride += _T("\\");
				erfOverride += pszFilePart;
				// assuming erfOverride is not absolute
				searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>(_T("ERFDirs")).c_str(), _T(",;"), true);
				_tstringlist filePaths;
				if ( FindFiles(erfOverride.c_str(), searchPath, filePaths) ) {
					for (_tstringlist::iterator fileItr = filePaths.begin(); fileItr != filePaths.end(); ++fileItr) {
						_tstring filePath = (*fileItr);
						ok |= FindMatchingFiles(filePath.c_str(), result);
					}
				}
			}
		}
		if (!ok)
			result.push_back(fileName);	
		return ok;
	}

	_tstring FindFile(const _tstring& fname, const _tstringlist& paths)
	{
		_tstringlist result;
		if ( FindFiles(fname.c_str(), paths, result) )
			return result[0];
		return fname;
	}

	bool FindFiles(LPCTSTR fileName, const _tstringlist& paths, _tstringlist& result)
	{
		TCHAR buffer[MAX_PATH];
		bool ok = FindMatchingFiles(fileName, result);
		for (_tstringlist::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr ){
			PathCombine(buffer, itr->c_str(), fileName);
			ok |= FindMatchingFiles(buffer, result);
		}
		return ok;
	}

	// Check whether the given file is a child of the root paths
	bool IsFileInRootPaths(const _tstring& fname, const _tstringlist& paths)
	{
		TCHAR root[MAX_PATH];
		TCHAR file[MAX_PATH];
		GetFullPathName(fname.c_str(), _countof(file), file, NULL);
		PathMakePretty(file);

		for (_tstringlist::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr ){
			GetFullPathName((*itr).c_str(), _countof(root), root, NULL);
			PathAddBackslash(root);
			PathMakePretty(root);
			if (-1 != _taccess(root,0)) {
				size_t len = _tcslen(root);
				if (0 == _tcsncmp(root, file, len))
					return true;
			}
		}
		return false;
	}

	// Return the Relative Texture Path for filename or empty
	_tstring GetRelativePath(const _tstring& fname, const _tstring& prefix, const _tstringlist& paths)
	{
		TCHAR buffer[MAX_PATH];
		//if (textureUseFullPath == 1) // full path name
		//{
		//	GetFullPathName(fname.c_str(), _countof(buffer), buffer, NULL);
		//	return string(buffer);
		//}
		//else if (textureUseFullPath == -1) // only filename
		//{
		//	return string(PathFindFileName(fname.c_str()));
		//}
		if (!PathIsRelative(fname.c_str())) 
		{
			TCHAR root[MAX_PATH];
			TCHAR file[MAX_PATH];
			GetFullPathName(fname.c_str(), _countof(file), file, NULL);
			PathMakePretty(file);

			for (_tstringlist::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr ){
				GetFullPathName((*itr).c_str(), _countof(root), root, NULL);
				PathAddBackslash(root);
				PathMakePretty(root);
				if (-1 != _taccess(root,0)) {
					size_t len = _tcslen(root);
					if (0 == _tcsnicmp(root, file, len))
						return _tstring(file+len);
				}
			}
		}
		else // Test if its relative to one of the specified root paths just return the texture 
		{
			for (_tstringlist::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr ){
				PathCombine(buffer, itr->c_str(), fname.c_str());
				if (-1 != _taccess(buffer, 0)){
					return fname;
				}
			}
		}

		// check if prefix is in place if so then just return fname as is
		if (_tcsnicmp(fname.c_str(), prefix.c_str(), prefix.size()) == 0)
		{
			return fname;
		}

		// Now just combine prefix with file portion of the name
		PathCombine(buffer, prefix.c_str(), PathFindFileName(fname.c_str()));
		return _tstring(buffer);
	}


	bool FindExtensionERFs(LPCTSTR pszFileExt, _tstringlist& fileNames)
	{
		_tstring erfFilePartList;
		if ( erfExtensionMap.TryGetSetting(pszFileExt, erfFilePartList) ) {
			_tstringlist erfFileParts = TokenizeString(erfFilePartList.c_str(), ",;", true);
			for (_tstringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				// user specified unpack location in INI
				_tstring erfFilePart = (*itr);
				erfFilePart += ".*";
				_tstringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>(_T("ERFDirs")).c_str(), _T(",;"), true);
				if ( FindFiles(erfFilePart.c_str(), searchPath, fileNames) ) {
					return true;
				}
			}
		}
		return false;
	}


	DAOStreamPtr OpenStream( LPCTSTR fileName )
	{
		_tstringlist result;
		// search local then search overrides
		LPCTSTR pszFilePart = PathFindFileName(fileName);
		_tstringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>("OverrideDirs").c_str(), ",;", true);
		if ( FindFiles(fileName, searchPath, result) )
		{
			return DAOStreamPtr( DAOStream::Create(result[0].c_str(), true));
		}

		// next lookup by extension and load file from ERF or ERF ubpack locations
		
		_tstring pszFileExt = PathFindExtension(pszFilePart);
		if (_tcsicmp(pszFileExt.c_str(), ".xml") == 0)
		{
			TCHAR value[MAX_PATH];
			_tcscpy(value, pszFilePart);
			PathRemoveExtension(value);
			pszFileExt = PathFindExtension(value);
		}
		_tstring erfFilePartList;
		if ( erfExtensionMap.TryGetSetting(pszFileExt.c_str(), erfFilePartList) ) {
			_tstringlist erfFileParts = TokenizeString(erfFilePartList.c_str(), ",;", true);
			for (_tstringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				_tstring erfFilePart = (*itr);
				_tstring erfOverride = erfOverrideDirs.GetSetting(erfFilePart, erfFilePart);
				erfOverride += _T("\\");
				erfOverride += pszFilePart;
				searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>(_T("ERFDirs")).c_str(), _T(",;"), true);
				_tstringlist filePaths;
				if ( FindFiles(erfOverride.c_str(), searchPath, filePaths) ) {
					for (_tstringlist::iterator fileItr = filePaths.begin(); fileItr != filePaths.end(); ++fileItr) {
						_tstring filePath = (*fileItr);
						if ( FindMatchingFiles(filePath.c_str(), result) )
						{
							return DAOStreamPtr( DAOStream::Create(result[0].c_str(), true));
						}
					}
				}
			}

			// finally locate the ERF file and extract file from that
			for (_tstringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				_tstring erfFilePart = (*itr);
				wchar_t filePart[MAX_PATH];
				mbstowcs(filePart, pszFilePart, strlen(pszFilePart)+1);

				_tstring erfFileMatch = erfFilePart + _T(".*");
				_tstringlist erfFileList;
				if ( FindFiles(erfFileMatch.c_str(), searchPath, erfFileList) )
				{
					for (_tstringlist::iterator erfitr = erfFileList.begin(); erfitr != erfFileList.end(); ++erfitr) {
						_tstring erfFileName(*erfitr);

						DAOStreamPtr erfstream( DAOStream::Create(erfFileName.c_str(), true));

						ERFHeader hdr;
						if (erfstream->Read(&hdr, sizeof(hdr), 1) != 1) {
							//KFbxLog::LogVerbose("Unable to to read '%s'", erfFileName.c_str() );
							continue;
						}
						if ( wcsncmp(hdr.hdr, L"ERF ", 4) != 0 
							|| wcsncmp(hdr.type, L"V2.0", 4) != 0){
							//KFbxLog::LogVerbose("File is not a recognized ERF format: '%s'", erfFileName.c_str() );
							continue;
						}
						std::vector<FileReference> refs;
						refs.resize(hdr.nfiles);
						if (erfstream->Read(&refs[0], sizeof(FileReference), hdr.nfiles) != hdr.nfiles) {
							//KFbxLog::LogVerbose("ERF File list could not be read: %s", erfFileName.c_str() );
							continue;
						}
						FileReferenceRange range = std::equal_range(refs.begin(), refs.end(), filePart, FileReferenceEquivalence() );
						if (range.first != range.second)
						{
							const FileReference& fref = *range.first;
							return DAOStreamPtr( DAOOffsetStream::Create(erfstream, fref.offset, fref.length));
						}
					}
				}
			}
		}
		return DAOStreamPtr(NULL);
	}


} theResolver;


_tstring ResourceManager::FindFile( const _tstring& fname )
{
	theResolver.LoadResources();
	return theResolver.FindFile(fname.c_str());
}

_tstring ResourceManager::FindFile( const _tstring& fname, const _tstringlist& paths )
{
	theResolver.LoadResources();
	return theResolver.FindFile(fname, paths);
}

bool ResourceManager::FindFiles( LPCTSTR fileName, const _tstringlist& paths, _tstringlist& result )
{
	theResolver.LoadResources();
	return theResolver.FindFiles(fileName, paths, result);
}

bool ResourceManager::IsFileInRootPaths( const _tstring& fname, const _tstringlist& paths )
{
	theResolver.LoadResources();
	return theResolver.IsFileInRootPaths(fname, paths);
}

_tstring ResourceManager::GetRelativePath( const _tstring& fname, const _tstring& prefix, const _tstringlist& paths )
{
	theResolver.LoadResources();
	return theResolver.GetRelativePath(fname, prefix, paths);
}

void ResourceManager::LoadSettings( AppSettings& settings )
{
	theResolver.LoadResources();
	return theResolver.LoadSettings(settings);
}

DAOStreamPtr ResourceManager::OpenStream( LPCTSTR fileName )
{
	theResolver.LoadResources();
	return theResolver.OpenStream(fileName);
}