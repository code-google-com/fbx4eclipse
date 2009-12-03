/**********************************************************************
*<
FILE: ImporterBase.h

DESCRIPTION:	Base Importer class

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "fbxdao.h"
#include <fbxsdk.h>
#include "reader.h"
#include "GFFFormat.h"
#include "../Common/AppSettings.h"
#include "../fbxcmn/KFbxLog.h"

class ImporterBase
{
public:
	//////////////////////////////////////////////////////////////////////////
	DAOReader *o;
	KFbxScene* lScene;
	KFbxStreamOptions* pStreamOptions;
	KFbxSdkManager *mManager;
	DAO::GFF::GFFFile &gffFile;

	//////////////////////////////////////////////////////////////////////////
	ImporterBase(DAOReader *owner, KFbxScene* scene, KFbxStreamOptions* options, DAO::GFF::GFFFile &file);

	void OpenFile(DAO::GFF::GFFFile& gffFile, LPCTSTR fileName, LPCSTR fileType = NULL);


	//////////////////////////////////////////////////////////////////////////
	template <class T>
	T GetProp(LPCTSTR name, T defaultValue)
	{
		//IOSREF.GetBoolProp(name, default);
		return defaultValue;
	}

	void SetUserPropString(KFbxNode* node, LPCTSTR name, LPCTSTR value );
	void SetUserPropInt(KFbxNode* node, LPCTSTR name, int value );
	void SetPropString(KFbxNode* node, LPCTSTR name, LPCTSTR value);
	void SetPropDouble(KFbxNode* node, LPCTSTR name, double value);
	void SetPropInt(KFbxNode* node, LPCTSTR name, int value);
	void SetPropBool(KFbxNode* node, LPCTSTR name, bool value);
};