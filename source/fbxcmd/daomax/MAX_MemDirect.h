/**********************************************************************
 *<
	FILE: MAX_Mem

	DESCRIPTION:  Access to the memory routines used by Max
				  (Can be called from plug-ins compiled with different
				   memory handling)

	CREATED BY: Cleve Ard & Michaelson Britt

	HISTORY: Created, Oct. 2003

 *>	Copyright (c) 2003 Autodesk, Inc., All Rights Reserved.
 **********************************************************************/
#pragma once

#if VERSION_3DSMAX < (9000<<16) // Version 9

#include <crtdbg.h>
#include <malloc.h>
#include <new.h>



// NOTE: For efficiency, most functions are declared as pointers which
// route into the corresponding function.  This avoids passing through
// a "wrapper" layer.
//
// New and Delete were implemented with wrappers, so that the original
// MAX_Mem.h could use function overloads with them, since this is not
// possible with the preprocessor.
//   int* x = new;
// There's no way to define 'new' as a preprocessor macro such that
// 'new int' is translated to 'MAX_new( sizeof(int) )'


CoreExport void *__cdecl MAX_new(size_t size);

CoreExport void *__cdecl MAX_new_array(size_t size);

// Disabled: Placement forms not supported
//CoreExport void *__cdecl MAX_new_placement(size_t size, void *_P);

CoreExport void __cdecl MAX_delete(void* mem);

CoreExport void __cdecl MAX_delete_array(void* mem);

// Disabled: Placement forms not supported
//CoreExport void __cdecl MAX_delete_placement(void *memblock, void *_P);

//Allocate block of memory from heap 
CoreExport void *	(__cdecl *MAX_malloc)(size_t size);

//Allocate storage for array, initializing every byte in allocated block to 0 
CoreExport void *	(__cdecl *MAX_calloc)(size_t num, size_t size);

//Reallocate block to new size 
CoreExport void *	(__cdecl *MAX_realloc)(void *memblock, size_t size);

//Expand or shrink block of memory without moving it 
CoreExport void *	(__cdecl *MAX_expand)(void * memblock, size_t size);

//Free allocated block 
CoreExport void	(__cdecl *MAX_free)(void * memblock);

//Return size of allocated block 
CoreExport size_t	(__cdecl *MAX_msize)(void *memblock);

// Set hook function
// Disabled: because HEAPHOOK does not seem to be enabled in practice
//CoreExport _HEAPHOOK (__cdecl *MAX_setheaphook)(_HEAPHOOK);

//Add memory to heap 
CoreExport int	(__cdecl *MAX_heapadd)(void * memblock, size_t size);

//Check heap for consistency 
CoreExport int	(__cdecl *MAX_heapchk)(void);

//Release unused memory in heap 
CoreExport int	(__cdecl *MAX_heapmin)(void);

//Fill free heap entries with specified value 
CoreExport int	(__cdecl *MAX_heapset)(unsigned int fill);

//Return information about each entry in heap 
CoreExport int	(__cdecl *MAX_heapwalk)(_HEAPINFO *entryinfo);

//Return address of current new handler routine as set by _set_new_handler 
CoreExport _PNH	(__cdecl *MAX_query_new_handler)( void );

//Enable error-handling mechanism when new operator fails (to allocate memory) and enable compilation of Standard Template Libraries (STL) 
CoreExport _PNH	(__cdecl *MAX_set_new_handler)( _PNH pNewHandler );

//Return integer indicating new handler mode set by _set_new_mode for malloc 
CoreExport int	(__cdecl *MAX_query_new_mode)( void );

//Set new handler mode for malloc 
CoreExport int	(__cdecl *MAX_set_new_mode)( int newhandlermode );

//Get/Set the upper limit for the size of a memory allocation that will be supported by the small-block heap 
CoreExport size_t	(__cdecl *MAX_get_sbh_threshold)(void);
CoreExport int	(__cdecl *MAX_set_sbh_threshold)(size_t size);


//-----------------------------------------------------------------------------
// The following debug functions are available only in a Sparks debug build

#ifdef _DEBUG
#ifndef IS_HYBRID

//Debug version of calloc; only available in the debug versions of the run-time libraries 
CoreExport void *	(__cdecl *MAX_calloc_dbg)(size_t num, size_t size, int blockType, const char * filename, int lineNumber);

//Debug version of _expand; only available in the debug versions of the run-time libraries 
CoreExport void *	(__cdecl *MAX_expand_dbg)(void * userData, size_t newSize, int blockType, const char * filename, int lineNumber);

