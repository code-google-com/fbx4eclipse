#include "stdafx.h"
#include "fbxdao.h"
#include <fbxsdk.h>
#include "../fbxcmn/KFbxLog.h"
#include "writer.h"

#include <fbxfilesdk/fbxfilesdk_nsuse.h>


DAOWriter::DAOWriter( KFbxSdkManager &manager, KFbxExporter &exporter, int SubID, int ID ) 
	: base(manager, ID)
	, mManager(&manager)
	, mExporter(&exporter)
	, mSubID(SubID)
	, mID(ID)
{
}

DAOWriter::~DAOWriter()
{
	FileClose();
}

bool DAOWriter::FileCreate( char* pFileName )
{
	filepath = pFileName;
	return true;
}

bool DAOWriter::FileClose()
{
	filepath.clear();
	return true;
}

bool DAOWriter::IsFileOpen()
{
	return (filepath[0] != 0);
}

KFbxStreamOptions* DAOWriter::GetWriteOptions()
{
	//No need to worry, the exporter that calls this, will call destroy
	//on these stream options at the end of import.
	KFbxStreamOptions* lStreamOptions = KFbxStreamOptions::Create(mManager, "");
	return lStreamOptions;
}

bool DAOWriter::Write( KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions )
{
	if (!IsFileOpen())
	{
		KFbxLog::LogError("Trying to write to an closed stream");
		return false;
	}

	Text filename = PathFindFileName(filepath);
	LPCTSTR ext = PathFindExtension(filename);
	if (ext != NULL && _tcsicmp(ext, ".XML") == 0){
		PathRemoveExtension(filename);
		ext = PathFindExtension(filename);
	}
	if (ext != NULL && _tcsicmp(ext, ".MMH") == 0)
	{
		DAOStreamPtr file = DAOStream::Create(filepath, false);
		if (file.isNull())
		{
			KFbxLog::LogError("Could not create file: %s", filepath);
			return false;
		}
		return WriteMMH( pDocument, pStreamOptions, file );
	}
	else if (ext != NULL && _tcsicmp(ext, ".MSH") == 0)
	{
		KFbxLog::LogError("Writing MMH and MSH files is not supported yet.");
		return false;
		//return ReadMESH( pDocument, pStreamOptions, gffFile );
	}
	else
	{
		KFbxLog::LogError("Unknown filetype '%s'", (LPCTSTR)ext);
	}
	return false;
}

bool DAOWriter::PreprocessScene( KFbxScene &pScene )
{
	return false;
}

bool DAOWriter::PostprocessScene( KFbxScene &pScene )
{
	return false;
}

#pragma region Writer Settings
KFbxWriter* CreateWriter(KFbxSdkManager& pManager, KFbxExporter& pExporter, int pSubID, int pPluginID)
{
	return new DAOWriter(pManager, pExporter, pSubID, pPluginID) ;
}

void* GetWriterInfo(KFbxWriter::KInfoRequest pRequest, int pId)
{
	static char const* sExt[] = 
	{
		"mmh",
		"msh",
		"mmh.xml",
		"msh.xml",
		NULL
	};

	static char const* sDesc[] = 
	{
		"Dragon Age: Origins Model Hierarchy (*.mmh, *.mmh.xml)",
		"Dragon Age: Origins Mesh (*.msh, *.msh.xml)",
		"Dragon Age: Origins Model Hierarchy (*.mmh, *.mmh.xml)",
		"Dragon Age: Origins Mesh (*.msh, *.msh.xml)",
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

void FillWriterIOSettings(KFbxIOSettings& pIOS)
{
}
#pragma endregion