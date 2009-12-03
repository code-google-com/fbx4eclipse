#pragma once

#include <exception>

#include <fbxfilesdk/fbxfilesdk_def.h>

namespace FBXFILESDK_NAMESPACE
{
	class KFbxSdkManager;
	class KFbxMemoryAllocator;
}

extern FBXFILESDK_NAMESPACE::KFbxSdkManager* k_pSdkManager;
extern FBXFILESDK_NAMESPACE::KFbxMemoryAllocator* k_pAllocator;
