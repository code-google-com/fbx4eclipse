/**********************************************************************
*<
FILE: ExporterBase.h

DESCRIPTION:	Base Exporter class

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include "stdafx.h"
#include "exporterBase.h"
#include "../fbxcmn/KFbxLog.h"

#include "GFFFormat.h"
using namespace DAO;
using namespace DAO::GFF;

ExporterBase::ExporterBase( DAOWriter *owner, KFbxScene* scene, KFbxStreamOptions* options, LPCSTR filename ) 
	: o(owner)
	, lScene(scene)
	, pStreamOptions(options)
	, mManager(owner->mManager)
{
	this->filename = filename;
}