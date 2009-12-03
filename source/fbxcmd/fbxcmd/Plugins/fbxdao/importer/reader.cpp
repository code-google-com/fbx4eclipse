#include "stdafx.h"
#include "fbxdao.h"
#include <fbxsdk.h>
#include "reader.h"
#include "../fbxcmn/KFbxLog.h"

#include <fbxfilesdk/fbxfilesdk_nsuse.h>

using namespace std;

typedef std::map<std::string, KArrayKFbxNode, ltstr> NameNodeMap;
typedef NameNodeMap::iterator NameNodeItr;

namespace
{
	class Listener : public DAO::IDAOLogListener
	{
		KFbxLogLevel MapLevels(DAO::DAOLogLevel level) { return (KFbxLogLevel)(int)level; }

		virtual void Message( DAO::DAOLogLevel level, const TCHAR* strMessage ) 
		{
			KFbxLog::LogMessage( MapLevels(level), strMessage );
		}	
	};
	static Listener theListener;
}


DAOReader::DAOReader( KFbxSdkManager &manager, KFbxImporter &importer, int SubID, int ID ) 
	: base(manager, ID), mManager(&manager), mImporter(&importer), mSubID(SubID), mID(ID)
{
	fileOpen = false;
	DAO::DAOLog::AddListener( &theListener);
}

DAOReader::~DAOReader()
{
	FileClose();
	DAO::DAOLog::RemoveListener( &theListener);
}

bool DAOReader::FileOpen( char* pFileName )
{
	try
	{
		KFbxLog::LogDebug("DAOReader::FileOpen(%s)", pFileName);

		GetFullPathName(pFileName, filepath.max_size(), filepath.data(), NULL );
		gffFile.open(pFileName);
		fileOpen = true;
		return true;
	}
	catch (std::exception& ex)
	{
		mImporter->GetError().SetLastError(eDOCUMENT_NOT_SUPPORTED, ex.what());
		fileOpen = false;
		return false;
	}
}

bool DAOReader::FileOpen( KFile * pFile )
{
	return false;
}

bool DAOReader::FileClose()
{
	fileOpen = false;
	return true;
}

bool DAOReader::IsFileOpen()
{
	return fileOpen;
}

KFbxStreamOptions* DAOReader::GetReadOptions(bool pParseFileAsNeeded)
{
	//No need to worry, the exporter that calls this, will call destroy
	//on these stream options at the end of import.
	KFbxStreamOptions* lStreamOptions = KFbxStreamOptions::Create(mManager, "");
	return lStreamOptions;
}



bool DAOReader::Read( KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions )
{
	KString str = pDocument->GetName();
	KFbxLog::LogDebug("DAOReader::Read(%s)", (LPCTSTR)str);

	KFbxScene*      lScene = KFbxCast<KFbxScene>(pDocument);
	bool            lIsAScene = (lScene != NULL);
	bool            lResult = false;

	if(lIsAScene)
	{
		Text fileType = gffFile.get_FileInfo().Header.FileType;
		if ( fileType.Compare("MMH ", true) == 0)
		{
			return ReadMMH( pDocument, pStreamOptions, gffFile );
		}
		else if ( fileType.Compare("MESH", true) == 0 )
		{
			return ReadMESH( pDocument, pStreamOptions, gffFile );
		}
		else
		{
			KFbxLog::LogWarn("Unknown filetype '%s'", (LPCTSTR)fileType);
		}
	}
	return lResult;
}

//////////////////////////////////////////////////////////////////////////

KFbxReader* CreateReader(KFbxSdkManager& pManager, KFbxImporter& pImporter, int pSubID, int pPluginID)
{
	return new DAOReader(pManager, pImporter, pSubID, pPluginID) ;
}

void *GetReaderInfo(KFbxReader::KInfoRequest pRequest, int pId)
{
	static char const* sExt[] = 
	{
		"mmh",
		"msh",
		NULL
	};

	static char const* sDesc[] = 
	{
		"Dragon Age: Origins Model Hierarchy (*.mmh)",
		"Dragon Age: Origins Mesh (*.msh)",
		0
	};

	static char const* sVersions[] = 
	{
		"GFF_V4",
		0
	};

	switch (pRequest)
	{
	case KFbxWriter::eInfoExtension:
		return sExt;
	case KFbxWriter::eInfoDescriptions:
		return sDesc;
	case KFbxWriter::eInfoVersions:
		return sVersions;
	default:
		return 0;
	}
}

void FillReaderIOSettings(KFbxIOSettings& pIOS)
{
}
