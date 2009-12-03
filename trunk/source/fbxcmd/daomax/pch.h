#ifdef __cplusplus
#pragma warning( disable:4800 )

#include <stdio.h>
#include <tchar.h>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>
#include <list>
#include <deque>
#include <sstream>
#include <string>
#include <set>

using std::runtime_error;

// _WIN32 will detect windows on most compilers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <math.h>
#include <io.h>
#include <string.h>
#include <direct.h>
#include <commdlg.h>

#ifndef ASSERT
#ifdef _DEBUG
#include <crtdbg.h>
#define ASSERT _ASSERTE
#else
#define ASSERT(exp)
#endif
#endif

#include <Max.h>
#include "MAX_MemDirect.h"
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
#include <iskin.h>
#include <strclass.h>
#include <Maxscrpt/maxscrpt.h>
#include <maxscrpt/strings.h>
#include "objectParams.h"
#undef ALPHA_NONE

#include <fbxsdk.h>
#include "../fbxcmd/plugins/fbxcmn/KFbxLog.h"

#include "niutils.h"

#include "ResourceManager.h"
#endif