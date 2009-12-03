/**********************************************************************
*<
FILE: GFFFile.h

DESCRIPTION:	GR2 File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once 

#include "../DataHeader.h"
#include "../DAOFormat.h"
#include "text.h"

#include <pshpack1.h>

namespace DAO {
namespace GFF {

#include "BinaryGFFIDList.h"

enum FieldDataType : unsigned short
{
	DT_GENERIC		= 0xFFFF,
	DT_UINT8 		= 0,
	DT_INT8 		= 1,
	DT_UINT16 		= 2,
	DT_INT16 		= 3,
	DT_UINT32 		= 4,
	DT_INT32 		= 5,
	DT_UINT64 		= 6,
	DT_INT64 		= 7,
	DT_FLOAT32 		= 8,
	DT_FLOAT64 		= 9,
	DT_VECTOR3F 	=10,
	DT_VECTOR4F 	=12,
	DT_QUATERNIONF 	=13,
	DT_ECSTRING 	=14,
	DT_COLOR4F 		=15,
	DT_MATRIX4X4F 	=16,
	DT_TLKSTRING 	=17,
};

enum FieldFlag : unsigned short
{
	FF_LIST = 0x8000,
	FF_STRUCT = 0x4000,
	FF_REFERENCE  = 0x2000,
};

//////////////////////////////////////////////////////////////////
typedef FixedString<CHAR, 4, ' '> ShortString;

} // namespace GFF
} // namespace DAO

template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, DAO::GFF::FieldDataType const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, DAO::GFF::FieldFlag const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, DAO::GFF::GFFID const & val);

Text EnumToText(DAO::GFF::GFFID const & val);
Text EnumToText(DAO::GFF::FieldDataType const & val);
Text EnumToText(DAO::GFF::FieldFlag const & val);

template<typename T>
bool Recall(IDAOStream& in, ValuePtr<T>& val) 
{
	bool ok = true;
	if (!val.isNull())
		ok = Recall(in, *val);
	return ok;
}

template<typename T>
bool Store(IDAOStream& out, ValuePtr<T> const & val) 
{
	bool ok = true;
	if (!val.isNull())
		ok = Store(out, *val);
	return ok;
}

template<typename T>
bool Store(IDAOStream& out, ValuePtr<T> & val) 
{
	bool ok = true;
	if (!val.isNull())
		ok = Store(out, *val);
	return ok;
}

template<typename T> 
void Dump(IDAODumpStream& out, LPCTSTR name, ValuePtr<T> const & val) 
{
	if (val.isNull()) {
		out.Indent(), out.PrintF("%s: <NULL>\n", name);
	} else {
		//out.PrintF("%s:\n", name);
		//out.IncreaseIndent();
		Dump(out, name, *val);
		//out.DecreaseIndent();
	}
}

template<typename T>
unsigned int SizeOf(IDAOStream& in, ValuePtr<T> const & val) {
	return (val.isNull()) ? 0 : sizeof(T);
}


template<typename T>
bool Recall(IDAOStream& in, ValueRef<T>& val) 
{
	bool ok = true;
	if (!val.isNull())
		ok = Recall(in, *val);
	return ok;
}

template<typename T>
bool Store(IDAOStream& out, ValueRef<T> & val) 
{
	bool ok = true;
	if (!val.isNull())
		ok = Store(out, *val);
	return ok;
}

template<typename T> 
void Dump(IDAODumpStream& out, LPCTSTR name, ValueRef<T> const & val) 
{
	//out.Indent();
	if (val.isNull()) {
		out.PrintF("%s: <NULL>\n", name);
	} else {
		//out.PrintF("%s *:\n", name);
		//out.IncreaseIndent();
		Dump(out, name, *val);
		//out.DecreaseIndent();
	}
}

template<typename T>
unsigned int SizeOf(IDAOStream& in, ValueRef<T> const & val) {
	return (val.isNull()) ? 0 : sizeof(T);
}


#pragma endregion
ROOTVARTYPE(Text);

#include <poppack.h>
