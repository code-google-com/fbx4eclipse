#pragma once
#include <fbxsdk.h>

#pragma comment( lib, "Wininet.lib")
#if 0
#if _MSC_VER < 1400
	#ifdef _DLL
		#ifdef _DEBUG
			#pragma  comment (lib, "fbxsdk_md2003d.lib")
		#else
			#pragma  comment (lib, "fbxsdk_md2003.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma  comment (lib, "fbxsdk_mt2003d.lib")
		#else
			#pragma  comment (lib, "fbxsdk_mt2003.lib")
		#endif
	#endif
#elif _MSC_VER < 1500
	#ifdef _AMD64_
		#ifdef _DLL
			#ifdef _DEBUG
				#pragma  comment (lib, "fbxsdk_md2005_amd64d.lib")
			#else
				#pragma  comment (lib, "fbxsdk_md2005_amd64.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma  comment (lib, "fbxsdk_mt2005_amd64d.lib")
			#else
				#pragma  comment (lib, "fbxsdk_mt2005_amd64.lib")
			#endif
		#endif
	#else
		#ifdef _DLL
			#ifdef _DEBUG
				#pragma  comment (lib, "fbxsdk_md2005d.lib")
			#else
				#pragma  comment (lib, "fbxsdk_md2005.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma  comment (lib, "fbxsdk_mt2005d.lib")
			#else
				#pragma  comment (lib, "fbxsdk_mt2005.lib")
			#endif
		#endif
	#endif
#elif (_MSC_VER < 1600)
	#ifdef _AMD64_
		#ifdef _DLL
			#ifdef _DEBUG
				#pragma  comment (lib, "fbxsdk_md2008_amd64d.lib")
			#else
				#pragma  comment (lib, "fbxsdk_md2008_amd64.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma  comment (lib, "fbxsdk_mt2008_amd64d.lib")
			#else
				#pragma  comment (lib, "fbxsdk_mt2008_amd64.lib")
			#endif
		#endif
	#else
		#ifdef _DLL
			#ifdef _DEBUG
				#pragma  comment (lib, "fbxsdk_md2008d.lib")
			#else
				#pragma  comment (lib, "fbxsdk_md2008.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma  comment (lib, "fbxsdk_mt2008d.lib")
			#else
				#pragma  comment (lib, "fbxsdk_mt2008.lib")
			#endif
		#endif
	#endif
#endif

#endif