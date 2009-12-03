/**********************************************************************
*<
FILE: ImporterBase.h

DESCRIPTION:	Base Importer class

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include "stdafx.h"
#include "importerBase.h"
#include "../fbxcmn/KFbxLog.h"

#include "GFFFormat.h"
using namespace DAO;
using namespace DAO::GFF;

ImporterBase::ImporterBase( DAOReader *owner, KFbxScene* scene, KFbxStreamOptions* options, DAO::GFF::GFFFile &file ) : o(owner)
	, lScene(scene)
	, pStreamOptions(options)
	, mManager(owner->mManager)
	, gffFile(file)
{
}

void ImporterBase::SetUserPropString( KFbxNode* node, LPCTSTR name, LPCTSTR value )
{
	KFbxProperty p1 = KFbxProperty::Create(node, "UDP3DSMAX", DTString);
	p1.ModifyFlag(KFbxUserProperty::eUSER, true);
	KString strValue;
	p1.Get(&strValue, eSTRING);
	Text line = FormatText("%s = %s\r\n", name, value);
	strValue.Append(line);
	p1.Set( strValue );
}

void ImporterBase::SetUserPropInt( KFbxNode* node, LPCTSTR name, int value )
{
	KFbxProperty p1 = KFbxProperty::Create(node, "UDP3DSMAX", DTString);
	p1.ModifyFlag(KFbxUserProperty::eUSER, true);
	KString strValue;
	p1.Get(&strValue, eSTRING);
	Text line = FormatText("%s = %d\r\n", name, value);
	strValue.Append(line);
	p1.Set( strValue );
}

void ImporterBase::SetPropString( KFbxNode* node, LPCTSTR name, LPCTSTR value )
{
	KFbxProperty p1 = KFbxProperty::Create(node, name, DTString);
	p1.ModifyFlag(KFbxUserProperty::eUSER, true);
	KString strValue(value);
	p1.Set( strValue );
}

void ImporterBase::SetPropDouble( KFbxNode* node, LPCTSTR name, double value )
{
	KFbxProperty p1 = KFbxProperty::Create(node, name, DTDouble);
	p1.ModifyFlag(KFbxUserProperty::eUSER, true);
	p1.Set( value );
}

void ImporterBase::SetPropInt( KFbxNode* node, LPCTSTR name, int value )
{
	KFbxProperty p1 = KFbxProperty::Create(node, name, DTInteger);
	p1.ModifyFlag(KFbxUserProperty::eUSER, true);
	p1.Set( value );
}

void ImporterBase::SetPropBool( KFbxNode* node, LPCTSTR name, bool value )
{
	KFbxProperty p1 = KFbxProperty::Create(node, name, DTBool);
	p1.ModifyFlag(KFbxUserProperty::eUSER, true);
	p1.Set( value );
}

void ImporterBase::OpenFile( GFFFile& gffFile, LPCTSTR fileName, LPCSTR fileType )
{
	DAOStreamPtr stream = ResourceManager::OpenStream(fileName);
	if (!stream.isNull()) {
		gffFile.open( *stream );
	} else {
		KFbxLog::LogWarn("Unable to open '%s'", fileName);
	}
}
