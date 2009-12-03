// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "program.h"
#include "fbxutils.h"
#include "fbxsdk.h" 
#include "fbx_import.h" 
using namespace std;

#pragma comment(lib, "shlwapi.lib")

using namespace std;

#pragma warning(disable : 4073)

Cmd::CmdListType Cmd::CmdList;

struct ReaderFunctions
{
	KFbxReader::CreateFuncType createFuncType;
	KFbxReader::IOSettingsFillerFuncType ioSettingsFillerFuncType;
	KFbxReader::GetInfoFuncType getInfoFuncType;
};

struct WriterFunctions
{
	KFbxWriter::CreateFuncType createFuncType;
	KFbxWriter::IOSettingsFillerFuncType ioSettingsFillerFuncType;
	KFbxWriter::GetInfoFuncType getInfoFuncType;
};

extern "C" {
	typedef int (__stdcall * RegisterFBXPlugins) (KFbxSdkManager*, KFbxMemoryAllocator*
		, int *nReaders, ReaderFunctions** readers
		, int *nWriters, WriterFunctions** writers);

	typedef int (__stdcall * UnregisterFBXPlugins) (KFbxSdkManager*);
};

// sprintf for std::string without having to worry about buffer size.
std::string FormatString(const TCHAR* format,...)
{
	TCHAR buffer[512];
	std::string text;
	va_list args;
	va_start(args, format);
	int nChars = _vsntprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		text = buffer;
	} else {
		size_t Size = _vsctprintf(format, args);
		TCHAR* pbuf = (TCHAR*)_alloca(Size);
		nChars = _vsntprintf(pbuf, Size, format, args);
		text = pbuf;
	}
	va_end(args);
	return text;
}

