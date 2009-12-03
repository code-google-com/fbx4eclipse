#include <pch.h>
#include <vector>
#include <DelayImp.h>   // Required for hooking
#pragma comment(lib, "Delayimp.lib")

#pragma warning(disable : 4073)

extern HINSTANCE hInstance;

static std::set<HMODULE> k_plugins;
static KFbxSdkManager* k_pSdkManager;
static KFbxMemoryAllocator * k_pAllocator;
static bool k_fbxInitialized = false;

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
		void *pv = malloc(pSize);
		//OutputDebugString( FormatText("malloc  0x%08x - 0x%08x (%d)\n", pv, (char*)pv + pSize, pSize) );
		return pv;
	}

	static void* MyCalloc(size_t pCount,size_t pSize)
	{
		void *pv = calloc(pCount, pSize);
		//OutputDebugString( FormatText("calloc  0x%08x - 0x%08x (%d)\n", pv, (char*)pv + pCount * pSize, pCount * pSize) );
		return pv;
	}

	static void* MyRealloc(void* pData, size_t pSize)
	{
		void *pv = realloc(pData, pSize);
		//OutputDebugString( FormatText("realloc 0x%08x - 0x%08x [0x%08x] (%d)\n", pv, (char*)pv + pSize, pData, pSize) );
		return pv;
	}

	static void  MyFree(void* pData)
	{
		//OutputDebugString( FormatText("free    0x%08x\n", pData) );
		free(pData);
	}

	static size_t MyMsize(void* pData)
	{
		size_t sz = _msize(pData);
		//OutputDebugString( FormatText("msize   0x%08x = %d\n", pData, sz) );
		return sz;
	}
};

bool FindFiles( const stringlist& list, const std::string& filespec, stringlist& files )
{
	bool retval = false;
	TCHAR srcpath[MAX_PATH], filebuf[MAX_PATH];

	for (stringlist::const_iterator itr = list.begin(); itr != list.end(); ++itr) 
	{
		std::string path = (*itr);
		PathCombine(srcpath, path.c_str(), filespec.c_str());

		WIN32_FIND_DATA FindFileData;
		ZeroMemory(&FindFileData, sizeof(FindFileData));

		HANDLE hFind = FindFirstFile(srcpath, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE) {
			for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
				PathCombine(filebuf, path.c_str(), FindFileData.cFileName);
				files.push_back(std::string(filebuf));
				retval = true;
			}
			FindClose(hFind);
		}
	}
	return retval;
}

extern "C"
bool IsFbxInitialized()
{
	return k_fbxInitialized ;
}

extern "C"
KFbxSdkManager* GetFbxManager()
{
	return k_pSdkManager;
}

extern "C"
void InitializeFbxSettings()
{
	try
	{
		if (FAILED(__HrLoadAllImportsForDll("fbxcmn.dll"))) {
			k_fbxInitialized = false;
			return ;
		}

		k_pAllocator = new FbxCmdMemoryAllocator();
		KFbxSdkManager::SetMemoryAllocator(k_pAllocator);

		k_pSdkManager = KFbxSdkManager::Create();

		stringlist paths, plugins;
		if (paths.empty()) {
			paths.push_back( "." );

			TCHAR fullpath[MAX_PATH];
			GetModuleFileName(NULL, fullpath, _countof(fullpath));
			PathRemoveFileSpec(fullpath);
			paths.push_back( fullpath );

			PathAppend(fullpath, "plugins");
			paths.push_back( fullpath );
		}
		if ( KFbxIOPluginRegistry* pRegistry = k_pSdkManager->GetIOPluginRegistry() )
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
							int n = registerPlugins(k_pSdkManager, k_pAllocator, &nReaders, &readers, &nWriters, &writers);
							int firstPluginID, count;
							for (int i=0; i<nReaders; ++i){
								pRegistry->RegisterReader(readers[i].createFuncType, readers[i].getInfoFuncType, firstPluginID, count, readers[i].ioSettingsFillerFuncType);
							}
							for (int i=0; i<nWriters; ++i){
								pRegistry->RegisterWriter(writers[i].createFuncType, writers[i].getInfoFuncType, firstPluginID, count, writers[i].ioSettingsFillerFuncType);
							}
							k_pAllocator->mFreeHandler(readers);
							k_pAllocator->mFreeHandler(writers);
							k_plugins.insert(hLib);
						} else {
							FreeLibrary(hLib);
						}
					}
				}
			}
		}
		k_fbxInitialized = true;
	}
	catch(...)
	{
		k_fbxInitialized = false;
		// Handle the error. Errors will reach here only if
		// the hook function could not fix it.
	}
}

