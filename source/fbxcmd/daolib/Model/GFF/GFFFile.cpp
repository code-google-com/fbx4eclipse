/**********************************************************************
*<
FILE: GFFFile.h

DESCRIPTION:	GFF File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdexcept>
#include "GFFFile.h"
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>
using namespace std;
using namespace DAO;
using namespace DAO::GFF;

//////////////////////////////////////////////////////////////////////////

//IMPLVARTYPE(DAO::GFF::GFFHeader);
//IMPLVARTYPE(DAO::GFF::GFFInfo);


template<> bool Recall<GFFInfo>(IDAOStream& in, GFFInfo& val) 
{
	LPVOID pdata = in.get_pdata(); in.set_pdata(&val);

	bool ok = Recall(val.GetInfDefmap(), in, static_cast<LPVOID>(&val)); 
	if (ok) {
		for (int i=0; i<val.Structs.size(); ++i) {
			GFFStructDef &sval = *val.Structs[i];
			Resize(in, sval.Fields, sval.FieldCount);

			int pos = in.Tell();
			if (pos != sval.FieldOffset)
				in.Seek(SEEK_SET, sval.FieldOffset);

			for (unsigned int j=0; j<sval.FieldCount  && ok; ++j) {
				ValueRef<GFFFieldDef> field( sval.Fields[j] ) ;
				ok |= Recall(in, field);
			}
		}
	}
	// unpack content
	in.set_pdata(pdata);
	return ok;
}

template<> bool Store<GFFInfo>(IDAOStream& out, GFFInfo const & val) 
{
	LPVOID pdata = out.get_pdata(); out.set_pdata( &val );

	size_t offset = 0;
	offset += SizeOf(out, val.Header);
	offset += SizeOf(out, val.Structs);
	for (int i=0; i<val.Structs.size(); ++i) {
		GFFStructDef &sval = const_cast<GFFStructDef &>(*val.Structs[i]);
		sval.FieldCount = sval.Fields.size();
		for (unsigned int j=0; j<sval.FieldCount; ++j) {
			ValueRef<GFFFieldDef> field (sval.Fields[j]);
			offset += SizeOf(out, field);
		}
	}
	// size content

	// store header and structs
	bool ok = Store(val.GetInfDefmap(), out, static_cast<LPCVOID>(&val)); 
	// handle fields
	for (int i=0; i<val.Structs.size() && ok; ++i) {
		GFFStructDef &sval = const_cast<GFFStructDef &>(*val.Structs[i]);
		for (unsigned int  j=0; j<sval.FieldCount  && ok; ++j) {
			ValueRef<GFFFieldDef> field (sval.Fields[j]);
			ok |= Store(out, field);
		}
	}
	// handle content
	out.set_pdata(pdata);
	return ok;
} 

//////////////////////////////////////////////////////////////////////////

static void DumpFieldValue(IDAODumpStream& out, LPCSTR name, const GFFFieldDef &fval);

static void DumpStructFields(IDAODumpStream& out, LPCSTR name, const GFFStructDef &sval)
{
	GFFInfo& val = *(GFFInfo*)out.get_pdata();
	//Dump(out, name, sval);
	::Dump(sval.GetInfDefmap(), sval.GetTypeName(), out, name, static_cast<LPCVOID>(&sval)); 

	out.IncreaseIndent();
	out.Indent();
	out.PrintF("%-30s\t(size=%d)\n", "Fields", sval.FieldCount);
	out.IncreaseIndent();

	for (unsigned int j=0; j<sval.FieldCount; ++j) {
		TCHAR buffer[100];
		buffer[0] = 0;
		_stprintf(buffer, _T("[%d]"), j);

		const GFFFieldDef& field = *sval.Fields[j];
		DumpFieldValue(out, buffer, field);
	}
	out.DecreaseIndent();
	out.DecreaseIndent();
}

static void DumpFieldValue(IDAODumpStream& out, LPCSTR name, const GFFFieldDef &field)
{
	GFFInfo& val = *(GFFInfo*)out.get_pdata();

	FieldFlag flags = field.Flags;
	// Dereference offset location
	if (flags & FF_REFERENCE)
		flags = FieldFlag(flags & ~FF_REFERENCE);

	Dump(out, name, field);
	out.IncreaseIndent();

	if (flags == FF_STRUCT)
	{
		unsigned short index = (field.TypeID);
		if (index >= 0 && index < val.Structs.size())
		{
			TCHAR buffer[100];
			buffer[0] = 0;
			_stprintf(buffer, _T("Root [%d]"), index);
			DumpStructFields(out, buffer, *val.Structs[index]);
		}
	}
	else if (flags == (FF_STRUCT | FF_LIST))
	{
		//const DAOArray<GFFStructDef> &sval = field.Children;
		//
		//long len = sval.length();
		//if (len > 0)
		//{
		//	TCHAR buffer[100];
		//	buffer[0] = 0;
		//	for ( int i = 0; i < len; ++i )
		//	{
		//		_stprintf(buffer, _T("[%d]"), i);
		//		DumpStructFields(out, buffer, sval[i] );
		//	}
		//}
	}
	out.DecreaseIndent();
}

template<> void Dump<GFFInfo>(IDAODumpStream& out,  LPCTSTR name, GFFInfo const & val) 
{ 
	LPVOID pdata = out.get_pdata(); out.set_pdata( const_cast<GFFInfo*>(&val) );
	Dump(val.GetInfDefmap(), val.GetTypeName(), out, name, static_cast<LPCVOID>(&val)); 
	//Dump(out, "Header", val.Header);
	//if (!val.Structs.empty())
	//{
	//	DumpStructFields(out, "Root", val.Structs[0]);
	//}
	out.set_pdata( pdata );
} 
template<> unsigned int SizeOf<GFFInfo>(IDAOStream& in, GFFInfo const & val) 
{ 
	size_t offset = 0;
	offset += SizeOf(in, val.Header);
	offset += SizeOf(in, val.Structs);
	for (int i=0; i<val.Structs.size(); ++i) {
		GFFStructDef &sval = const_cast<GFFStructDef &>(*val.Structs[i]);
		sval.FieldCount = sval.Fields.size();
		for (unsigned int j=0; j<sval.FieldCount; ++j) {
			GFFFieldDef& field = *sval.Fields[j];
			offset += SizeOf(in, field);
		}
	}
	return offset;
} 

//////////////////////////////////////////////////////////////////////////

IMPLVARTYPE(DAO::GFF::GFFHeader);
//template<> bool ::Recall<GFFHeader>(IDAOStream& in, GFFHeader& val) { 
//	return ::Recall(val.GetInfDefmap(), in, static_cast<LPVOID>(&val)); 
//} 
//template<> bool ::Store<GFFHeader>(IDAOStream& out, GFFHeader const & val) {
//	return ::Store(val.GetInfDefmap(), out, static_cast<LPCVOID>(&val)); 
//} 
//template<> void ::Dump<GFFHeader>(IDAODumpStream& out,  LPCTSTR name, GFFHeader const & val) { 
//	::Dump(val.GetInfDefmap(), val.GetTypeName(), out, name, static_cast<LPCVOID>(&val)); 
//} 
//template<> unsigned int ::SizeOf<GFFHeader>(IDAOStream& in, GFFHeader const & val) { 
//	return ::SizeOf(val.GetInfDefmap(), in, static_cast<LPCVOID>(&val)); 
//} 

//////////////////////////////////////////////////////////////////////////
GFFFile::GFFFile() 
	: rootStruct(NULL)
{

}

GFFFile::~GFFFile()
{
	GFFStructPtr(NULL).swap(rootStruct);
	fileInfo.Structs.clear();
}

void GFFFile::open(const _tstring& filename )
{
	this->filename = filename;
	DAOStream in(filename.c_str(), true);
	open(in);
}

void GFFFile::open(IDAOStream& stream )
{
	if (!Recall(stream, fileInfo)) {
		throw runtime_error(FormatStringA("Invalid File Header for file: %s", filename.c_str()));
	}

	// At this point we have the structure and field definitions loaded.  
	// Now move on to the implementation
	GFFStructPtr root ( new GFFStruct( fileInfo, *fileInfo.Structs[0] ) );
	rootStruct.swap(root);
	LPVOID pdata = stream.get_pdata();
	try
	{
		stream.set_pdata(&fileInfo);
		stream.Seek(SEEK_SET, fileInfo.Header.DataOffset);
		DAOOffsetStream sin(stream, fileInfo.Header.DataOffset);
		rootStruct->Recall(sin);
	}
	catch(...)
	{
		stream.set_pdata(pdata);
		throw;		
	}
}

void GFFFile::save( const _tstring& filename )
{
	if (filename.empty())
		throw runtime_error("Invalid file name");

	DAOStream out(filename.c_str(), false);
	save(out);
}

void GFFFile::save(IDAOStream& stream)
{
	throw runtime_error("Save not implemented");
}


void GFFFile::dump()
{
	DAODumpStream out(NULL, false);
	Dump(out, PathFindFileName(filename.c_str()), fileInfo);

	out.Print("-----------------------------------------\n");
	// At this point we have the structure and field definitions loaded.  
	// Now move on to the implementation
	out.set_pdata( &fileInfo );
	::Dump(out, "Root", rootStruct);
	out.set_pdata(NULL);
}

void GFFFile::dump( const _tstring& filename )
{
	DAODumpStream out(filename.c_str(), false);
	Dump(out, PathFindFileName(filename.c_str()), fileInfo);
}

