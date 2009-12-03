/**********************************************************************
*<
FILE: BaseImporter.h

DESCRIPTION:	Base Importer class

CREATED BY: tazpn (Theo)

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "config.h"

/*
* Define the major, minor, build, patch version numbers
*/
#define VERSION_MAJOR_INT  1
#define VERSION_MINOR_INT  3
#define VERSION_BUILD_INT  0

#ifndef DAOTOOLS_REVISION_INT
#define VERSION_PATCH_INT  0
#else
#define  VERSION_PATCH_INT  DAOTOOLS_REVISION_INT
#endif

#ifndef DAOTOOLS_REVISION
#define VERSION_STRING "1, 3, 0, 0"
#else
#define VERSION_STRING "1, 3, 0, " DAOTOOLS_REVISION
#endif

#define COPYRIGHTYEARINT 2009
#define COPYRIGHTYEAR "2009"

#define DLLNAME "fbxcmd"
#define PRODUCTNAME "FBX Command Line Utilities"
#define RESFILETYPE VFT_APP

