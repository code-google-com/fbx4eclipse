/**********************************************************************
*<
FILE: ExporterBase.h

DESCRIPTION:	Base Exporter class

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "fbxdao.h"
#include <fbxsdk.h>
#include "writer.h"
#include "GFFFormat.h"
#include "../Common/AppSettings.h"
#include "../fbxcmn/KFbxLog.h"

class ExporterBase
{
public:
	//////////////////////////////////////////////////////////////////////////
	DAOWriter *o;
	KFbxScene* lScene;
	KFbxStreamOptions* pStreamOptions;
	KFbxSdkManager *mManager;
	Text filename;

	//////////////////////////////////////////////////////////////////////////
	ExporterBase(DAOWriter *owner, KFbxScene* scene, KFbxStreamOptions* options, LPCSTR filename);

	//////////////////////////////////////////////////////////////////////////
	bool TryGetUserProperty(KFbxNode* node, LPCTSTR name, KFbxProperty& value);
	bool TryGetPropString(KFbxNode* node, LPCTSTR name, KString& value);
	bool TryGetPropDouble(KFbxNode* node, LPCTSTR name, double& value);
	bool TryGetPropInt   (KFbxNode* node, LPCTSTR name, int& value);
	bool TryGetPropBool  (KFbxNode* node, LPCTSTR name, bool& value);

	bool TryGetUserPropString(KFbxNode* node, LPCTSTR name, KString& value );
	bool TryGetUserPropInt(KFbxNode* node, LPCTSTR name, int& value );
};