/**********************************************************************
*<
FILE: PHYFile.h

DESCRIPTION:	PHY File Format

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
#include "MMH/MMHFile.h"
#include "PHY/PHYFile.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
using namespace DAO::PHY;


PHYFile::PHYFile() {

}

DAO::MMH::MDLHPtr DAO::PHY::PHYFile::get_Root()
{
	return StructPtrCast<MDLH>( this->get_RootStruct() );
}