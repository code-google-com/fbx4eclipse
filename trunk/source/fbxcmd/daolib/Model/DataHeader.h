/**********************************************************************
*<
FILE: DataFormat.h

DESCRIPTION:	Data File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

//#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <iostream>
#include <stdexcept>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <sstream>
#include <strstream>
//
////#define DllExport
//#ifdef _DEBUG
//#define ASSERT(x) { if (!(x) && IsDebuggerPresent()) __asm { int 3 }; }
//WINBASEAPI BOOL WINAPI IsDebuggerPresent( VOID );
//#else
//#define ASSERT(x)
//#endif

#ifndef _countof
#  define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif

#include "Common/custom_types.h"
#include "common/vector2f.h"
#include "common/vector3f.h"
#include "common/vector4f.h"
#include "common/color4.h"
#include "common/quaternion.h"
#include "common/matrix33.h"
#include "common/matrix43.h"
#include "common/matrix44.h"
//#include "tab.h"
//#include "strclass.h"
