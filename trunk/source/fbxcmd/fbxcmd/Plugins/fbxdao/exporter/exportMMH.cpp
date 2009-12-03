/**********************************************************************
*<
FILE: ExporterCore.cpp

DESCRIPTION:	Core Export helper routines

CREATED BY: tazpn (Theo)

HISTORY: 

*>	Copyright (c) 2006, All Rights Reserved.
**********************************************************************/
#include "stdafx.h"
#include "writer.h"
#include "exporterBase.h"
#include "MMHFormat.h"
#include "MSHFormat.h"
#include "../common/appsettings.h"
#include "../common/fbxconvert.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
using namespace DAO::MESH;

static const char *MMHExport = "MMHExport";
static const char *MSHExport = "MSHExport";
static const char *MSHCommon = "MSHCommon";

typedef std::map<std::string, KArrayKFbxNode, ltstr> NameNodeMap;
typedef NameNodeMap::iterator NameNodeItr;

typedef std::map<std::string, GFFFile, ltstr> NameFileMap;
typedef NameFileMap::iterator NameFileItr;

class MMHExportImpl : public ExporterBase
{
	typedef ExporterBase base;
public:
	MMHExportImpl(DAOWriter *owner, KFbxScene* scene, KFbxStreamOptions* options, DAOStreamPtr file);

	bool DoExport();

	//////////////////////////////////////////////////////////////////////////

	AppSettings exportSettings;
};


bool DAOWriter::WriteMMH( KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, DAOStreamPtr file )
{
	KFbxScene*      lScene = KFbxCast<KFbxScene>(pDocument);
	bool            lIsAScene = (lScene != NULL);
	bool            lResult = false;
	if(lIsAScene)
	{
		MMHExportImpl impl(this, lScene, pStreamOptions, file);
		lResult = impl.DoExport();
	}
	return lResult;
}


MMHExportImpl::MMHExportImpl( DAOWriter *owner, KFbxScene* scene, KFbxStreamOptions* options, DAOStreamPtr file ) 
	: base(owner, scene, options, file)
	, exportSettings("MMHExport")
{
}


bool MMHExportImpl::DoExport()
{


	return true;
}

