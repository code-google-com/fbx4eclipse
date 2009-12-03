/**********************************************************************
 *<
	FILE: DAOExport.h

	DESCRIPTION:	Includes for Plugins

	HISTORY:

 *>	Copyright (c) 2009, All Rights Reserved.
 **********************************************************************/
#pragma once

#include <windows.h>
#include <shlwapi.h>
#include <math.h>
#include <io.h>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <Max.h>
#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#ifdef USE_BIPED
#  include <cs/BipedApi.h>
#endif
//#include <scenetraversal.h> 
#include <plugapi.h>
#include <triobj.h> 
#include <bitmap.h>
#include <modstack.h>
#include <iskin.h>
#include <strclass.h>
#include "objectParams.h"
#undef ALPHA_NONE

#include <math.h>
#include <string.h>

#include "niutils.h"

#ifndef ASSERT
#ifdef _DEBUG
#include <crtdbg.h>
#define ASSERT _ASSERTE
#else
#define ASSERT(exp)
#endif
#endif

#include <direct.h>
#include <commdlg.h>

#include "MSHExport.h"

using std::runtime_error;
extern TCHAR *GetString(int id);
extern TSTR shortDescription;
extern HINSTANCE hInstance;

// Parameter and ParamBlock IDs
enum { Export_params, };  // pblock ID
enum { export_msh_params, };
enum 
{ 
   EXP_PB_SHORTDESC,
   EXP_PB_FLIPUV,
   EXP_PB_TRIM_WALKMESH,
   EXP_PB_WALKMESH_LIMIT,
   EXP_PB_COL2MATCH,
   EXP_PB_COL3MATCH,
   EXP_PB_WALKMATCH,
   EXP_PB_HOOKMATCH,
   EXP_PB_HAIRMATCH,
   EXP_PB_HELMMATCH,
   EXP_PB_SCALE_FACTOR,
   EXP_PB_TANGENT_ALGORITHM,
   EXP_PB_ENABLE_SIMPLE_EXPORT,
   EXP_PB_DEFAULT_MATERIAL_FLAG,
   EXP_PB_COLSMATCH,
   EXP_PB_EXPORTHIDDEN,
   
   EXP_PB_MAX_BLOCKS,
   EXP_PB_FN_SET_OFFSET = 100,
};
