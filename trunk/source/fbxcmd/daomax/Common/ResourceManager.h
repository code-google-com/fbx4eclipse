#pragma once
#include <tchar.h>
#include <string>
#include <vector>
#include "daostream.h"

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > _tstring;
typedef std::vector<_tstring> _tstringlist;

class AppSettings;

class ResourceManager
{
	ResourceManager(void);
	~ResourceManager(void);
public:
	// Get the base inifile name
	static _tstring get_iniFile();

	// Locate a single file
	static _tstring FindFile(const _tstring& fname);

	// Locate a single file
	static _tstring FindFile(const _tstring& fname, const _tstringlist& paths);

	// Locate any filename that matches
	static bool FindFiles(const TCHAR * fileName, const _tstringlist& paths, _tstringlist& result);

	// Check whether the given file is a child of the root paths
	static bool IsFileInRootPaths(const _tstring& fname, const _tstringlist& paths);

	// Return the Relative Texture Path for filename or empty
	static _tstring GetRelativePath(const _tstring& fname, const _tstring& prefix, const _tstringlist& paths);

	// Locate the stream by name, also search ERF resources for specified stream
	static DAOStreamPtr OpenStream(const TCHAR * fileName);

	static void LoadSettings( AppSettings& settings );
};
