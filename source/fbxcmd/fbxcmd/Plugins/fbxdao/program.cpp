// ***** BEGIN LICENSE BLOCK *****
//
// Copyright (c) 2006-2008
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above
//      copyright notice, this list of conditions and the following
//      disclaimer in the documentation and/or other materials provided
//      with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// ***** END LICENSE BLOCK *****
#include "stdafx.h"
#include "fbxdao.h"
#include "Windows.h"
#include <fbxsdk.h>
#include <fbxfilesdk/fbxfilesdk_nsuse.h>
#include "../../core/fbx_import.h"

KFbxSdkManager* k_pSdkManager = NULL;
KFbxMemoryAllocator* k_pAllocator = NULL;
HMODULE k_hModuleHandle = NULL;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if ( ul_reason_for_call == DLL_PROCESS_ATTACH)
		k_hModuleHandle = hModule;
    return TRUE;
}

extern KFbxWriter* CreateWriter(KFbxSdkManager& pManager, KFbxExporter& pExporter, int pSubID, int pPluginID);
extern void* GetWriterInfo(KFbxWriter::KInfoRequest pRequest, int pId);
extern void FillWriterIOSettings(KFbxIOSettings& pIOS);
extern KFbxReader* CreateReader(KFbxSdkManager& pManager, KFbxImporter& pImporter, int pSubID, int pPluginID);
extern void *GetReaderInfo(KFbxReader::KInfoRequest pRequest, int pId);
extern void FillReaderIOSettings(KFbxIOSettings& pIOS);

struct ReaderFunctions
{
	KFbxReader::CreateFuncType createFuncType;
	KFbxReader::IOSettingsFillerFuncType ioSettingsFillerFuncType;
	KFbxReader::GetInfoFuncType getInfoFuncType;
};

struct WriterFunctions
{
	KFbxWriter::CreateFuncType createFuncType;
	KFbxWriter::IOSettingsFillerFuncType ioSettingsFillerFuncType;
	KFbxWriter::GetInfoFuncType getInfoFuncType;
};

extern "C" int __stdcall 
RegisterFBXPlugins(KFbxSdkManager* pSdkManager, KFbxMemoryAllocator* pAllocator
				   , int *nReaders, ReaderFunctions** readers
				   , int *nWriters, WriterFunctions** writers
				   )
{
	k_pSdkManager = pSdkManager;
	k_pAllocator = pAllocator;
	KFbxSdkManager::SetMemoryAllocator(pAllocator);

	int retval = 1;
	*nReaders = 1;
	*readers = (ReaderFunctions*)pAllocator->mCallocHandler(sizeof(ReaderFunctions), 1);
	(*readers)[0].createFuncType = CreateReader;
	(*readers)[0].ioSettingsFillerFuncType = FillReaderIOSettings;
	(*readers)[0].getInfoFuncType = GetReaderInfo;

	*nWriters = 1;
	*writers = (WriterFunctions*)pAllocator->mCallocHandler(sizeof(WriterFunctions), 1);
	(*writers)[0].createFuncType = CreateWriter;
	(*writers)[0].ioSettingsFillerFuncType = FillWriterIOSettings;
	(*writers)[0].getInfoFuncType = GetWriterInfo;

	return retval;
}

extern "C" int __stdcall 
UnregisterFBXPlugins(KFbxSdkManager* pSdkManager)
{
	k_pSdkManager = NULL;
	k_pAllocator = NULL;
	KFbxSdkManager::SetMemoryAllocator(NULL);
	return 0;
}