extern "C"
void FreeFbxSettings()
{
	{
		for (std::set<HMODULE>::iterator itr = k_plugins.begin(); itr != k_plugins.end(); ++itr) {
			HMODULE hLib = *itr;
			if (hLib != NULL) {
				FARPROC proc = GetProcAddress(hLib, _TEXT("UnregisterFBXPlugins"));
				if (proc != NULL) {
					UnregisterFBXPlugins unregisterPlugins = (UnregisterFBXPlugins)proc;
					int n = unregisterPlugins(k_pSdkManager);
				}
				FreeLibrary(hLib);
			}
		}
		k_plugins.clear();
		if (k_pSdkManager) {
			k_pSdkManager->UnloadPlugins();
			k_pSdkManager->Destroy();
		}
		for (std::set<HMODULE>::iterator itr = k_plugins.begin(); itr != k_plugins.end(); ++itr) {
			FreeLibrary(*itr);
		}
	}
}


// delayHookFunc - Delay load hooking function
FARPROC WINAPI delayHookFailureFunc (unsigned dliNotify, PDelayLoadInfo pdli)
{
	FARPROC fp = NULL;   // Default return value

	// NOTE: The members of the DelayLoadInfo structure pointed
	// to by pdli shows the results of progress made so far. 

	switch (dliNotify) {

case dliFailLoadLib:
	// LoadLibrary failed.
	// In here a second attempt could be made to load the dll somehow.
	// If fp is still NULL, the ERROR_MOD_NOT_FOUND exception will be raised.
	fp = NULL;
	{
		TCHAR fullpath[MAX_PATH];
		GetModuleFileName(hInstance, fullpath, _countof(fullpath));
		PathRemoveFileSpec(fullpath);
		PathAppend(fullpath, PathFindFileName(pdli->szDll) );
		pdli->hmodCur = LoadLibrary(fullpath);
		fp = (FARPROC)pdli->hmodCur;
	}

	break;

case dliFailGetProc:
	// GetProcAddress failed.
	// A second attempt could be made to get the function pointer somehow.
	// We can override and give our own function pointer in fp.
	// Ofcourse, fp is still going to be NULL,
	// the ERROR_PROC_NOT_FOUND exception will be raised.
	fp = (FARPROC) NULL;
	break;
	}

	return(fp);
}

// __delayLoadHelper gets the hook function in here:
//ExternC PfnDliHook   __pfnDliNotifyHook2 = delayHookNotifyFunc;
ExternC PfnDliHook   __pfnDliFailureHook2 = delayHookFailureFunc;



//////////////////////////////////////////////////////////////////////////


extern "C"
int GetFbxReaderFormats(std::vector<_tstring> &exts)
{
	if (k_pSdkManager != NULL)
	{
		KFbxIOPluginRegistry* pRegistry = k_pSdkManager->GetIOPluginRegistry();
		int nbReaders = pRegistry->GetReaderFormatCount();
		for(int i=0; i < nbReaders ; i++)
		{
			_tstring ext = pRegistry->GetReaderFormatExtension(i);
			if ( exts.end() == std::find(exts.begin(), exts.end(), ext))
				exts.push_back(ext);
		}
		return true;
	}
	return (int)exts.size();
}

extern "C"
int GetWriterFormats(std::vector<_tstring> &exts)
{
	if (k_pSdkManager != NULL)
	{
		KFbxIOPluginRegistry* pRegistry = k_pSdkManager->GetIOPluginRegistry();
		int nbWriters = pRegistry->GetWriterFormatCount();
		for(int i=0; i < nbWriters; i++)
		{
			_tstring ext = pRegistry->GetWriterFormatExtension(i);
			if ( exts.end() == std::find(exts.begin(), exts.end(), ext))
				exts.push_back(ext);
		}
	}
	return (int)exts.size();
}