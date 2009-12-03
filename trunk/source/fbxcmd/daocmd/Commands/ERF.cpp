#include "stdafx.h"
#include "program.h"
#include <sys/stat.h>
#include <io.h>
#include <shellapi.h>
using namespace std;

void FileFiles(vector<_tstring>& collection, const TCHAR *path)
{
	TCHAR buffer[MAX_PATH], dir[MAX_PATH], search[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	ZeroMemory(&FindFileData, sizeof(FindFileData));
	if (path == NULL || path[0] == 0)
		return;
	PathCanonicalize(search, path);
	if ( _tcspbrk(search, _T("*?")) == NULL ) {
		_tcscpy(dir, path);
		PathAddBackslash(search);
		_tcscat(search, _T("*"));
	} else {
		_tcscpy(dir, path);
		PathRemoveFileSpec(dir);
	}

	hFind = FindFirstFile(search, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		vector<_tstring> l;
		for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
			if (FindFileData.cFileName[0] == '.' || (FindFileData.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM)))
				continue;
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				PathCombine(buffer, dir, FindFileData.cFileName);
				PathAddBackslash(buffer);
				l.push_back( _tstring(buffer) );
			} else {
				PathCombine(buffer, dir, FindFileData.cFileName);
				GetLongPathName(buffer, buffer, MAX_PATH);
				collection.push_back( _tstring(buffer) );					
			}
		}
		FindClose(hFind);
		for (vector<_tstring>::iterator itr = l.begin(), end = l.end(); itr != end; ++itr) {
			FileFiles(collection, (*itr).c_str());
		}
	}
}


void CreateDirectories(LPCTSTR path)
{
	TCHAR tmp[260];
	GetFullPathName(path, _countof(tmp), tmp, NULL);

	if ( !CreateDirectory(tmp, NULL) ) {
		if (GetLastError() == ERROR_PATH_NOT_FOUND ) {
			PathRemoveFileSpec(tmp);
			CreateDirectories(tmp);
			CreateDirectory(path, NULL);
		}
	}
}


//////////////////////////////////////////////////////////////////////////


static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "Erf - Simple pack/unpack help for DA:O ERF files."; break;
	case Cmd::htLong:  
		{
			TCHAR fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_tsplitpath(fullName, NULL, NULL, exeName, NULL);
			cout << "Usage: " << exeName << " erf [-opts[modifiers]] <erf file>" << endl 
				<< "  Simple pack/unpack help for DA:O ERF files." << endl
				<< endl
				<< "  If input argument is a file then the program unpack the erf file." << endl
				<< "  If input argument is a path then the program packs an erf file from that path." << endl
				<< endl
				<< "<Switches>" << endl
				<< "  d <path>  Output directory or file" << endl
				<< "  v         Verbose Help (Lists specific versions)" << endl
				<< endl
				;
		}
		break;
	}
}

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

void EraseFileSpec(TCHAR * outdir)
{
	// Path must be double null terminated
	TCHAR path[MAX_PATH];
	_tcscpy(path, outdir);
	int len = _tcslen(path);
	path[len] = path[len + 1] = 0;


	SHFILEOPSTRUCT file;
	file.hwnd  =  NULL;
	file.wFunc =  FO_DELETE;
	file.pFrom =  path;
	file.pTo   =  NULL;
	file.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_ALLOWUNDO | FOF_NOERRORUI;
	file.fAnyOperationsAborted    =    0;
	file.hNameMappings =    NULL;

	SHFileOperation( &file );
}

