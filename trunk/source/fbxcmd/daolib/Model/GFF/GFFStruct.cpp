/**********************************************************************
*<
FILE: GFFStruct.h

DESCRIPTION:	GFF File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFFStruct.h"
#include "GFFFile.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;

//IMPLVARTYPE(DAO::GFF::GFFStructDef);

template<> bool ::Recall<GFFStructDef>(IDAOStream& in, GFFStructDef& val) { 
	return ::Recall(val.GetInfDefmap(), in, static_cast<LPVOID>(&val)); 
} 
template<> bool ::Store<GFFStructDef>(IDAOStream& out, GFFStructDef const & val) {
	return ::Store(val.GetInfDefmap(), out, static_cast<LPCVOID>(&val)); 
} 
template<> void ::Dump<GFFStructDef>(IDAODumpStream& out,  LPCTSTR name, GFFStructDef const & val) { 
	::Dump(val.GetInfDefmap(), val.GetTypeName(), out, name, static_cast<LPCVOID>(&val)); 
	out.IncreaseIndent();
	::Dump(out, _T("Fields"), val.Fields);
	out.DecreaseIndent();
} 
template<> unsigned int ::SizeOf<GFFStructDef>(IDAOStream& in, GFFStructDef const & val) { 
	return ::SizeOf(val.GetInfDefmap(), in, static_cast<LPCVOID>(&val)); 
} 

//////////////////////////////////////////////////////////////////////////

GFFStruct::GFFStruct( const GFFInfo& owner, const GFFStructDef& defininition )
	: _owner(owner), define( &const_cast<GFFStructDef&>(defininition) )
{
	//GFFStructDefRef ptr(&defininition);
	//this->define.swap(ptr);

	// Allocate memory and define field pointers
	for (unsigned long i=0; i < define->FieldCount; ++i)
	{
		GFFFieldDefRef fref( define->Fields[i] );
		GFFFieldPtr fptr(GFFField::Construct(GFFStructRef(this), fref));
		GFFFieldMap::iterator itr = fields.insert(fields.end(), GFFFieldMap::value_type(fref->Label, GFFFieldPtr()));
		(*itr).second.swap(fptr);
	}
}

GFFStruct::~GFFStruct()
{
	fields.clear();
}

bool GFFStruct::Recall(IDAOStream& in)
{
	bool ok = true;
	int offset = in.Tell();
	for ( GFFFieldMap::iterator itr = fields.begin(), end = fields.end(); itr != end && ok; ++itr)
	{
		GFFFieldRef field( (*itr).second );
		GFFFieldDefRef fieldDef = field->getDefinition();
		in.Seek(SEEK_SET, offset + fieldDef->Index);
		ok = field->Recall(in);
	}
	return ok;
}

bool GFFStruct::Store(IDAOStream& out) const
{
	bool ok = true;
	return ok;
}

void GFFStruct::Dump(IDAODumpStream& out, LPCTSTR name) const
{
	uint len = 0;
	TCHAR buffer[100];
	buffer[0] = 0;

	out.Indent();
	out.PrintF(_T("%-30s\t%s\n"), "Struct", name);
	out.IncreaseIndent();
	for ( GFFFieldMap::const_iterator itr = fields.begin(), end = fields.end(); itr != end; ++itr)
	{
		GFFID id = (*itr).first;
		GFFFieldRef field( (*itr).second );
		GFFFieldDefRef fieldDef = field->getDefinition();
		Text key = EnumToText(id);
		::Dump(out, key.data(), field);
	}
	out.DecreaseIndent();
}

template<> bool ::Recall<DAO::GFF::GFFStruct>(IDAOStream& in, DAO::GFF::GFFStruct& val) { 
	return val.Recall(in);
} 
template<> bool ::Store<DAO::GFF::GFFStruct>(IDAOStream& out, DAO::GFF::GFFStruct const & val) {
	return val.Store(out);
} 
template<> void ::Dump<DAO::GFF::GFFStruct>(IDAODumpStream& out,  LPCTSTR name, DAO::GFF::GFFStruct const & val) { 
	val.Dump(out, name);
} 
template<> unsigned int ::SizeOf<DAO::GFF::GFFStruct>(IDAOStream& in, DAO::GFF::GFFStruct const & val) { 
	return val.getDefinition()->StructSize;
} 
