/**********************************************************************
*<
FILE: GFFCommon.h

DESCRIPTION:	GFF File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFFCommon.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;

#pragma region Text

template<> bool ::Recall(IDAOStream& in, Text& val)
{
	bool ok = true;
	int offset = 0;
	if (ok) ok = Recall(in, offset);
	if (ok) 
	{
		long cur = in.Tell();
		in.Seek(SEEK_SET, offset);
		// make sure we have at least 5 bytes for this to work
		char buf[8]; memset(buf, 0, sizeof(buf));
		for(;;)
		{
			ok = (1 == in.Read(buf, 4, 1)); // always 4 byte aligned with null terminato
			if (!ok) break;
			val.append(buf);
			if (strlen(buf) < 4)
				break;
		}
		in.Seek(SEEK_SET, cur);
	}
	return ok;
}

template<> bool ::Store(IDAOStream& out, Text const & val)
{
	bool ok = true;
	long off = AlignWord(out.TellEnd());

	// Strings are stored in 2 parts offset + data.
	if (ok ) ok = Store(out, off);
	if (ok)
	{
		long pos = out.Tell();
		out.Seek(SEEK_END, off);
		ok = ( 1 == out.Write(val.c_str(), AlignWord(val.size() + 1)) );
		out.Seek(SEEK_SET, pos);
	}
	return ok;
}

template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, Text const & val)
{
	out.Indent();
	out.PrintF("%-30s\t%s\n", name, val.c_str());
}

template<> unsigned int ::SizeOf(IDAOStream& in, Text const & val)
{
	return sizeof(unsigned int);
}
#pragma endregion




const EnumLookupType FieldDataTypes[] =
{
	{DT_GENERIC	,_T("Generic")},
	{DT_UINT8 		,_T("UINT8")},
	{DT_INT8 		,_T("INT8")},
	{DT_UINT16		,_T("UINT16")},
	{DT_INT16 		,_T("INT16")},
	{DT_UINT32 	,_T("UINT32")},
	{DT_INT32 		,_T("INT32")},
	{DT_UINT64 	,_T("UINT64")},
	{DT_INT64 		,_T("INT64")},
	{DT_FLOAT32 	,_T("FLOAT32")},
	{DT_FLOAT64 	,_T("FLOAT64")},
	{DT_VECTOR3F 	,_T("Vector3f")},
	{DT_VECTOR4F 	,_T("Vector4f")},
	{DT_QUATERNIONF,_T("Quaternionf")},
	{DT_ECSTRING 	,_T("ECString")},
	{DT_COLOR4F 	,_T("Color4f")},
	{DT_MATRIX4X4F ,_T("Matrix4x4f")},
	{DT_TLKSTRING 	,_T("TlkString")},
	{0,  NULL},
};

const EnumLookupType FieldFlags[] =
{
	{0					,_T("None")},
	{FF_LIST		,_T("LIST")},
	{FF_STRUCT		,_T("STRUCT")},
	{FF_REFERENCE  ,_T("REF")},
	{0,  NULL},
};


template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, FieldDataType const & val)
{
	Dump(out, name, EnumToString(val, FieldDataTypes));
}

template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, FieldFlag const & val)
{
	Dump(out, name, FlagsToString(val, FieldFlags));
}

Text EnumToText(DAO::GFF::FieldDataType const & val)
{
	return EnumToString(val, FieldDataTypes);
}

Text EnumToText(DAO::GFF::FieldFlag const & val)
{
	return FlagsToString(val, FieldFlags);
}

//////////////////////////////////////////////////////////////////////////

//