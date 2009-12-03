#pragma once

#include "config.h"

/*
* Define the major, minor, build, patch version numbers
*/
#define VERSION_MAJOR_INT  1
#define VERSION_MINOR_INT  0
#define VERSION_BUILD_INT  1

#ifndef SVN_REVISION_INT
#define VERSION_PATCH_INT  0
#else
#define  VERSION_PATCH_INT  SVN_REVISION_INT
#endif

#ifndef SVN_REVISION
#define VERSION_STRING "1, 0, 1, 0"
#else
#define VERSION_STRING "1, 0, 1, " SVN_REVISION_INT
#endif

#define COPYRIGHTYEARINT 2009
#define COPYRIGHTYEAR "2009"

#define DLLNAME "daocmd"
#define PRODUCTNAME "DAO Command Line Utilities"
#define RESFILETYPE VFT_APP
