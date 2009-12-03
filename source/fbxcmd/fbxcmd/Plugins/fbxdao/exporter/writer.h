#pragma once

#include "fbxdao.h"
#include <fbxsdk.h>
#include "../Common/ResourceManager.h"
#include "daolog.h"
#include "../fbxcmn/KFbxLog.h"

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


	DAOWriter(KFbxSdkManager &manager, KFbxExporter &exporter, int SubID, int ID);

	//VERY important to put the file close in the destructor
	virtual ~DAOWriter() ;

	virtual bool FileCreate(char* pFileName) ;
	virtual bool FileClose() ;
	virtual bool IsFileOpen();
	virtual KFbxStreamOptions* GetWriteOptions() ;
	virtual bool Write(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions) ;

	virtual bool PreprocessScene(KFbxScene &pScene);
	virtual bool PostprocessScene(KFbxScene &pScene);

	// Implementation
	bool WriteMMH( KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, DAOStreamPtr file );

public:
	KFbxSdkManager *mManager;
	KFbxExporter *mExporter;
	int mSubID;
	int mID;
	FixedString<CHAR, 260> filepath;
};