static bool ExtractERFFiles( TCHAR * infile, TCHAR * outdir ) 
{
	static char buffer[65536];

	FILE* f = _tfopen(infile, _T("rb"));
	if (f == NULL){
		_ftprintf( stderr, _T("File could not be opened.\n") );
		return false;
	}

	EraseFileSpec(outdir);
	CreateDirectories(outdir);

	ERFHeader hdr;
	if (fread(&hdr, sizeof(hdr), 1, f) != 1) {
		_ftprintf( stderr, _T("File is not a recognized ERF format.\n") );
		return false;
	}
	if ( wcsncmp(hdr.hdr, L"ERF ", 4) != 0 
		|| wcsncmp(hdr.type, L"V2.0", 4) != 0){
			_ftprintf( stderr, _T("File is not a recognized ERF format.\n") );
			return false;
	}

	FileReference *refs = (FileReference *)calloc(hdr.nfiles, sizeof(FileReference));
	if (fread(refs, sizeof(FileReference), hdr.nfiles, f) != hdr.nfiles) {
		_ftprintf( stderr, _T("File list could not be read.\n") );
		free(refs);
		return false;
	}

	for (size_t i = 0; i < hdr.nfiles; ++i) {
		const FileReference &fref = refs[i];

		TCHAR fullname[260];
#ifdef UNICODE
		PathCombine(fullname, outdir, fref.filename);
#else
		char tmp[260];
		wcstombs(tmp, fref.filename, wcslen(fref.filename)+1);
		PathCombine(fullname, outdir, tmp);
#endif

		FILE* o = _tfopen(fullname, _T("wb"));
		if (o == NULL)
			continue;
		_ftprintf( stdout, _T("%s\n"), fullname);

		fseek(f, fref.offset, SEEK_SET);
		size_t left = fref.length;
		while ( left > 0 ) {
			size_t read = min( left, sizeof(buffer) );
			if (read == 0)
				break;
			size_t nread = fread(buffer, 1, read, f);
			if (nread == 0)
				break;
			if ( fwrite(buffer, 1, nread, o) != nread )
				break;
			left -= nread;
		}
		fclose(o);

	}

	free(refs);
	return true;
}

static bool PackERFFiles( TCHAR * matchFiles, TCHAR * outfile ) 
{
	static char buffer[65536];

	vector<_tstring> files;
	FileFiles(files, matchFiles);
	std::sort(files.begin(), files.end(), NumericStringEquivalence());
	if (files.size() > 0)
	{
		if (_taccess(outfile, 0) != -1)
		{
			TCHAR saveFile[MAX_PATH];
			_tcscpy(saveFile, outfile);
			_tcscat(saveFile, _T(".save"));
			if (_taccess(saveFile, 0) != -1)
				EraseFileSpec(saveFile);			
			MoveFileEx(outfile, saveFile, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED|MOVEFILE_WRITE_THROUGH);
		}

		FILE* f = _tfopen(outfile, _T("wb"));
		if (f == NULL){
			_ftprintf( stderr, _T("File '%s' could not be opened.\n"), outfile );
			return false;
		}

		unsigned long offset = 0;

		ERFHeader hdr;
		wcsncpy(hdr.hdr,  L"ERF ", 4);
		wcsncpy(hdr.type, L"V2.0", 4);
		hdr.nfiles = (unsigned int)files.size();
		hdr.buildYear = 109;
		hdr.buildDay = 266;
		hdr.descriptionStrRef = -1;

		offset += sizeof(hdr);
		offset += sizeof(FileReference) * hdr.nfiles;

		FileReference *refs = (FileReference *)calloc(hdr.nfiles, sizeof(FileReference));
		vector<_tstring>::iterator itr = files.begin();
		for (int i=0; i < (int)hdr.nfiles; ++i, ++itr)
		{
			FileReference &fref = refs[i];
			const TCHAR *fpath = (*itr).c_str();
			const TCHAR *fname = PathFindFileName(fpath);
#ifdef _UNICODE
			wcsncpy(fref.filename, fname, _countof(fref.filename));
#else
			mbstowcs(fref.filename, fname, _countof(fref.filename));
#endif
			struct _stat64 fstats;
			if ( -1 == _tstat64(fpath, &fstats) ){
				fref.length = 0;
			} else {
				bool ok = ((fstats.st_mode & (_S_IFDIR|_S_IFCHR)) != (_S_IFCHR|_S_IFDIR));
				fref.length = ok ? (unsigned long)fstats.st_size : 0;
			}
			fref.offset = offset;
			offset = AlignWord(offset + fref.length);
		}


		if ( fwrite(&hdr, sizeof(hdr), 1, f) != 1 )
		{
			_ftprintf( stderr, _T("File could not be written to.\n") );
		}
		if ( fwrite(refs, sizeof(FileReference), hdr.nfiles, f) != hdr.nfiles )
		{
			_ftprintf( stderr, _T("File could not be written to.\n") );
		}
		
		offset = 0;
		offset += sizeof(hdr);
		offset += sizeof(FileReference) * hdr.nfiles;

		itr = files.begin();
		for (int i=0; i < (int)hdr.nfiles; ++i, ++itr)
		{
			FileReference &fref = refs[i];
			const TCHAR *fpath = (*itr).c_str();

			FILE* infile = _tfopen(fpath, _T("rb"));
			if (infile == NULL)
				continue;
			_ftprintf( stdout, _T("%s\n"), fpath);

			for (size_t j=ftell(f); j < fref.offset; ++j)
				fwrite("\0", 1, 1, f);		

			offset = AlignWord(offset + fref.length);

			size_t left = fref.length;
			while ( left > 0 ) {
				size_t read = min( left, sizeof(buffer) );
				if (read == 0)
					break;
				size_t nread = fread(buffer, 1, read, infile);
				if (nread == 0)
					break;
				if ( fwrite(buffer, 1, nread, f) != nread )
					break;
				left -= nread;
			}
			for ( ; left > 0; --left)
				fwrite("\0", 1, 1, f);		
			fclose(infile);
		}
		free(refs);
		fclose(f);
	}

	return true;
}

