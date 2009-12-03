
/**********************************************************************
*<
FILE: DECL.h

DESCRIPTION:	MESH File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "MESH/MESHCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "../DataHeader.h"

namespace DAO {

using namespace GFF;
namespace MESH {
///////////////////////////////////////////////////////////////////

enum DECLTYPE : unsigned long
{
	/// 1 float 
	DECLTYPE_FLOAT1 = 0,
	/// 2 floats
	DECLTYPE_FLOAT2 = 1,
	/// 3 floats
	DECLTYPE_FLOAT3 = 2,
	/// 4 floats
	DECLTYPE_FLOAT4 = 3,
	/// 4-byte color
	DECLTYPE_COLOR = 4,
	/// 4 unsigned bytes
	DECLTYPE_UBYTE4 = 5,
	/// 2 shorts
	DECLTYPE_SHORT2 = 6,
	/// 4 shorts
	DECLTYPE_SHORT4 = 7,
	/// 4 normalized bytes
	DECLTYPE_UBYTE4N = 8,
	/// 2 normalized shorts
	DECLTYPE_SHORT2N = 9,
	/// 4 normalized shorts
	DECLTYPE_SHORT4N = 10,
	/// 2 normalized unsigned shorts
	DECLTYPE_USHORT2N = 11,
	/// 4 normalized unsigned shorts
	DECLTYPE_USHORT4N = 12,
	/// 3d unsigned 10/10/10 format
	DECLTYPE_UDEC3 = 13,
	/// 3d unsigned 10/10/10 normalized
	DECLTYPE_DEC3N = 14,
	/// 2 16-bit floats
	DECLTYPE_FLOAT16_2 = 15,
	/// 4 16-bit unsigned floats
	DECLTYPE_FLOAT16_4 = 16,
	DECLTYPE_UNUSED = 0xffffffff
};

enum DECLUSAGE
{
	/// Position
	DECLUSAGE_POSITION = 0,
	/// Blend weights
	DECLUSAGE_BLENDWEIGHT = 1,
	/// Blend indices
	DECLUSAGE_BLENDINDICES = 2,
	/// Normal
	DECLUSAGE_NORMAL = 3,
	/// Point Size 
	DECLUSAGE_PSIZE = 4,
	/// Texture coordinates
	DECLUSAGE_TEXCOORD = 5,
	/// Tangent vector
	DECLUSAGE_TANGENT = 6,
	/// binormal vector
	DECLUSAGE_BINORMAL = 7,
	/// tessellation factor
	DECLUSAGE_TESSFACTOR = 8,
	/// PositionT 
	DECLUSAGE_POSITIONT = 9,
	/// color channel
	DECLUSAGE_COLOR = 10,
	/// fog value
	DECLUSAGE_FOG = 11,
	/// depth 
	DECLUSAGE_DEPTH = 12,
	/// sample
	DECLUSAGE_SAMPLE = 13,
	// error/other/unset
	DECLUSAGE_UNUSED = 0xffffffff
};

class DECL
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  DECL(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  long get_Stream() const;
  void set_Stream(long value);

  long get_Offset() const;
  void set_Offset(long value);

  DECLTYPE get_dataType() const;
  void set_dataType(DECLTYPE value);

  DECLUSAGE get_usage() const;
  void set_usage(DECLUSAGE value);

  unsigned long get_usageIndex() const;
  void set_usageIndex(unsigned long value);

  unsigned long get_Method() const;
  void set_Method(unsigned long value);

};

typedef ValuePtr<DECL> DECLPtr;
typedef ValueRef<DECL> DECLRef;



class DECLValue
{
	DECLTYPE type;
	LPVOID data;
public:
	DECLValue();
	DECLValue(DECLTYPE type, LPVOID data);
	DECLValue(const DECLValue& rhs);

	DECLTYPE get_type() const;
	bool isEmpty() const;
	float asFloat() const;
	Vector2f asVector2f() const;
	Vector3f asVector3f() const;
	Vector4f asVector4f() const;
	ARGB4 asColor() const;
	UByte4 asUByte4() const;
	Short2 asShort2() const;
	Short4 asShort4() const;
	UShort2 asUShort2() const;
	UShort4 asUShort4() const;
	Signed_10_10_10 asS101010() const;
	Unsigned_10_10_10 asU101010() const;
	Float16_2 asFloat16_2() const;
	Float16_4 asFloat16_4() const;
};

} //namespace MESH
} //namespace DAO


template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, DAO::MESH::DECLTYPE const & val);
Text EnumToText(DAO::MESH::DECLTYPE const & val);

template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, DAO::MESH::DECLUSAGE const & val);
Text EnumToText(DAO::MESH::DECLUSAGE const & val);
