/**********************************************************************
*<
FILE: MESHFile.h

DESCRIPTION:	MESH File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "MESHCommon.h"
#include "GFF/GFFFile.h"
#include "MESH/MESH.h"
namespace DAO {
namespace MESH {

class MESHFile 
	: public DAO::GFF::GFFFile
{
	typedef DAO::GFF::GFFFile base;
public:
	MESHFile();
	virtual MESHPtr get_Root();

};

}
}