#include "stdafx.h"
#include "program.h"
#include "../plugins/fbxcmn/KFbxLog.h"
using namespace std;

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


// to get a file extention for a WriteFileFormat
const char *GetFileFormatExt(KFbxSdkManager* pSdkManager, const int pWriteFileFormat)
{
	return pSdkManager->GetIOPluginRegistry()->GetWriterFormatExtension(pWriteFileFormat);
}

bool SaveScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename,  const char * pFileFormat=NULL, bool pEmbedMedia=false)
{
	int lMajor, lMinor, lRevision;
	bool lStatus = true;

	int lFileFormat = -1;
	int lFileFormatSubType = -1;
	KFbxIOPluginRegistry* pRegistry = pSdkManager->GetIOPluginRegistry();
	int nbWriters = pRegistry ? pRegistry->GetWriterFormatCount() : 0;

	if (pFileFormat != NULL && pFileFormat[0] != 0)
	{
		for(int i=0; i < nbWriters; i++)
		{
			if (char const* const* versions = pRegistry->GetWritableVersions(i)) {
				for(int j=0; versions && *versions; ++versions, ++j) {
					if (stricmp(*versions, pFileFormat) == 0){
						lFileFormat = i;
						lFileFormatSubType = j;
						break;
					} 
				}
			}
			if ( LPCSTR ext = pRegistry->GetWriterFormatExtension(i) ){
				if (stricmp(ext, pFileFormat) == 0) {
					lFileFormat = lFileFormat;
					break;
				}
			}
		}
	}


	// Create an exporter.
	KFbxExporter* lExporter = KFbxExporter::Create(pSdkManager, "");

	// Initialize the exporter by providing a filename.
	if(lExporter->Initialize(pFilename) == false)
	{
		KFbxLog::LogError("Call to KFbxExporter::Initialize() failed.");
		KFbxLog::LogError("Error returned: %s", lExporter->GetLastErrorString());
		return false;
	}

	KFbxIO::GetCurrentVersion(lMajor, lMinor, lRevision);
	KFbxLog::LogDebug("FBX version number for this version of the FBX SDK is %d.%d.%d", lMajor, lMinor, lRevision);

	// Set the file format
	if( lFileFormat < 0 || lFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
	{
		if ( LPSTR fileext = PathFindExtension(pFilename) )
		{
			if (fileext[0] == '.') ++fileext;

			for(int i=0; i < nbWriters; i++)
			{
				if ( LPCSTR ext = pRegistry->GetWriterFormatExtension(i) ){
					if (stricmp(ext, fileext) == 0) {

						if ( pSdkManager->GetIOPluginRegistry()->WriterIsFBX(i) )
						{
							if (!pEmbedMedia)
							{
								KString lDesc =pSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(i);
								char *lASCII = "ascii";
								if (lDesc.Find(lASCII)>=0)
								{
									lFileFormat = i;
									break;
								}
							}
						}
						else
						{

							lFileFormat = i;
						}
					}
				}
			}
		}
	}

	if( lFileFormat < 0 || lFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
	{
		if (!pSdkManager->GetIOPluginRegistry()->DetectFileFormat(pFilename, lFileFormat))
		{
			// Write in fall back format if pEmbedMedia is true
			lFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

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
							lFileFormat = lFormatIndex;
							break;
						}
					}
				}
			}
		}
	}

	LPCSTR ext = pRegistry->GetWriterFormatExtension(lFileFormat);
	LPCSTR desc = pRegistry->GetWriterFormatDescription(lFileFormat);

	lExporter->SetFileFormat(lFileFormat);
	if (lFileFormatSubType != -1)
		lExporter->SetFileExportVersion(KString(pFileFormat), KFbxSceneRenamer::eNONE);

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

	if (lStatus == false)
	{
		{
			KFbxIO::EError errcode = lExporter->GetLastErrorID();
			LPCSTR errstr = lExporter->GetLastErrorString();
			KFbxLog::LogError("Importer Error %d: %s", errcode, errstr);
		}
		{
			KFbxDocument::EError errcode = pScene->GetLastErrorID();
			LPCSTR errstr = pScene->GetLastErrorString();
			KFbxLog::LogError("Document Error %d: %s", errcode, errstr);
		}
	}

	// Destroy the exporter.
	lExporter->Destroy();
	return lStatus;
}

