/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/

#include <fbxsdk.h>
#include <fbxfilesdk/kfbxio/kfbxiosettings.h>

#include "Common.h"
#include <list>
#include <string>
#include <set>
#include <windows.h>
#include <Shlwapi.h>
#include <tchar.h>
#pragma comment(lib, "shlwapi.lib")
#include "../fbxcmd/Plugins/fbxcmn/KFbxLog.h"

class ConsoleLogger : public ILogListener
{
	HANDLE  hError;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
public:
	ConsoleLogger()
	{
		hError = GetStdHandle( STD_ERROR_HANDLE );
		GetConsoleScreenBufferInfo( hError, &csbi );
	}

	void Message( KFbxLogLevel level, const TCHAR* strMessage )
	{
		switch (level)
		{
		case LOG_ERROR:
			SetConsoleTextAttribute( hError, FOREGROUND_RED | FOREGROUND_INTENSITY | (csbi.wAttributes & 0x00F0) );
			//_ftprintf( stderr, "ERROR:   %s\n", strMessage );
			fputs(strMessage, stderr); fputs("\n", stderr);
			SetConsoleTextAttribute( hError, csbi.wAttributes );
			break;
		case LOG_WARN: 
			SetConsoleTextAttribute( hError, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | (csbi.wAttributes & 0x00F0) );
			//_ftprintf( stderr, "WARN:    %s\n", strMessage );
			fputs(strMessage, stderr); fputs("\n", stderr);
			SetConsoleTextAttribute( hError, csbi.wAttributes );
			break;
		case LOG_INFO:
			SetConsoleTextAttribute( hError, csbi.wAttributes | FOREGROUND_INTENSITY);
			//_ftprintf( stderr, "INFO:    %s\n", strMessage );
			fputs(strMessage, stderr); fputs("\n", stderr);
			SetConsoleTextAttribute( hError, csbi.wAttributes );
			break;
		case LOG_DEBUG: 
			SetConsoleTextAttribute( hError,  (csbi.wAttributes & ~FOREGROUND_INTENSITY) );
			//_ftprintf( stderr, "DEBUG:   %s\n", strMessage );
			fputs(strMessage, stderr); fputs("\n", stderr);
			SetConsoleTextAttribute( hError, csbi.wAttributes );
			break;
		case LOG_VERBOSE: 
			SetConsoleTextAttribute( hError, FOREGROUND_BLUE | FOREGROUND_GREEN | (csbi.wAttributes & 0x00F0) );
			//_ftprintf( stderr, "VERBOSE: %s\n", strMessage );
			fputs(strMessage, stderr); fputs("\n", stderr);
			SetConsoleTextAttribute( hError, csbi.wAttributes );
			break;
		}
	}
};

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

typedef std::list<std::string> stringlist;
static std::set<HMODULE> k_plugins;
ConsoleLogger logger;


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
FbxCmdMemoryAllocator* pAllocator;

void InitializeSdkObjects(KFbxSdkManager*& pSdkManager, KFbxScene*& pScene)
{
	pAllocator = new FbxCmdMemoryAllocator();

	KFbxSdkManager::SetMemoryAllocator(pAllocator);

    // The first thing to do is to create the FBX SDK manager which is the 
    // object allocator for almost all the classes in the SDK.
    pSdkManager = KFbxSdkManager::Create();

    if (!pSdkManager)
    {
        printf("Unable to create the FBX SDK manager\n");
        exit(0);
    }

	// Load plugins from the executable directory
	KString lPath = KFbxGetApplicationDirectory();
#if defined(KARCH_ENV_WIN)
	KString lExtension = "dll";
#elif defined(KARCH_ENV_MACOSX)
	KString lExtension = "dylib";
#elif defined(KARCH_ENV_LINUX)
	KString lExtension = "so";
#endif
	//pSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	//////////////////////////////////////////////////////////////////////////
	KFbxLog::EnableLogging(true);
	KFbxLog::SetLogLevel( LOG_INFO );
	KFbxLog::AddListener(&logger);

	stringlist paths, plugins;

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
						k_plugins.insert(hLib);
					} else {
						FreeLibrary(hLib);
					}
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////


    // Create the entity that will hold the scene.
    pScene = KFbxScene::Create(pSdkManager,"");
}

