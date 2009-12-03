#include "stdafx.h"
#include "program.h"
#include <sys/stat.h>
#include <io.h>
#include <shellapi.h>
#include "GFF/GFFFile.h"
#include "GFF/GFFStruct.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;

LPCSTR hdrFormat =
"\n\
/**********************************************************************\n\
*<\n\
FILE: %s.h\n\
\n\
DESCRIPTION:	%s File Format\n\
\n\
HISTORY:\n\
\n\
*>	Copyright (c) 2009, All Rights Reserved.\n\
**********************************************************************/\n\
\n\
#pragma once\n\
\n\
#include \"%s/%sCommon.h\"\n\
#include \"GFF/GFFField.h\"\n\
#include \"GFF/GFFList.h\"\n\
#include \"GFF/GFFStruct.h\"\n\
\n\
namespace DAO {\n\
\n\
using namespace GFF;\n\
namespace %s {\n\
";

LPCSTR ftrFormat ="\n\
} //namespace %s\n\
} //namespace DAO\n\
";

LPCSTR implHdrFormat = "\n\
/**********************************************************************\n\
*<\n\
FILE: %s.h\n\
\n\
DESCRIPTION:	%s File Format\n\
\n\
HISTORY:\n\
*>	Copyright (c) 2009, All Rights Reserved.\n\
**********************************************************************/\n\
#include <stdafx.h>\n\
#include \"GFF/GFFCommon.h\"\n\
#include \"GFF/GFFField.h\"\n\
#include \"GFF/GFFList.h\"\n\
#include \"GFF/GFFStruct.h\"\n\
#include \"%s/%sCommon.h\"\n\
#include \"%s/%s.h\"\n\
using namespace std;\n\
using namespace DAO;\n\
using namespace DAO::GFF;\n\
using namespace DAO::%s;\n\
";

//////////////////////////////////////////////////////////////////////////
static void ConvertClassName(Text& text)
{
	text.toUpper();
}

static void ConvertMemberName(Text& text)
{
	text.toMixed();
	text.removeChar('_');
	text[0] = tolower(text[0]);
}
static void ConvertFieldName(Text& text)
{
	text.toMixed();
	text.removeChar('_');
	text[0] = tolower(text[0]);
}

const EnumLookupType FieldDataTypesToC[] =
{
	{DT_GENERIC	    ,_T("GFFField")},
	{DT_UINT8 		,_T("unsigned char")},
	{DT_INT8 		,_T("char")},
	{DT_UINT16		,_T("unsigned short")},
	{DT_INT16 		,_T("short")},
	{DT_UINT32 	    ,_T("unsigned long")},
	{DT_INT32 		,_T("long")},
	{DT_UINT64 	    ,_T("unsigned __int64")},
	{DT_INT64 		,_T("__int64")},
	{DT_FLOAT32 	,_T("float")},
	{DT_FLOAT64 	,_T("double")},
	{DT_VECTOR3F 	,_T("Vector3f")},
	{DT_VECTOR4F 	,_T("Vector4f")},
	{DT_QUATERNIONF ,_T("Quaternion")},
	{DT_ECSTRING 	,_T("Text")},
	{DT_COLOR4F 	,_T("Color")},
	{DT_MATRIX4X4F  ,_T("Matrix4")},
	{DT_TLKSTRING 	,_T("Text")},
	{0,  NULL},
};
static Text GetFieldDataType(FieldDataType type)
{
	return EnumToString(type, FieldDataTypesToC);
}

const EnumLookupType FieldDataTypesToFieldName[] =
{
	{DT_GENERIC		,_T("asField")},
	{DT_UINT8 		,_T("asUInt8")},
	{DT_INT8 		,_T("asInt8")},
	{DT_UINT16		,_T("asUInt16")},
	{DT_INT16 		,_T("asInt16")},
	{DT_UINT32 		,_T("asUInt32")},
	{DT_INT32 		,_T("asInt32")},
	{DT_UINT64 		,_T("asUInt64")},
	{DT_INT64 		,_T("asInt64")},
	{DT_FLOAT32 	,_T("asFloat32")},
	{DT_FLOAT64 	,_T("asFloat64")},
	{DT_VECTOR3F 	,_T("asVector3f")},
	{DT_VECTOR4F 	,_T("asVector4f")},
	{DT_QUATERNIONF,_T("asQuat")},
	{DT_ECSTRING 	,_T("asECString")},
	{DT_COLOR4F 	,_T("asColor4")},
	{DT_MATRIX4X4F ,_T("asMatrix4")},
	{DT_TLKSTRING 	,_T("asTlkString")},
	{0,  NULL},
};
static Text GetFieldDataTypeToFieldName(FieldDataType type)
{
	return EnumToString(type, FieldDataTypesToFieldName);
}

