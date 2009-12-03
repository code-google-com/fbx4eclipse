/**********************************************************************
*<
FILE: MaterialObject.h

DESCRIPTION:	MaterialObject File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#include <stdafx.h>
#include "MAO/MAOCommon.h"
#include "MAO/MaterialObject.h"
#include "MATFormat.h"
using namespace std;
using namespace DAO;
using namespace DAO::MAO;

static boost::any emptyAny;

void MaterialObject::loadSettingsFromMaterial( DAO::MAT::MaterialRef material ) 
{
	DAO::MAT::SemanticMap::iterator sitr = material->Semantics.find( this->DefaultSemantic );
	if (sitr != material->Semantics.end())
	{
		DAO::MAT::LogicList& ll = (*sitr).second.Logic.Logic;
		// just grab the first logic as default
		if ( ll.begin() != ll.end() )
		{
			DAO::MAT::Logic &l = *ll.begin();
			DAO::MAT::Pass &p = l.Technique.Pass;
			ParameterSets::iterator pitr = material->ParameterSets.find( p.RefName );
			if (pitr != material->ParameterSets.end())
			{
				ParameterSet& set = (*pitr).second;
				for (ParameterSet::iterator vitr = set.begin(); vitr != set.end(); ++vitr) {
					this->ParameterDefaults.insert(ParameterSet::value_type(vitr->first, vitr->second));
				}		
			}
			for (ParameterSet::iterator vitr = p.Parameters.begin(); vitr != p.Parameters.end(); ++vitr) {
				this->ParameterDefaults.insert(ParameterSet::value_type(vitr->first, vitr->second));
			}
		}
	}
}

const boost::any& MaterialObject::asAny(LPCTSTR param)
{
	_tstring key(param);
	ParameterSet::iterator pitr = ParameterOverrides.find( key );
	if (pitr != ParameterOverrides.end())
		return pitr->second;
	pitr = ParameterDefaults.find( key );
	if (pitr != ParameterDefaults.end())
		return pitr->second;
	return emptyAny;
}

_tstring MaterialObject::asText(LPCTSTR param) {
	const boost::any& val = asAny( param );
	if (val.type() == typeid(_tstring)) {
		return boost::any_cast<_tstring>(val);
	}
	return _tstring();
}

float MaterialObject::asFloat32(LPCTSTR param) {
	const boost::any& val = asAny( param );
	if (val.type() == typeid(float)) {
		return boost::any_cast<float>(val);
	}
	return std::numeric_limits<float>::quiet_NaN();
}

Vector4f MaterialObject::asVector4f(LPCTSTR param) {
	const boost::any& val = asAny( param );
	if (val.type() == typeid(Vector4f)) {
		return boost::any_cast<Vector4f>(val);
	}
	return Vector4f();
}

Vector4fArray MaterialObject::asVector4fArray(LPCTSTR param) {
	const boost::any& val = asAny( param );
	if (val.type() == typeid(Vector4fArray)) {
		return boost::any_cast<Vector4fArray>(val);
	}
	return Vector4fArray();
}

Color4 MaterialObject::asColor4(LPCTSTR param) {
	const boost::any& val = asAny( param );
	if (val.type() == typeid(Color4)) {
		return boost::any_cast<Color4>(val);
	}
	return Color4();
}

Matrix44 MaterialObject::asMatrix4(LPCTSTR param) {
	const boost::any& val = asAny( param );
	if (val.type() == typeid(Matrix44)) {
		return boost::any_cast<Matrix44>(val);
	}
	return Matrix44();
}

Texture MaterialObject::asTexture(LPCTSTR param) {
	const boost::any& val = asAny( param );
	if (val.type() == typeid(Texture)) {
		return boost::any_cast<Texture>(val);
	}
	return Texture();
}
