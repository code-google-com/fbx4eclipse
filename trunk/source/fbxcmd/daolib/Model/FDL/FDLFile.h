/**********************************************************************
*<
FILE: FDLFile.h

DESCRIPTION:	FDL File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "FDLCommon.h"
#include "GFF/GFFFile.h"
#include "FDL/LSTF.h"

namespace DAO {
namespace FDL {

class FDLFile 
	: public DAO::GFF::GFFFile
{
	typedef DAO::GFF::GFFFile base;
public:
	FDLFile();
	virtual DAO::FDL::LSTFPtr get_Root();
};

}
}