void DestroySdkObjects(KFbxSdkManager* pSdkManager)
{
	KFbxLog::RemoveListener(&logger);

	for (std::set<HMODULE>::iterator itr = k_plugins.begin(); itr != k_plugins.end(); ++itr) {
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
	k_plugins.clear();

    // Delete the FBX SDK manager. All the objects that have been allocated 
    // using the FBX SDK manager and that haven't been explicitly destroyed 
    // are automatically destroyed at the same time.
    if (pSdkManager) pSdkManager->Destroy();
    pSdkManager = NULL;
}

bool SaveScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    KFbxExporter* lExporter = KFbxExporter::Create(pSdkManager, "");


	KFbxSdkManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    printf("FBX version number for this version of the FBX SDK is %d.%d.%d\n\n", lMajor, lMinor, lRevision);


    if( pFileFormat < 0 || pFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format if pEmbedMedia is true
        pFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        if (!pEmbedMedia)
        {
            //Try to export in ASCII if possible
            int lFormatIndex, lFormatCount = pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

            for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
            {
                if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
                {
                    KString lDesc =pSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                    char *lASCII = "ascii";
                    if (lDesc.Find(lASCII)>=0)
                    {
                        pFileFormat = lFormatIndex;
                        break;
                    }
                }
            }
        }
    }

	// Initialize the exporter by providing a filename.
	if(lExporter->Initialize(pFilename, pFileFormat) == false)
	{
		printf("Call to KFbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lExporter->GetLastErrorString());
		return false;
	}

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.

    IOSREF.SetBoolProp(EXP_FBX_MATERIAL,        true);
    IOSREF.SetBoolProp(EXP_FBX_TEXTURE,         true);
    IOSREF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
    IOSREF.SetBoolProp(EXP_FBX_SHAPE,           true);
    IOSREF.SetBoolProp(EXP_FBX_GOBO,            true);
    IOSREF.SetBoolProp(EXP_FBX_ANIMATION,       true);
    IOSREF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Export the scene.
    lStatus = lExporter->Export(pScene); 

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    int lFileFormat = -1;
    int i, lTakeCount;
    KString lCurrentTakeName;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

    if (!pSdkManager->GetIOPluginRegistry()->DetectFileFormat(pFilename, lFileFormat))
    {
        // Unrecognizable file format. Try to fall back to native format.
        lFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeReaderFormat();
    }

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, lFileFormat);
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        printf("Call to KFbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lImporter->GetLastErrorString());

        if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
            lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
        {
            printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access take information without
        // the expense of loading the entire file.

        printf("Take Information\n");

        lTakeCount = lImporter->GetTakeCount();

        printf("    Number of takes: %d\n", lTakeCount);
        printf("    Current take: \"%s\"\n", lImporter->GetCurrentTakeName());
        printf("\n");

        for(i = 0; i < lTakeCount; i++)
        {
            KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            printf("    Take %d\n", i);
            printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the take should be imported 
            // under a different name.
            printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the take should be not
            // be imported. 
            printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            printf("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOSREF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOSREF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOSREF.SetBoolProp(IMP_FBX_LINK,            true);
        IOSREF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOSREF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOSREF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOSREF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
    {
        printf("Please enter password: ");

        lPassword[0] = '\0';

        scanf("%s", lPassword);
        KString lString(lPassword);

        IOSREF.SetStringProp(IMP_FBX_PASSWORD,      lString);
        IOSREF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(pScene);

        if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
        {
            printf("\nPassword is wrong, import aborted.\n");
        }
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}