bool LoadScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename, const char * pFileFormat=NULL)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	bool lStatus;
	char lPassword[1024];

	//KFbxStreamOptionsFbxReader* lImportOptions=KFbxStreamOptionsFbxReader::Create(pSdkManager, "");

	int lFileFormat = -1;
	int lFileFormatSubType = -1;

	if (pFileFormat != NULL && pFileFormat[0] != 0)
	{
		if ( KFbxIOPluginRegistry* pRegistry = pSdkManager->GetIOPluginRegistry() )
		{
			int nbWriters = pRegistry->GetWriterFormatCount();
			for(int i=0; i < nbWriters; i++)
			{
				if (char const* const* versions = pRegistry->GetWritableVersions(i)){
					for(int j=0; versions && *versions; ++versions, ++j) {
						if (stricmp(*versions, pFileFormat) == 0){
							lFileFormat = i;
							lFileFormatSubType = j;
						} 
					}
				}
			}
		}
	}

	// Get the file version number generate by the FBX SDK.
	KFbxIO::GetCurrentVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

	if (lFileFormat == -1)
	{
		if (!pSdkManager->GetIOPluginRegistry()->DetectFileFormat(pFilename, lFileFormat))
		{
			// Unrecognizable file format. Try to fall back to native format.
			lFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeReaderFormat();
		}
	}
	lImporter->SetFileFormat(lFileFormat);

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename);
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if( !lImportStatus )
	{
		KFbxLog::LogError("Call to KFbxImporter::Initialize() failed.");
		KFbxLog::LogError("Error returned: %s", lImporter->GetLastErrorString());

		if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
			lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
		{
			KFbxLog::LogError("FBX version number for this FBX SDK is %d.%d.%d", lSDKMajor, lSDKMinor, lSDKRevision);
			KFbxLog::LogError("FBX version number for file %s is %d.%d.%d", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	KFbxLog::LogDebug("FBX version number for this FBX SDK is %d.%d.%d", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		KFbxLog::LogDebug("FBX version number for file %s is %d.%d.%d", pFilename, lFileMajor, lFileMinor, lFileRevision);

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

		//lImportOptions->SetOption(KFBXSTREAMOPT_FBX_PASSWORD, lString);
		//lImportOptions->SetOption(KFBXSTREAMOPT_FBX_PASSWORD_ENABLE, true);
		//lStatus = lImporter->Import(pScene, lImportOptions);

		if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
		{
			KFbxLog::LogError("\nPassword is wrong, import aborted.");
		}
	}
	else if (lStatus == false)
	{
		{
			KFbxIO::EError errcode = lImporter->GetLastErrorID();
			LPCSTR errstr = lImporter->GetLastErrorString();
			KFbxLog::LogError("Importer Error %d: %s", errcode, errstr);
		}
		{
			KFbxDocument::EError errcode = pScene->GetLastErrorID();
			LPCSTR errstr = pScene->GetLastErrorString();
			KFbxLog::LogError("Document Error %d: %s", errcode, errstr);
		}
	}
	if (lStatus == true)
	{

	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "Convert - Simple import/export."; break;
	case Cmd::htLong:  
		{
			char fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_splitpath(fullName, NULL, NULL, exeName, NULL);
			cout << "Usage: " << exeName << " convert [-opts[modifiers]]" << endl 
				<< "  Simply import and export input " << endl
				<< endl
				<< "<Switches>" << endl
				<< "  i <path>          Input File" << endl
				<< "  o <path>          Export File - Defaults to input file with '-out' appended" << endl
				<< "  fi <format>       Import file format (see Formats command with -v switch)" << endl
				<< "  fo <format>       Export file format (see Formats command with -v switch)" << endl
				<< "  embed             Embed media if supported by file format" << endl
				<< endl
				;
		}
		break;
	}
}

static bool Convert( string &infile, string &outfile, Program &prog, string &informat, string &outformat, bool embedMedia );

static bool ExecuteCmd(Program &prog)
{
	string infile = prog.infile;
	string outfile = prog.outfile;
	int argc = prog.argc;
	char **argv = prog.argv;
	string informat, outformat;
	bool embedMedia = false;

	list<Cmd *> plugins;

	bool ok = true;
	KFbxLog::EnableLogging(true);
	KFbxLog::SetLogLevel( LOG_INFO );
	ConsoleLogger logger;
	KFbxLog::AddListener(&logger);

	for (int i = 0; i < argc; i++)
	{
		char *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{
			switch (tolower(arg[1]))
			{
			case 'i':
				infile = ( arg[2] == ':' ) ? (arg + 3) : argv[++i];
				break;
			case 'o':
				outfile = ( arg[2] == ':' ) ? (arg + 3) : argv[++i];
				break;
			case 'f':
				switch (tolower(arg[2]))
				{
				case 'i': informat = ( arg[3] == ':' ) ? (arg + 4) : argv[++i]; break;
				case 'o': outformat = ( arg[3] == ':' ) ? (arg + 4) : argv[++i]; break;
				}			
				break;
			case 'e':
				embedMedia = true;
				break;

			case 'v':
				KFbxLog::SetLogLevel(LOG_VERBOSE);
				break;

			default:
				KFbxLog::LogError("ERROR: Unknown argument specified \"%s\"", arg);
				break;
			}
		}
		else if (infile.empty())
		{
			infile = arg;
		}
		else if (outfile.empty())
		{
			outfile = arg;
		}
	}
	if (infile.empty()){
		HelpString(Cmd::htLong);
		ok = false;
	}
	if (outfile.empty()) {
		char path[MAX_PATH], drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
		_splitpath(infile.c_str(), drive, dir, fname, ext);
		strcat(fname, "-out");
		_makepath(path, drive, dir, fname, ".fbx");
		GetFullPathName(path, _countof(path), path, NULL);
		outfile = path;
	}

	if (ok)
	{
		const char* pName = "";
		if ( KFbxScene* pScene = KFbxScene::Create(prog.pSdkManager, pName) )
		{
			// The first thing to do is to create the FBX SDK manager which is the 
			// object allocator for almost all the classes in the SDK.

			if (ok) ok = LoadScene(prog.pSdkManager, pScene, infile.c_str(), informat.c_str());

			if (ok) ok = SaveScene(prog.pSdkManager, pScene, outfile.c_str(), outformat.c_str(), embedMedia);

			if (ok)
			{
				if (_access(outfile.c_str(), 0) != -1) {
					KFbxLog::LogInfo("%s written.", outfile.c_str());
				} else {
					KFbxLog::LogWarn("%s does not exist. Conversion may have failed silently.", outfile.c_str());
				}
			}
			else
			{
				if (_access(outfile.c_str(), 0) != -1) {
					KFbxLog::LogError("%s present but conversion did not succeed.", outfile.c_str());
				} else {
					KFbxLog::LogWarn("%s conversion failed.", outfile.c_str());
				}
			}
		}
	}

	KFbxLog::RemoveListener(&logger);
	return ok;
}

REGISTER_COMMAND(Convert, HelpString, ExecuteCmd);