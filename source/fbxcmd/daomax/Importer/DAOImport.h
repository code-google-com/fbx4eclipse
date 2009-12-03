/**********************************************************************
 *<
	FILE: DAOImport.h

	DESCRIPTION:	Includes for Plugins

	HISTORY:

 *>	Copyright (c) 2009, All Rights Reserved.
 **********************************************************************/
#pragma once

#include <math.h>
#include <io.h>
#include <sstream>
#include <exception>

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
#include <meshdelta.h>
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

#include "MSHImport.h"

extern TCHAR *GetString(int id);
extern TSTR shortDescription;
extern HINSTANCE hInstance;

enum { import_params, };  // pblock ID
enum { imp_msh_params, };
enum 
{ 
   IMP_PB_SHORTDESC,
   IMP_PB_FLIPUV,
   IMP_PB_AUTOSMOOTH,
   IMP_PB_AUTOSMOOTH_ANGLE,
   IMP_PB_RENDERTEX,
   IMP_PB_TRIM_WALKMESH,
   IMP_PB_WALKMESH_LIMIT,
   IMP_PB_ENABLE_COLLISION,
   IMP_PB_ENABLE_WALKMESH,
   IMP_PB_ENABLE_HOOKS,
   IMP_PB_ENABLE_HAIRHELM,
   IMP_PB_ENABLE_BONES,
   IMP_PB_BONEMATCH,
   IMP_PB_WALK_OPACITY,
   IMP_PB_FACEBONEMATCH,
   IMP_PB_ENABLE_SCALE,
   IMP_PB_SCALE_FACTOR,
   IMP_PB_ENABLE_ANIMATION,
   IMP_PB_NORMALBUMP,
   IMP_PB_ENABLE_WELD,
   IMP_PB_WELD_LIMIT,

   IMP_PB_MAX_BLOCKS,
   IMP_PB_FN_SET_OFFSET = 100,
};