static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "DumpGFFHeader - Dumps a C++ Header file for wrapping GFF files"; break;
	case Cmd::htLong:  
		{
			TCHAR fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_tsplitpath(fullName, NULL, NULL, exeName, NULL);
			cout << "Usage: " << exeName << " DumpGFFHeader [-opts[modifiers]] <gff file>" << endl 
				<< "  Dumps a C++ Header file for wrapping GFF files." << endl
				<< endl
				<< "<Switches>" << endl
				<< "  v         Verbose Help (Lists specific versions)" << endl
				<< endl
				;
		}
		break;
	}
}

static bool ExecuteCmd(Program &prog)
{
	TCHAR infile[MAX_PATH], fullpath[MAX_PATH];
	int argc = prog.argc;
	TCHAR **argv = prog.argv;
	bool verboseHelp = false;
	TCHAR path[MAX_PATH];
	bool headers = false;
	bool bodies = false;
	bool files = true;

	if (argc == 0)
		return false;

	infile[0] = 0;
	path[0] = 0;
	for (int i = 0; i < argc; i++)
	{
		TCHAR *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{
			switch (arg[1])
			{
				case 'v':
					verboseHelp = true;
					break;

				case 'd':
					if (++i < argc) {
						_tcscpy(path, argv[i]);
					}
					files = true;
					break;

				case 'h':
					headers = true;
					break;

				case 'c':
					bodies = true;
					break;

				default:
					_fputts( _T("ERROR: Unknown argument specified \""), stderr );
					_fputts( arg, stderr );
					_fputts( _T("\".\n"), stderr );
					return false;
			}
		}
		else if (infile[0] == 0)
		{
			_tcscpy(infile, arg);
		}
	}
	if (infile[0] == 0)
	{
		_ftprintf( stderr, _T("File not specified.\n") );
		return false;
	}

	TCHAR drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	GetFullPathName(infile, _countof(fullpath), fullpath, NULL);
	_tsplitpath(fullpath, drive, dir, fname, ext);
	if (path[0] == 0) {
		_tmakepath(path, drive, dir, fname, NULL);
	}

	struct _stat64 fstats;
	if ( -1 == _tstat64(fullpath, &fstats) ){
		_ftprintf( stderr, _T("Path is not a valid file or directory.\n") );
		return false;
	}

	if ( (fstats.st_mode & _S_IFDIR) == _S_IFDIR ) // file not directory
	{
		_ftprintf( stderr, _T("Path is not a valid file or directory.\n") );
		return false;
	}
	if (headers || !bodies || files)
	{
		DAO::GFF::GFFFile gfffile;
		gfffile.open(fullpath);

		Text hdrType = gfffile.get_FileInfo().Header.FileType.c_str();
		hdrType.Trim();

		DAODumpStream out(NULL, false);
		const DAO::GFF::GFFInfo& info = gfffile.get_FileInfo();
		for (int i=0; i<info.Structs.size(); ++i) {
			DAO::GFF::GFFStructDef &sval = *info.Structs[i];

			Text className = sval.StructType.data();
			className.Trim();
			ConvertClassName(className);

			if (files)
			{
				TCHAR filename[MAX_PATH];
				PathCombine(filename, path, className.c_str());
				PathAddExtension(filename, ".h");
				out.Open(filename, false);
			}

			out.PrintF(hdrFormat
				, className.c_str()
				, hdrType.c_str()
				, hdrType.c_str()
				, hdrType.c_str()
				, hdrType.c_str()
				);

			out.Indent(); out.Print("///////////////////////////////////////////////////////////////////\n\n");

			out.Indent(), out.PrintF("class %s\n", className.c_str());
			out.Indent(), out.Print("{\n");
			out.Indent(), out.Print("protected:\n");
			out.IncreaseIndent();
			out.Indent(), out.Print("GFFStructRef impl;\n");
			out.Indent(), out.Print("static ShortString type;\n");
			out.DecreaseIndent();
			out.Indent(), out.Print("public:\n");
			out.IncreaseIndent();
			out.Indent(), out.PrintF("%s(GFFStructRef owner);\n\n", className.c_str());
			out.Indent(); out.PrintF("static const ShortString& Type() { return type; }\n");
			out.Indent(); out.PrintF("const ShortString& get_type() const { return type; }\n\n");

			for (int i=0; i<sval.Fields.count(); ++i)
			{
				DAO::GFF::GFFFieldDefRef field ( sval.Fields[i] );
				Text name = EnumToText(field->Label);
				name.remove(0,4);

				Text memberName = name; ConvertMemberName(memberName);
				Text fieldName = name; ConvertFieldName(fieldName);
				Text fieldType = GetFieldDataType(field->TypeID);
				bool isPtr = false;
				bool isPrimitive = true;
				if (field->Flags & FF_LIST)
				{
					fieldType.assign("GFFListRef");
					isPtr = true, isPrimitive = false;
				}
				else if (field->Flags & FF_REFERENCE)
				{
					fieldType.append("Ref");
					isPtr = true, isPrimitive = false;
				}
				else if (field->Flags & FF_STRUCT)
				{
					fieldType.assign("GFFStructRef");
					isPtr = true, isPrimitive = false;
				}

				if (field->TypeID == DT_ECSTRING)
					isPrimitive = false;
				
				out.Indent(), out.PrintF("%s get_%s() const;\n", fieldType.c_str(), memberName.c_str());
				if (!isPtr)
				{
					if (isPrimitive)
						out.Indent(), out.PrintF("void set_%s(%s value);\n", memberName.c_str(), fieldType.c_str());
					else
						out.Indent(), out.PrintF("void set_%s(const %s& value);\n", memberName.c_str(), fieldType.c_str());
				}
				out.Print("\n");
			}
			out.DecreaseIndent();		
			out.Indent(), out.Print("};\n");
			out.Print("\n");
			out.Indent(), out.PrintF("typedef ValuePtr<%s> %sPtr;\n", className.c_str(), className.c_str());
			out.Indent(), out.PrintF("typedef ValueRef<%s> %sRef;\n", className.c_str(), className.c_str());
			out.Print("\n\n");

			out.PrintF(ftrFormat
				, hdrType.c_str());

			out.Close();
		}

	}
	if (bodies || files)
	{
		DAO::GFF::GFFFile gfffile;
		gfffile.open(fullpath);

		Text hdrType = gfffile.get_FileInfo().Header.FileType.c_str();
		hdrType.Trim();

		DAODumpStream out(NULL, false);

		const DAO::GFF::GFFInfo& info = gfffile.get_FileInfo();
		for (int i=0; i<info.Structs.size(); ++i) 
		{
			DAO::GFF::GFFStructDef &sval = *info.Structs[i];

			Text className = sval.StructType.data();
			className.Trim();
			ConvertClassName(className);

			if (files)
			{
				TCHAR filename[MAX_PATH];
				PathCombine(filename, path, className.c_str());
				PathAddExtension(filename, ".cpp");
				out.Open(filename, false);
			}

			out.PrintF(implHdrFormat
				, className.c_str()
				, hdrType.c_str(), hdrType.c_str(), hdrType.c_str()
				, hdrType.c_str(), className.c_str()
				, hdrType.c_str()
				);

			out.Indent(); 
			out.Print("///////////////////////////////////////////////////////////////////\n\n");

			out.Indent();
			out.PrintF("ShortString %s::type(\"%s\");", className.c_str(), className.c_str());

			out.Indent(); 
			out.PrintF("%s::%s(GFFStructRef owner) : impl(owner) {\n}\n\n", className.c_str(), className.c_str());

			for (int i=0; i<sval.Fields.count(); ++i)
			{
				DAO::GFF::GFFFieldDefRef field ( sval.Fields[i] );
				Text name = EnumToText(field->Label);
				name.remove(0,4);

				Text memberName = name; ConvertMemberName(memberName);
				Text fieldName = name; ConvertFieldName(fieldName);
				Text fieldType = GetFieldDataType(field->TypeID);
				Text fieldAccessor = GetFieldDataTypeToFieldName(field->TypeID);
				Text idName = EnumToText(field->Label);

				bool isPtr = false;
				bool isPrimitive = true;
				if (field->Flags & FF_LIST)
				{
					fieldType.assign("GFFListRef");
					fieldAccessor.assign("asList");
					isPtr = true, isPrimitive = false;
				}
				else if (field->Flags & FF_REFERENCE)
				{
					fieldType.append("Ref");
					isPtr = true, isPrimitive = false;
				}
				else if (field->Flags & FF_STRUCT)
				{
					fieldType.assign("GFFStructRef");
					fieldAccessor.assign("asStruct");
					isPtr = true, isPrimitive = false;
				}
				if (field->TypeID == DT_ECSTRING)
					isPrimitive = false;
				
				out.Indent(); out.PrintF("%s %s::get_%s() const {\n", fieldType.c_str(), className.c_str(), memberName.c_str());
				out.IncreaseIndent();
				out.Indent(); out.PrintF("return impl->GetField(GFF_%s)->%s();\n", idName.c_str(), fieldAccessor.c_str());
				out.DecreaseIndent();
				out.Indent(); out.Print("}\n\n");

				if (!isPtr)
				{
					out.Indent(); 
					if (isPrimitive)
						out.PrintF("void %s::set_%s(%s value) {\n", className.c_str(), memberName.c_str(), fieldType.c_str());
					else
						out.PrintF("void %s::set_%s(const %s& value) {\n", className.c_str(), memberName.c_str(), fieldType.c_str());
					out.IncreaseIndent();
					out.Indent(); out.PrintF("impl->GetField(GFF_%s)->assign(value);\n", idName.c_str());
					out.DecreaseIndent();
					out.Indent(); out.Print("}\n\n");
				}
			}
			out.Close();
		}
	}

	return false;
}

REGISTER_COMMAND(DumpGFFHeader, HelpString, ExecuteCmd);
