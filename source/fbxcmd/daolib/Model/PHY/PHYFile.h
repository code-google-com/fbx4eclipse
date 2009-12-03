/**********************************************************************
*<
FILE: PHYFile.h

DESCRIPTION:	PHY File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "PHYCommon.h"
#include "GFF/GFFFile.h"
#include "MMH/MDLH.h"

namespace DAO {
namespace PHY {

class PHYFile 
	: public DAO::GFF::GFFFile
{
	typedef DAO::GFF::GFFFile base;
public:
	PHYFile();
	virtual DAO::MMH::MDLHPtr get_Root();
};

}
}