/**********************************************************************
*<
FILE: MaterialObject.h

DESCRIPTION:	MaterialObject File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "MAO/MAOCommon.h"
#include "boost/any.hpp"
#include "boost/variant.hpp"

namespace DAO {

typedef std::map<_tstring, boost::any, ltstr> ParameterSet;
typedef std::map<_tstring, ParameterSet, ltstr> ParameterSets;
typedef ValueRef<ParameterSet> ParameterSetRef;

namespace MAT {

class Texture
{
public:
	_tstring Name;
	_tstring SamplerName;
	_tstring Default;
};

class Pass
{
public:
	_tstring Name;
	_tstring VertexShader;
	_tstring PixelShader;
	_tstring State;

	ParameterSetRef Ref;
	_tstring RefName;
	stringlist RequiredStreamData;
	ParameterSet Parameters;
};
	
class Technique
{
public:
	_tstring Name;
	Pass Pass;
};

class LogicTest
{
public:
	_tstring Note;
};

class Logic
{
public:
	_tstring Note;
	LogicTest ShaderModel;
	Technique Technique;
};
typedef std::list<Logic> LogicList;

class LogicRoot
{
public:
	_tstring Note;
	LogicList Logic;
};

class Semantic 
{
public:
	_tstring Name;
	_tstring RenderClass;
	LogicRoot Logic;
};
typedef std::map<_tstring, Semantic> SemanticMap;

class Material
{
public:
	_tstring Name;
	ParameterSets ParameterSets;
	SemanticMap Semantics;
};

typedef ValuePtr<Material> MaterialPtr;
typedef ValueRef<Material> MaterialRef;


} // namespace MAO
} // namespace DAO