static bool ExecuteCmd(Program &prog)
{
	TCHAR infile[MAX_PATH], fullpath[MAX_PATH];
	int argc = prog.argc;
	TCHAR **argv = prog.argv;
	bool verboseHelp = false;
	TCHAR path[MAX_PATH];

	if (argc == 0)
		return false;

	infile[0] = 0;
	path[0] = 0;
	for (int i = 0; i < argc; i++)
	{
		TCHAR *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{
			switch (arg[1])
			{
				case 'v':
					verboseHelp = true;
					break;

				case 'd':
					if (++i < argc) {
						_tcscpy(path, argv[i]);
					}
					break;

				default:
					_fputts( _T("ERROR: Unknown argument specified \""), stderr );
					_fputts( arg, stderr );
					_fputts( _T("\".\n"), stderr );
					return false;
			}
		}
		else if (infile[0] == 0)
		{
			_tcscpy(infile, arg);
		}
	}
	if (infile[0] == 0)
	{
		_ftprintf( stderr, _T("File not specified.\n") );
		return false;
	}

	TCHAR drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	GetFullPathName(infile, _countof(fullpath), fullpath, NULL);
	_tsplitpath(fullpath, drive, dir, fname, ext);
	if (path[0] == 0) {
		_tmakepath(path, drive, dir, fname, NULL);
	}

	struct _stat64 fstats;
	if ( -1 == _tstat64(fullpath, &fstats) ){
		_ftprintf( stderr, _T("Path is not a valid file or directory.\n") );
		return false;
	}

	if ( (fstats.st_mode & _S_IFDIR) == _S_IFDIR ) // file not directory
	{
		TCHAR filematch[MAX_PATH];
		PathCombine(filematch, fullpath, _T("*"));
		PathRemoveBackslash(path);
		_tcscat(path, _T(".erf"));
		return PackERFFiles(filematch, path);
	}
	else
	{
		return ExtractERFFiles(fullpath, path);
	}

	return false;
}

REGISTER_COMMAND(Erf, HelpString, ExecuteCmd);