//Debug version of malloc; only available in the debug versions of the run-time libraries 
CoreExport void *	(__cdecl *MAX_malloc_dbg)(size_t size,int blockType,const char *filename,int lineNumber);

//Debug version of free; only available in the debug versions of the run-time libraries 
CoreExport void	(__cdecl *MAX_free_dbg)(void * userData, int blockType);

//Debug version of _msize; only available in the debug versions of the run-time libraries 
CoreExport size_t	(__cdecl *MAX_msize_dbg)(void *userData, int blockType);

//Debug version of realloc; only available in the debug versions of the run-time libraries 
CoreExport void *	(__cdecl *MAX_realloc_dbg)(void *userData, size_t newSize, int blockType, const char *filename, int lineNumber);

CoreExport extern long& MAX_crtAssertBusy;

//Install a client-defined reporting function by hooking it into the C run-time debug reporting process
CoreExport _CRT_REPORT_HOOK	(__cdecl *MAX_CrtSetReportHook)(_CRT_REPORT_HOOK reportHook);

//Install a client-defined reporting function by hooking it into the C run-time debug reporting process
CoreExport int	(__cdecl *MAX_CrtSetReportMode)(int reportType,int reportMode);

//Identify the file or stream to be used as a destination for a specific report type by _CrtDbgReport
CoreExport _HFILE	(__cdecl *MAX_CrtSetReportFile)(int reportType,_HFILE reportFile);

//Generate a debug report with a user message and send the report to three possible destinations
CoreExport int	(__cdecl *MAX_CrtDbgReport)(int,const char *,int,const char *,const char *,...);

CoreExport void	__cdecl MAX_CrtDbgBreak(void);

CoreExport extern long& MAX_crtBreakAlloc;      /* Break on this allocation */

//Set a breakpoint on a specified object allocation order number
CoreExport long	(__cdecl *MAX_CrtSetBreakAlloc)(long lBreakAlloc);

//Install a client-defined allocation function by hooking it into the C run-time debug memory allocation process
CoreExport _CRT_ALLOC_HOOK	(__cdecl *MAX_CrtSetAllocHook)(_CRT_ALLOC_HOOK allocHook);

CoreExport extern int& MAX_crtDbgFlag;

//Confirm the integrity of the memory blocks allocated on the debug heap
CoreExport int	(__cdecl *MAX_CrtCheckMemory)(void);

//Retrieve or modify the state of the _crtDbgFlag flag to control the allocation behavior of the debug heap manager
CoreExport int	(__cdecl *MAX_CrtSetDbgFlag)(int newFlag);

//Call an application-supplied function for all _CLIENT_BLOCK types on the heap
CoreExport void	(__cdecl *MAX_CrtDoForAllClientObjects)(void (*pfn)(void *, void *),void *context);

//Verify that a specified memory range is valid for reading and writing
CoreExport int	(__cdecl *MAX_CrtIsValidPointer)(const void *address,unsigned int size,int access);

//Verify that a specified pointer is in the local heap
CoreExport int	(__cdecl *MAX_CrtIsValidHeapPointer)(const void *userData);

//Verify that a specified memory block is located within the local heap and that it has a valid debug heap block type identifier
CoreExport int	(__cdecl *MAX_CrtIsMemoryBlock)(const void *userData,unsigned int size,long *requestNumber,char **filename,int *linenumber);

//Install an application-defined function that is called every time a debug dump function is called to dump _CLIENT_BLOCK type memory blocks
CoreExport _CRT_DUMP_CLIENT	(__cdecl *MAX_CrtSetDumpClient)(_CRT_DUMP_CLIENT dumpClient);

//Obtain the current state of the debug heap and store it in an application-supplied _CrtMemState structure
CoreExport void	(__cdecl *MAX_CrtMemCheckpoint)(_CrtMemState *state);

//Compare two memory states for significant differences and return the results
CoreExport int	(__cdecl *MAX_CrtMemDifference)(_CrtMemState *stateDiff,const _CrtMemState *oldState,const _CrtMemState *newState);

//Dump information about objects on the heap since a specified checkpoint was taken or from the start of program execution
CoreExport void	(__cdecl *MAX_CrtMemDumpAllObjectsSince)(const _CrtMemState *state);

//Dump the debug header information for a specified memory state in a user-readable form
CoreExport void	(__cdecl *MAX_CrtMemDumpStatistics)(const _CrtMemState *state);

//Dump all of the memory blocks on the debug heap when a significant memory leak has occurred
CoreExport int	(__cdecl *MAX_CrtDumpMemoryLeaks)(void);

#endif //IS_HYBRID
#endif //_DEBUG

#endif // VERSION_3DSMAX < (9000<<16) // Version 9
