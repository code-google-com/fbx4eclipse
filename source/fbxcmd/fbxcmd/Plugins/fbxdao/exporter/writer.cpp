#include "stdafx.h"
#include "fbxdao.h"
#include <fbxsdk.h>
#include "../fbxcmn/KFbxLog.h"

#include <fbxfilesdk/fbxfilesdk_nsuse.h>

class DAOWriter : public KFbxWriter
{
	typedef class KFbxWriter base;
public:
	void *operator new(size_t count) {
		return k_pAllocator->mMallocHandler(count);
	}

	void operator delete(void* _Ptr) throw( ) {
		k_pAllocator->mFreeHandler(_Ptr);
	}


	DAOWriter(KFbxSdkManager &manager, KFbxExporter &exporter, int SubID, int ID)
		: base(manager, ID), mManager(&manager), mExporter(&exporter), mSubID(SubID), mID(ID)
	{
	}

	//VERY important to put the file close in the destructor
	virtual ~DAOWriter() ;

	virtual bool FileCreate(char* pFileName) ;
	virtual bool FileClose() ;
	virtual bool IsFileOpen();
	virtual KFbxStreamOptions* GetWriteOptions() ;
	virtual bool Write(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions) ;

	virtual bool PreprocessScene(KFbxScene &pScene);
	virtual bool PostprocessScene(KFbxScene &pScene);

private:
	FILE *mFilePointer;
	KFbxSdkManager *mManager;
	KFbxExporter *mExporter;
	int mSubID;
	int mID;
};

DAOWriter::~DAOWriter()
{
	FileClose();
}

bool DAOWriter::FileCreate( char* pFileName )
{
	KFbxLog::LogError("Writing MMH and MSH files is not supported yet.");
	return false;
}

bool DAOWriter::FileClose()
{
	return false;
}

bool DAOWriter::IsFileOpen()
{
	return false;
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
	KFbxLog::LogError("Writing MMH and MSH files is not supported yet.");
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

void FillWriterIOSettings(KFbxIOSettings& pIOS)
{
}
#pragma endregion