/**********************************************************************
*<
FILE: MaterialObject.h

DESCRIPTION:	MaterialObject File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "MAO/MAOCommon.h"
#include "MATFormat.h"
#include "boost/any.hpp"
#include "boost/variant.hpp"

namespace DAO {

typedef std::map<_tstring, boost::any, ltstr> ParameterSet;
typedef std::map<_tstring, ParameterSet, ltstr> ParameterSets;

namespace MAO {

class Texture
{
public:
	_tstring resName;
};

class Material
{
public:
	_tstring name;
	ParameterSets sets;
};

class MaterialObject
{
public:
	_tstring Name;
	_tstring Material;
	_tstring DefaultSemantic;
	ParameterSet ParameterDefaults;
	ParameterSet ParameterOverrides;

	void loadSettingsFromMaterial( DAO::MAT::MaterialRef material );

	const boost::any& asAny(LPCTSTR param);

	_tstring asText(LPCTSTR param);
	signed char asInt8(LPCTSTR param);
	unsigned char asUInt8(LPCTSTR param);
	signed short asInt16(LPCTSTR param);
	unsigned short asUInt16(LPCTSTR param);
	signed long asInt32(LPCTSTR param);
	unsigned long asUInt32(LPCTSTR param);
	signed __int64 asInt64(LPCTSTR param);
	unsigned __int64 asUInt64(LPCTSTR param);
	float asFloat32(LPCTSTR param);
	double asFloat64(LPCTSTR param);
	Vector2f asVector2f(LPCTSTR param);
	Vector3f asVector3f(LPCTSTR param);
	Vector4f asVector4f(LPCTSTR param);
	Vector2fArray asVector2fArray(LPCTSTR param);
	Vector3fArray asVector3fArray(LPCTSTR param);
	Vector4fArray asVector4fArray(LPCTSTR param);
	Quaternion asQuat(LPCTSTR param);
	Color4 asColor4(LPCTSTR param);
	Matrix44 asMatrix4(LPCTSTR param);
	Texture asTexture(LPCTSTR param);

};

typedef ValueRef<MaterialObject> MaterialObjectRef;


} // namespace MAO
} // namespace DAO
