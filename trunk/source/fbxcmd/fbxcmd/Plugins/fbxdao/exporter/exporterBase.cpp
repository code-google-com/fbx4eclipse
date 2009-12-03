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

bool ExporterBase::TryGetPropString( KFbxNode* node, LPCTSTR name, KString& value )
{
	KFbxProperty lProperty;
	if ( TryGetUserProperty(node, name, lProperty) ) {
		value = lProperty.Get(&value);
		return true;
	}
	return false;
}

bool ExporterBase::TryGetPropDouble( KFbxNode* node, LPCTSTR name, double& value )
{
	KFbxProperty lProperty;
	if ( TryGetUserProperty(node, name, lProperty) ) {
		value = lProperty.Get(&value);
		return true;
	}
	return false;
}

bool ExporterBase::TryGetPropInt( KFbxNode* node, LPCTSTR name, int& value )
{
	KFbxProperty lProperty;
	if ( TryGetUserProperty(node, name, lProperty) ) {
		value = lProperty.Get(&value);
		return true;
	}
	return false;
}

bool ExporterBase::TryGetPropBool( KFbxNode* node, LPCTSTR name, bool& value )
{
	KFbxProperty lProperty;
	if ( TryGetUserProperty(node, name, lProperty) ) {
		value = lProperty.Get(&value);
		return true;
	}
	return false;
}

bool ExporterBase::TryGetUserProperty( KFbxNode* lNode, LPCTSTR name, KFbxProperty& value )
{
	int len = _tcslen(name);
	for ( KFbxProperty lProperty = lNode->GetFirstProperty()
		; lProperty.IsValid()
		; lProperty = lNode->GetNextProperty(lProperty)
		)
	{
		if (lProperty.GetFlag(KFbxUserProperty::eUSER))
		{
			KString lPropName = lProperty.GetName();
			if (lPropName.GetLen() < len)
				continue;
			if ( 0 == lPropName.Right( len ).CompareNoCase(name) )
			{
				value = lProperty;
				return true;
			}
		}
	}
	return false;
}

bool ExporterBase::TryGetUserPropString( KFbxNode* node, LPCTSTR name, KString& value )
{
	KString udpString;
	if ( TryGetPropString(node, "UDP3DSMAX", udpString) )
	{
		for (LPTSTR line = udpString.Buffer(), next = line + strlen(line) + 1
			; *line
			; line = next, next = line + strlen(line) + 1
			)
		{
			Trim(line);
			if (line[0] == ';' || line[0] == 0) 
				continue;
			if (LPTSTR equals = _tcschr(line, TEXT('='))){
				*equals++ = 0;
				Trim(line), Trim(equals);
				if (_tcsicmp(line, name) == 0)
				{
					value = equals;
					return true;
				}
			}
		}
	}
	return false;
}

bool ExporterBase::TryGetUserPropInt( KFbxNode* node, LPCTSTR name, int& value )
{
	KString udpString;
	if ( TryGetPropString(node, "UDP3DSMAX", udpString) )
	{
		for (LPTSTR line = udpString.Buffer(), next = line + strlen(line) + 1
			; *line
			; line = next, next = line + strlen(line) + 1
			)
		{
			Trim(line);
			if (line[0] == ';' || line[0] == 0) 
				continue;
			if (LPTSTR equals = _tcschr(line, TEXT('='))){
				*equals++ = 0;
				Trim(line), Trim(equals);
				if (_tcsicmp(line, name) == 0)
				{
					value = atoi(equals);
					return true;
				}
			}
		}
	}
	return false;
}