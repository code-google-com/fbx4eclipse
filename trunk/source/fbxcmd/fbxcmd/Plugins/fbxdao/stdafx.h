// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// _WIN32 will detect windows on most compilers
#include <stdio.h>
#include <tchar.h>
#include <string.h>

#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <exception>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <fbxsdk.h>

#include "GFFFormat.h"
#include "MSHFormat.h"
#include "MMHFormat.h"

#include "common\common.h"
