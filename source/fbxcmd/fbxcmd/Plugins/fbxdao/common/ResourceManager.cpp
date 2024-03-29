#include "StdAfx.h"
#include "ResourceManager.h"
#include "AppSettings.h"
#include <Shlobj.h>
#include "../fbxcmn/KFbxLog.h"

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

		extern HMODULE k_hModuleHandle;
		stringlist results;
		TCHAR inipath[MAX_PATH], inifile[MAX_PATH];
		GetModuleFileName(k_hModuleHandle, inipath, _countof(inipath));
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
			Text value;
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, lpLCData, _countof(lpLCData));
			value.append(lpLCData);
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, lpLCData, _countof(lpLCData));
			value.append('-');
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
		stringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>("OverrideDirs").c_str(), ",;", true);
		_tstring filePath = FindFile(pszFilePart, searchPath);
		if (!filePath.empty() && ( 0 == _taccess(filePath.c_str(), 04) ) )
		{
			GetFullPathName(filePath.c_str(), _countof(buffer), buffer, NULL);
			return _tstring(buffer);
		}
		_tstring erfFilePartList;
		LPCTSTR pszFileExt = PathFindExtension(pszFilePart);
		if ( erfExtensionMap.TryGetSetting(pszFileExt, erfFilePartList) ) {
			stringlist erfFileParts = TokenizeString(erfFilePartList.c_str(), ",;", true);
			for (stringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
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

	bool FindFiles(LPCTSTR fileName, stringlist& result)
	{
		// First check for relative paths
		bool ok = FindMatchingFiles(fileName, result);

		// next search override directories
		LPCTSTR pszFilePart = PathFindFileName(fileName);
		stringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>("OverrideDirs").c_str(), ",;", true);
		_tstring filePath = FindFile(pszFilePart, searchPath);
		ok |= FindMatchingFiles(filePath.c_str(), result);

		// now check out the ERF folders and likely unpack locations
		_tstring erfFilePartList;
		LPCTSTR pszFileExt = PathFindExtension(pszFilePart);
		if ( erfExtensionMap.TryGetSetting(pszFileExt, erfFilePartList) ) {
			stringlist erfFileParts = TokenizeString(erfFilePartList.c_str(), ",;", true);
			for (stringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				// user specified unpack location in INI
				_tstring erfFilePart = (*itr);
				_tstring erfOverride = erfOverrideDirs.GetSetting(erfFilePart, erfFilePart);
				erfOverride += _T("\\");
				erfOverride += pszFilePart;
				// assuming erfOverride is not absolute
				searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>(_T("ERFDirs")).c_str(), _T(",;"), true);
				stringlist filePaths;
				if ( FindFiles(erfOverride.c_str(), searchPath, filePaths) ) {
					for (stringlist::iterator fileItr = filePaths.begin(); fileItr != filePaths.end(); ++fileItr) {
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

	bool FindFiles(LPCTSTR fileName, const _tstringlist& paths, stringlist& result)
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


	bool FindExtensionERFs(LPCTSTR pszFileExt, stringlist& fileNames)
	{
		_tstring erfFilePartList;
		if ( erfExtensionMap.TryGetSetting(pszFileExt, erfFilePartList) ) {
			stringlist erfFileParts = TokenizeString(erfFilePartList.c_str(), ",;", true);
			for (stringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				// user specified unpack location in INI
				_tstring erfFilePart = (*itr);
				erfFilePart += ".*";
				stringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>(_T("ERFDirs")).c_str(), _T(",;"), true);
				if ( FindFiles(erfFilePart.c_str(), searchPath, fileNames) ) {
					return true;
				}
			}
		}
		return false;
	}


	DAOStreamPtr OpenStream( LPCTSTR fileName )
	{
		stringlist result;
		if ( 0 == _taccess(fileName, 04) )
		{
			TCHAR buffer[MAX_PATH];
			GetFullPathName(fileName, _countof(buffer), buffer, NULL);
			return DAOStreamPtr( DAOStream::Create(buffer, true));
		}

		// search local then search overrides
		LPCTSTR pszFilePart = PathFindFileName(fileName);
		stringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>("OverrideDirs").c_str(), ",;", true);
		if ( FindFiles(fileName, searchPath, result) )
		{
			return DAOStreamPtr( DAOStream::Create(result[0].c_str(), true));
		}

		// next lookup by extension and load file from ERF or ERF ubpack locations
		
		Text pszFileExt = PathFindExtension(pszFilePart);
		if (_tcsicmp(pszFileExt, ".xml") == 0)
		{
			Text value = pszFilePart;
			PathRemoveExtension(value.data());
			pszFileExt = PathFindExtension(value.data());
		}
		_tstring erfFilePartList;
		if ( erfExtensionMap.TryGetSetting(pszFileExt, erfFilePartList) ) {
			stringlist erfFileParts = TokenizeString(erfFilePartList.c_str(), ",;", true);
			for (stringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				_tstring erfFilePart = (*itr);
				_tstring erfOverride = erfOverrideDirs.GetSetting(erfFilePart, erfFilePart);
				erfOverride += _T("\\");
				erfOverride += pszFilePart;
				searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>(_T("ERFDirs")).c_str(), _T(",;"), true);
				stringlist filePaths;
				if ( FindFiles(erfOverride.c_str(), searchPath, filePaths) ) {
					for (stringlist::iterator fileItr = filePaths.begin(); fileItr != filePaths.end(); ++fileItr) {
						_tstring filePath = (*fileItr);
						if ( FindMatchingFiles(filePath.c_str(), result) )
						{
							return DAOStreamPtr( DAOStream::Create(result[0].c_str(), true));
						}
					}
				}
			}

			// finally locate the ERF file and extract file from that
			for (stringlist::iterator itr = erfFileParts.begin(); itr != erfFileParts.end(); ++itr) {
				_tstring erfFilePart = (*itr);
				WText filePart(pszFilePart);
				_tstring erfFileMatch = erfFilePart + _T(".*");
				stringlist erfFileList;
				if ( FindFiles(erfFileMatch.c_str(), searchPath, erfFileList) )
				{
					for (stringlist::iterator erfitr = erfFileList.begin(); erfitr != erfFileList.end(); ++erfitr) {
						_tstring erfFileName(*erfitr);

						DAOStreamPtr erfstream( DAOStream::Create(erfFileName.c_str(), true));

						ERFHeader hdr;
						if (erfstream->Read(&hdr, sizeof(hdr), 1) != 1) {
							KFbxLog::LogVerbose("Unable to to read '%s'", erfFileName.c_str() );
							continue;
						}
						if ( wcsncmp(hdr.hdr, L"ERF ", 4) != 0 
							|| wcsncmp(hdr.type, L"V2.0", 4) != 0){
							KFbxLog::LogVerbose("File is not a recognized ERF format: '%s'", erfFileName.c_str() );
							continue;
						}
						std::vector<FileReference> refs;
						refs.resize(hdr.nfiles);
						if (erfstream->Read(&refs[0], sizeof(FileReference), hdr.nfiles) != hdr.nfiles) {
							KFbxLog::LogVerbose("ERF File list could not be read: %s", erfFileName.c_str() );
							continue;
						}
						for ( FileReferenceIterator fitr = refs.begin(), fend = refs.end(); fitr != fend; ++fitr)
						{
							const FileReference& fref = *fitr;
							if ( wcsicmp(fref.filename, filePart.c_str() ) == 0) {
								return DAOStreamPtr( DAOOffsetStream::Create(erfstream, fref.offset, fref.length));
							}
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

bool ResourceManager::FindFiles( LPCTSTR fileName, const _tstringlist& paths, stringlist& result )
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