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
};