// routine for parsing white space separated lines.  Handled like command line parameters w.r.t quotes.
void Cmd::ParseLine (
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

bool Cmd::ParseArgs(LPCTSTR line)
{
	int nargs = 0, nchars = 0;
	ParseLine(line, NULL, NULL, &nargs, &nchars);
	char **largv = (char **)malloc(nargs * sizeof(char *) + nchars * sizeof(char));
	ParseLine(line, largv, ((char*)largv) + nargs * sizeof(char*), &nargs, &nchars);
	bool rv = ParseArgs(nargs, largv);
	free(largv);
	return rv;
}

void Cmd::PrintHelp()
{
	char fullName[MAX_PATH], exeName[MAX_PATH];
	GetModuleFileName(NULL, fullName, MAX_PATH);
	_splitpath(fullName, NULL, NULL, exeName, NULL);
	cout << exeName << " built on " << __TIMESTAMP__ << endl
		<< "Usage: " << exeName << " <command> [-opts[modifiers]]" << endl
		<< endl
		<< "<Commands>" << endl;

	for (CmdListType::iterator itr = CmdList.begin(), end = CmdList.end(); itr != end; ++itr){
		Cmd* p = (*itr);
		cout << FormatString("  %-13s ", p->Name.c_str());
		p->HelpCmd(htShort);
		cout << endl;
	}
	cout << endl
		<< "<Global Switches>" << endl
		<< FormatString("  %-13s %s", "help", "List of additional help options") << endl
		<< endl;
}

Cmd* Cmd::GetCmd(std::string name)
{
	for (CmdListType::iterator itr = CmdList.begin(), end = CmdList.end(); itr != end; ++itr){
		Cmd* p = (*itr);
		if (0 == _tcsicmp(p->Name.c_str(), name.c_str())){
			return p;
		}
	}
	return NULL;
}
list<Cmd*> Cmd::GetCmds()
{
	list<Cmd*> list;
	for (CmdListType::iterator itr = CmdList.begin(), end = CmdList.end(); itr != end; ++itr){
		Cmd* p = (*itr);
		list.push_back(p);
	}
	return list;
}

bool Cmd::ParseArgs(int argc, char **argv)
{
	bool rv = false;
	try
	{
		if (argc == 0)
		{
			PrintHelp();
			return false;
		}
		else if (argv[0] && ( 0 == _tcsicmp(argv[0], "help")) )
		{
			if (argc > 1 && argv[1] && argv[1][0])
			{
				if (Cmd* p = GetCmd(argv[1])) {
					p->HelpCmd(htLong);
					return false;
				}
			}
			PrintHelp();
		}
		else
		{
			if (Cmd* p = GetCmd(argv[0])) {
				Program cmdLine(argc-1, &argv[1]);
				rv |= p->ExecuteCmd(cmdLine);
			}
			else
			{
				cerr << "Unknown command " << argv[0] <<endl;
				PrintHelp();
			}
		}
	}
	catch (exception* e)
	{
		cerr << "Exception occurred:" << endl;
		cerr << "  " << e->what() << endl;
	}
	catch (exception& e)
	{
		cerr << "Exception occurred:" << endl;
		cerr << "  " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception occurred." << endl;
	}
	return rv;
}



// MyMemoryAllocator : a custom memory allocator 
// to be used by the FBX SDK
class FbxCmdMemoryAllocator : public KFbxMemoryAllocator
{
public:
	FbxCmdMemoryAllocator() 
		: KFbxMemoryAllocator(MyMalloc, MyCalloc, MyRealloc, MyFree, MyMsize)
	{
	}

	~FbxCmdMemoryAllocator()  
	{
	}

	static void* MyMalloc(size_t pSize)       
	{
		//_heapchk();
		void *pv = malloc(pSize);
		//OutputDebugString( FormatText("malloc  0x%08x - 0x%08x (%d)\n", pv, (char*)pv + pSize, pSize) );
		return pv;
	}

	static void* MyCalloc(size_t pCount,size_t pSize)
	{
		//_heapchk();
		void *pv = calloc(pCount, pSize);
		//OutputDebugString( FormatText("calloc  0x%08x - 0x%08x (%d)\n", pv, (char*)pv + pCount * pSize, pCount * pSize) );
		return pv;
	}

	static void* MyRealloc(void* pData, size_t pSize)
	{
		//_heapchk();
		void *pv = realloc(pData, pSize);
		//OutputDebugString( FormatText("realloc 0x%08x - 0x%08x [0x%08x] (%d)\n", pv, (char*)pv + pSize, pData, pSize) );
		return pv;
	}

	static void  MyFree(void* pData)
	{
		//_heapchk();
		//OutputDebugString( FormatText("free    0x%08x\n", pData) );
		free(pData);
		//_heapchk();
	}

	static size_t MyMsize(void* pData)
	{
		//_heapchk();
		size_t sz = _msize(pData);
		//OutputDebugString( FormatText("msize   0x%08x = %d\n", pData, sz) );
		return sz;
	}
};


Program::Program(int argc, char **argv, bool zeroargs)
{
	pAllocator = new FbxCmdMemoryAllocator();

	KFbxSdkManager::SetMemoryAllocator(pAllocator);

	pSdkManager = KFbxSdkManager::Create();
	if (!pSdkManager)
		throw runtime_error("Unable to create the FBX SDK manager");

	stringlist paths, plugins;
	this->argc = argc;
	this->argv = argv;
	DefaultToUnknown();

	for (int i = 0; i < argc; i++)
	{
		char *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{
			switch ( tolower(arg[1]) )
			{
			case 'o':
				{
					const char *param = arg+2;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					if (outfile.empty())
					{
						outfile = param;
					}
					else
					{
						cerr << "ERROR: Input file already specified as \"" << infile << "\"" << endl;
					}
				}
				break;

			case 'i':
				{
					const char *param = arg+2;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					if (infile.empty())
					{
						infile = param;
					}
					else
					{
						cerr << "ERROR: Input file already specified as \"" << infile << "\"" << endl;
					}
				}
				break;

			case 'p':
				{
					const char *param = arg+2;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					paths.merge( TokenizeString(param, ",;") );
				}

			}
		}
	}
	if (!infile.empty() && outfile.empty()) {
		char path[MAX_PATH], drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
		_splitpath(infile.c_str(), drive, dir, fname, ext);
		strcat(fname, "_out");
		_makepath(path, drive, dir, fname, ext);
		outfile = path;
	}
	// Register third-party plugins
	if (paths.empty()) {
		paths.push_back( "." );

		TCHAR fullpath[MAX_PATH];
		GetModuleFileName(NULL, fullpath, _countof(fullpath));
		PathRemoveFileSpec(fullpath);
		paths.push_back( fullpath );

		PathAppend(fullpath, "plugins");
		paths.push_back( fullpath );
	}
	if ( KFbxIOPluginRegistry* pRegistry = pSdkManager->GetIOPluginRegistry() )
	{
		if ( FindFiles(paths, "*.dll", plugins) ) {
			for (stringlist::iterator itr = plugins.begin(); itr != plugins.end(); ++itr) {
				HMODULE hLib = GetModuleHandle((*itr).c_str());
				if (hLib != NULL)
					continue;
				hLib = LoadLibrary((*itr).c_str());
				if (hLib != NULL) {
					FARPROC proc = GetProcAddress(hLib, _TEXT("RegisterFBXPlugins"));
					if (proc != NULL) {
						RegisterFBXPlugins registerPlugins = (RegisterFBXPlugins)proc;
						int nReaders = 0, nWriters = 0;
						ReaderFunctions* readers = NULL;
						WriterFunctions* writers = NULL;
						int n = registerPlugins(pSdkManager, pAllocator, &nReaders, &readers, &nWriters, &writers);
						int firstPluginID, count;
						for (int i=0; i<nReaders; ++i){
							pRegistry->RegisterReader(readers[i].createFuncType, readers[i].getInfoFuncType, firstPluginID, count, readers[i].ioSettingsFillerFuncType);
						}
						for (int i=0; i<nWriters; ++i){
							pRegistry->RegisterWriter(writers[i].createFuncType, writers[i].getInfoFuncType, firstPluginID, count, writers[i].ioSettingsFillerFuncType);
						}
						pAllocator->mFreeHandler(readers);
						pAllocator->mFreeHandler(writers);
						this->plugins.insert(hLib);
					} else {
						FreeLibrary(hLib);
					}
				}
			}
		}
	}


	//DefaultToLatest();
}

Program::~Program()
{
	for (std::set<HMODULE>::iterator itr = this->plugins.begin(); itr != this->plugins.end(); ++itr) {
		HMODULE hLib = *itr;
		if (hLib != NULL) {
			FARPROC proc = GetProcAddress(hLib, _TEXT("UnregisterFBXPlugins"));
			if (proc != NULL) {
				UnregisterFBXPlugins unregisterPlugins = (UnregisterFBXPlugins)proc;
				int n = unregisterPlugins(pSdkManager);
			}
			FreeLibrary(hLib);
		}
	}
	this->plugins.clear();
	if (pSdkManager) {
		pSdkManager->UnloadPlugins();
		pSdkManager->Destroy();
	}
	for (std::set<HMODULE>::iterator itr = this->plugins.begin(); itr != this->plugins.end(); ++itr) {
		FreeLibrary(*itr);
	}
}

void SetWorkingDirectory()
{
	TCHAR fullpath[MAX_PATH];
	GetModuleFileName(NULL, fullpath, _countof(fullpath));
	PathRemoveFileSpec(fullpath);
	SetCurrentDirectory(fullpath);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//SetWorkingDirectory();
	return (Cmd::ParseArgs(argc-1, &argv[1]) ? 0 : 1);
}
