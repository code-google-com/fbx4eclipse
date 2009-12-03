/**********************************************************************
 *<
	FILE: DllEntry.cpp

	DESCRIPTION: Contains the Dll Entry stuff

	HISTORY: 

 *>	Copyright (c) 2009, All Rights Reserved.
 **********************************************************************/
#include <windows.h>
#include <shlwapi.h>
#include <max.h>
#include <iparamb2.h>
#include "niutils.h"

extern ClassDesc2* GetDAOImportDesc();
extern ClassDesc2* GetDAOExportDesc();
extern ClassDesc2* GetMSHUtilityDesc();

static void InitializeLibSettings();
extern "C" void InitializeFbxSettings();
extern "C" void FreeFbxSettings();
extern "C" bool IsFbxInitialized();

HINSTANCE hInstance;
static int controlsInit = FALSE;
static int libVersion = VERSION_3DSMAX;
static int nClasses = 0;
static ClassDesc2* classDescriptions[20];

// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.

	if (!controlsInit) {
		controlsInit = TRUE;
		InitCustomControls(hInstance);	// Initialize MAX's custom controls
		InitCommonControls();			// Initialize Win95 controls
	}
	if (fdwReason == DLL_PROCESS_ATTACH)
		InitializeLibSettings();
	if (fdwReason == DLL_PROCESS_DETACH)
		FreeFbxSettings();

	return (TRUE);
}

void InitializeLibSettings()
{
	__try { InitializeFbxSettings(); }
	__except (EXCEPTION_EXECUTE_HANDLER) { }

   Interface *gi = GetCOREInterface();
   TCHAR iniName[MAX_PATH];
   GetModuleFileName(NULL, iniName, _countof(iniName));
   if (LPTSTR fname = PathFindFileName(iniName))
      *fname = NULL;
   PathAddBackslash(iniName);
   PathAppend(iniName, "plugcfg");
   PathAppend(iniName, "DAOUtils.ini");
#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+
   if (gi && (_access(iniName, 0) == -1)) {
      LPCTSTR pluginDir = gi->GetDir(APP_PLUGCFG_DIR);
      PathCombine(iniName, pluginDir, "DAOUtils.ini");
   } 
#endif
   libVersion = GetIniValue("System", "MaxSDKVersion", libVersion, iniName);
   if (libVersion == 0)
      libVersion = VERSION_3DSMAX;

   nClasses = 0;

   if (IsFbxInitialized())
   {
	   if ( GetIniValue<bool>("MSHImport", "Enable", true, iniName) ) {
		   classDescriptions[nClasses++] = GetDAOImportDesc();
	   }
	   if ( GetIniValue<bool>("MSHExport", "Enable", true, iniName) ) {
		   classDescriptions[nClasses++] = GetDAOExportDesc();
	   }
   }
   //if ( GetIniValue<bool>("MSHUtility", "EnableMSHUtility", true, iniName) ) {
   //   classDescriptions[nClasses++] = GetMSHUtilityDesc();
   //}  
}

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const TCHAR* LibDescription()
{
   return TEXT("tazpn's DA:O Max Utilities");
}

static LPTSTR PathMerge(LPTSTR base, LPCTSTR file)
{
   PathRemoveFileSpec(base);
   PathAppend(base, file);
   return base;
}

// This function returns the number of plug-in classes this DLL
//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
	return nClasses;
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
   return classDescriptions[i];
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];
	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
   return ULONG(libVersion);
}
