/**********************************************************************
*<
FILE: GFFFile.h

DESCRIPTION:	GFF File Format

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
#include "MESH/MESHFile.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MESH;


MESHFile::MESHFile() {

}

DAO::MESH::MESHPtr DAO::MESH::MESHFile::get_Root()
{
	return StructPtrCast<MESH>( this->get_RootStruct() );
}