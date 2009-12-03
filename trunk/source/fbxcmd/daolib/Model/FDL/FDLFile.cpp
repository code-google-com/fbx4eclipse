/**********************************************************************
*<
FILE: FDLFile.h

DESCRIPTION:	FDL File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdexcept>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include "GFF/GFFFile.h"
#include "FDL/FDLFile.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::FDL;


FDLFile::FDLFile() {

}

DAO::FDL::LSTFPtr DAO::FDL::FDLFile::get_Root()
{
	return StructPtrCast<LSTF>( this->get_RootStruct() );
}