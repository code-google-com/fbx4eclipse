/**********************************************************************
*<
FILE: MMHFile.h

DESCRIPTION:	MMH File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "MMHCommon.h"
#include "GFF/GFFFile.h"
#include "MMH/MDLH.h"

namespace DAO {
namespace MMH {

class MMHFile 
	: public DAO::GFF::GFFFile
{
	typedef DAO::GFF::GFFFile base;
public:
	MMHFile();
	virtual MDLHPtr get_Root();
};

}
}