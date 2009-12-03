#pragma once

#include "fbxdao.h"
#include <fbxsdk.h>
#include "../Common/ResourceManager.h"
#include "daolog.h"
#include "../fbxcmn/KFbxLog.h"

class DAOReader : public KFbxReader
{
	typedef class KFbxReader base;
public:

	void *operator new(size_t count) {
		return k_pAllocator->mMallocHandler(count);
	}

	void operator delete(void* _Ptr) throw( ) {
		k_pAllocator->mFreeHandler(_Ptr);
	}


	DAOReader(KFbxSdkManager &manager, KFbxImporter &importer, int SubID, int ID);

	//VERY important to put the file close in the destructor
	virtual ~DAOReader() ;

	virtual bool FileOpen(char* pFileName);
	virtual bool FileOpen(KFile * pFile);
	virtual bool FileOpen(char* pFileName, EFileOpenSpecialFlags pFlags) { return FileOpen(pFileName); }

	virtual bool FileClose() ;
	virtual bool IsFileOpen();
	virtual KFbxStreamOptions* GetReadOptions(bool pParseFileAsNeeded = true) ;
	virtual bool Read(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions) ;


	virtual bool ReadMMH(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, DAO::GFF::GFFFile &file) ;
	virtual bool ReadMESH(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, DAO::GFF::GFFFile &file) ;

	void Reset();

	KFbxNode * ImportMESH(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, DAO::GFF::GFFFile &file,
		KFbxNode * pNode, DAO::MMH::MSHHRef meshRef, KArrayKFbxNode& bones );
public:
	bool fileOpen;
	DAO::GFF::GFFFile gffFile;
	KFbxSdkManager *mManager;
	KFbxImporter *mImporter;
	int mSubID;
	int mID;
	FixedString<CHAR, 260> filepath;
};