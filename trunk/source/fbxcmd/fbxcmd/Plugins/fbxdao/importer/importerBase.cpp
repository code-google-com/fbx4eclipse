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
#if 0
	// first load
	if ( 0 == _taccess(fileName, 04) )
	{
		DAOStream stream;
		stream.Open(fileName, true);
		gffFile.open(stream);
		return;
	}
	// next search overrides
	LPCTSTR pszFilePart = PathFindFileName(fileName);
	stringlist searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>("OverrideDirs").c_str(), ",;", true);
	_tstring filePath = environmentSettings.FindFile(pszFilePart, searchPath);
	if (!filePath.empty() && ( 0 == _taccess(filePath.c_str(), 04) ) )
	{
		DAOStream stream;
		stream.Open(filePath.c_str(), true);
		gffFile.open(stream);
		return;
	}
	// next lookup by extension
	_tstring erfFilePart;
	LPCTSTR pszFileExt = PathFindExtension(pszFilePart);
	if ( erfExtensionMap.TryGetSetting(pszFileExt, erfFilePart) ) {
		_tstring erfOverride = erfOverrideDirs.GetSetting(erfFilePart, erfFilePart);
		erfOverride += _T("\\");
		erfOverride += pszFilePart;
		searchPath = TokenizeString(environmentSettings.GetSetting<_tstring>(_T("ERFDirs")).c_str(), _T(",;"), true);
		filePath = environmentSettings.FindFile(erfOverride, searchPath);
		if (!filePath.empty() && ( 0 == _taccess(filePath.c_str(), 04) ) )
		{
			DAOStream stream;
			stream.Open(filePath.c_str(), true);
			gffFile.open(stream);
			return;
		}

		WText filePart(pszFilePart);
		// finally locate the ERF file and extract file from that
		_tstring erfFileName = erfFilePart + _T(".erf");
		filePath = environmentSettings.FindFile(erfFileName, searchPath);
		if (!filePath.empty() && ( 0 == _taccess(filePath.c_str(), 04) ) )
		{
			DAOStream stream;
			stream.Open(filePath.c_str(), true);

			ERFHeader hdr;
			if (stream.Read(&hdr, sizeof(hdr), 1) != 1) {
				throw std::runtime_error( "File is not a recognized ERF format.");
			}
			if ( wcsncmp(hdr.hdr, L"ERF ", 4) != 0 || wcsncmp(hdr.type, L"V2.0", 4) != 0){
				throw std::runtime_error( _T("File is not a recognized ERF format.\n") );
			}
			for (int i = 0; i < hdr.nfiles; ++i) {
				FileReference fref;
				if (stream.Read(&fref, sizeof(FileReference), 1) != 1) {
					throw std::runtime_error( _T("File list could not be read.\n") );
				}
				if (filePart.Compare(fref.filename, true) == 0)
				{
					DAOOffsetStream offsetStream(stream, fref.offset, fref.length);
					gffFile.open(offsetStream);
					return;
				}
			}
		}
	}
#endif
}
