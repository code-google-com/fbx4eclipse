/**********************************************************************
 *<
	FILE: NWN2Utility.h

	DESCRIPTION:	Includes for Plugins

	HISTORY:

 *>	Copyright (c) 2009, All Rights Reserved.
 **********************************************************************/

#ifndef __NWN2Utility__H
#define __NWN2Utility__H

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
#include <plugapi.h>
#include <triobj.h> 
#include <bitmap.h>
#include <modstack.h>
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

extern TCHAR *GetString(int id);
extern TSTR shortDescription;
extern HINSTANCE hInstance;
extern NameValueCollection Environment;

enum { walkmesh_params, };  // pblock ID
enum { wlk_mdb_params, };
enum 
{ 
   WLK_PB_PAINTTYPE,
   WLK_PB_PAINTMODE,
   WLK_PB_MAX_BLOCKS,
   WLK_PB_FN_SET_OFFSET = 100,
};

#endif
