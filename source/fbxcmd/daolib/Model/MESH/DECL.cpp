
/**********************************************************************
*<
FILE: DECL.h

DESCRIPTION:	MESH File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "MESH/MESHCommon.h"
#include "MESH/DECL.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MESH;
///////////////////////////////////////////////////////////////////


const EnumLookupType DECLTYPEs[] =
{
	/// 1 float 
	{DECLTYPE_FLOAT1,		 _T("FLOAT1")},
	/// 2 floats
	{DECLTYPE_FLOAT2,		 _T("FLOAT2")},
	/// 3 floats
	{DECLTYPE_FLOAT3,		 _T("FLOAT3")},
	/// 4 floats
	{DECLTYPE_FLOAT4,		 _T("FLOAT4")},
	/// 4-byte color
	{DECLTYPE_COLOR,		 _T("COLOR")},
	/// 4 unsigned bytes
	{DECLTYPE_UBYTE4,		 _T("UBYTE4")},
	/// 2 shorts
	{DECLTYPE_SHORT2,		 _T("SHORT2")},
	/// 4 shorts
	{DECLTYPE_SHORT4,		 _T("SHORT4")},
	/// 4 normalized bytes
	{DECLTYPE_UBYTE4N,		 _T("UBYTE4N")},
	/// 2 normalized shorts
	{DECLTYPE_SHORT2N,		 _T("SHORT2N")},
	/// 4 normalized shorts
	{DECLTYPE_SHORT4N,		 _T("SHORT4N")},
	/// 2 normalized unsigned shorts
	{DECLTYPE_USHORT2N,		 _T("USHORT2N")},
	/// 4 normalized unsigned shorts
	{DECLTYPE_USHORT4N,		 _T("USHORT4N")},
	/// 3d unsigned 10/10/10 format
	{DECLTYPE_UDEC3,		 _T("UDEC3")},
	/// 3d unsigned 10/10/10 normalized
	{DECLTYPE_DEC3N,		 _T("DEC3N")},
	/// 2 16-bit floats
	{DECLTYPE_FLOAT16_2,		 _T("FLOAT16_2")},
	/// 4 16-bit unsigned floats
	{DECLTYPE_FLOAT16_4,		 _T("FLOAT16_4")},
	{DECLTYPE_UNUSED,			_T("UNUSED")},
	{0,  NULL},
};


const EnumLookupType DECLUSAGEs[] =
{
	/// Position
	{DECLUSAGE_POSITION,		 _T("POSITION")},
	/// Blend weights
	{DECLUSAGE_BLENDWEIGHT,		 _T("BLENDWEIGHT")},
	/// Blend indices
	{DECLUSAGE_BLENDINDICES,		 _T("BLENDINDICES")},
	/// Normal
	{DECLUSAGE_NORMAL,		 _T("NORMAL")},
	/// Point Size 
	{DECLUSAGE_PSIZE,		 _T("PSIZE")},
	/// Texture coordinates
	{DECLUSAGE_TEXCOORD,		 _T("TEXCOORD")},
	/// Tangent vector
	{DECLUSAGE_TANGENT,		 _T("TANGENT")},
	/// binormal vector
	{DECLUSAGE_BINORMAL,		 _T("BINORMAL")},
	/// tessellation factor
	{DECLUSAGE_TESSFACTOR,		 _T("TESSFACTOR")},
	/// PositionT 
	{DECLUSAGE_POSITIONT,		 _T("POSITIONT")},
	/// color channel
	{DECLUSAGE_COLOR,		 _T("COLOR")},
	/// fog value
	{DECLUSAGE_FOG,		 _T("FOG")},
	/// depth 
	{DECLUSAGE_DEPTH,		 _T("DEPTH")},
	/// sample
	{DECLUSAGE_SAMPLE,		 _T("SAMPLE")},
	// error/other/unset
	{DECLUSAGE_UNUSED,		 _T("UNUSED")},
	{0,  NULL},
};

template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, DECLTYPE const & val)
{
	Dump(out, name, EnumToString(val, DECLTYPEs));
}

Text EnumToText(DECLTYPE const & val)
{
	return EnumToString(val, DECLTYPEs);
}

template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, DECLUSAGE const & val)
{
	Dump(out, name, EnumToString(val, DECLUSAGEs));
}

Text EnumToText(DECLUSAGE const & val)
{
	return EnumToString(val, DECLUSAGEs);
}

ShortString DECL::type("DECL");DECL::DECL(GFFStructRef owner) : impl(owner) {
}

long DECL::get_Stream() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_STREAM)->asInt32();
}

void DECL::set_Stream(long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_STREAM)->assign(value);
}

long DECL::get_Offset() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_OFFSET)->asInt32();
}

void DECL::set_Offset(long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_OFFSET)->assign(value);
}

DECLTYPE DECL::get_dataType() const {
  return (DECLTYPE)impl->GetField(GFF_MESH_VERTEXDECLARATOR_DATATYPE)->asUInt32();
}

void DECL::set_dataType(DECLTYPE value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_DATATYPE)->assign((unsigned long)value);
}

DECLUSAGE DECL::get_usage() const {
  return (DECLUSAGE)impl->GetField(GFF_MESH_VERTEXDECLARATOR_USAGE)->asUInt32();
}

void DECL::set_usage(DECLUSAGE value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_USAGE)->assign((unsigned long)value);
}

unsigned long DECL::get_usageIndex() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_USAGEINDEX)->asUInt32();
}

void DECL::set_usageIndex(unsigned long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_USAGEINDEX)->assign(value);
}

unsigned long DECL::get_Method() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_METHOD)->asUInt32();
}

void DECL::set_Method(unsigned long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_METHOD)->assign(value);
}



DECLValue::DECLValue() : data(NULL), type(DECLTYPE_UNUSED) {}

DECLValue::DECLValue(DECLTYPE type, LPVOID data) {
	this->type = type;
	this->data = data;
}
DECLValue::DECLValue(const DECLValue& rhs){
	this->type = rhs.type;
	this->data = rhs.data;
}

DECLTYPE DECLValue::get_type() const {
	return type;
}
bool DECLValue::isEmpty() const			{ 
	return type == DECLTYPE_UNUSED; 
}
float DECLValue::asFloat() const		{ 
	return type == DECLTYPE_FLOAT1 ? *(float*)data : 0.0f; 
}
Vector2f DECLValue::asVector2f() const	{ 
	return type == DECLTYPE_FLOAT2 ? *(Vector2f*)data : Vector2f(); 
}
Vector3f DECLValue::asVector3f() const  { 
	return type == DECLTYPE_FLOAT3 ? *(Vector3f*)data : Vector3f(); 
}
Vector4f DECLValue::asVector4f() const	{ 
	return type == DECLTYPE_FLOAT4 ? *(Vector4f*)data : Vector4f(); 
}
ARGB4 DECLValue::asColor() const		{ 
	return type == DECLTYPE_COLOR ? *(ARGB4*)data : ARGB4(); 
}
UByte4 DECLValue::asUByte4() const { 
	return type == DECLTYPE_UBYTE4 ? *(UByte4*)data : UByte4(); 
}
Short2 DECLValue::asShort2() const{ 
	return (type == DECLTYPE_SHORT2 || type == DECLTYPE_SHORT2N) ? *(Short2*)data : Short2(); 
}
Short4 DECLValue::asShort4() const{ 
	return (type == DECLTYPE_SHORT4 || type == DECLTYPE_SHORT4N) ? *(Short4*)data : Short4(); 
}
UShort2 DECLValue::asUShort2() const{ 
	return type == DECLTYPE_USHORT2N ? *(UShort2*)data : UShort2(); 
}
UShort4 DECLValue::asUShort4() const{ 
	return type == DECLTYPE_USHORT4N ? *(UShort4*)data : UShort4(); 
}
Signed_10_10_10 DECLValue::asS101010() const{ 
	return type == DECLTYPE_DEC3N ? *(Signed_10_10_10*)data : Signed_10_10_10(); 
}
Unsigned_10_10_10 DECLValue::asU101010() const{ 
	return type == DECLTYPE_UDEC3 ? *(Unsigned_10_10_10*)data : Unsigned_10_10_10(); 
}
Float16_2 DECLValue::asFloat16_2() const{ 
	return type == DECLTYPE_FLOAT16_2 ? *(Float16_2*)data : Float16_2(); 
}
Float16_4 DECLValue::asFloat16_4() const{ 
	return type == DECLTYPE_FLOAT16_4 ? *(Float16_4*)data : Float16_4(); 
}
