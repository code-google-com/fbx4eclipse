
/**********************************************************************
*<
FILE: AMEL.h

DESCRIPTION:	MMH File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "MMH/MMHCommon.h"
#include "MMH/AMEL.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString AMEL::type("AMEL");AMEL::AMEL(GFFStructRef owner) : impl(owner) {
}

float AMEL::get_nodeAgeMapElementPercentLifeElapsed() const {
  return impl->GetField(GFF_MMH_NODE_AGE_MAP_ELEMENT_PERCENT_LIFE_ELAPSED)->asFloat32();
}

void AMEL::set_nodeAgeMapElementPercentLifeElapsed(float value) {
  impl->GetField(GFF_MMH_NODE_AGE_MAP_ELEMENT_PERCENT_LIFE_ELAPSED)->assign(value);
}

float AMEL::get_nodeAgeMapElementScaleX() const {
  return impl->GetField(GFF_MMH_NODE_AGE_MAP_ELEMENT_SCALE_X)->asFloat32();
}

void AMEL::set_nodeAgeMapElementScaleX(float value) {
  impl->GetField(GFF_MMH_NODE_AGE_MAP_ELEMENT_SCALE_X)->assign(value);
}

float AMEL::get_nodeAgeMapElementScaleY() const {
  return impl->GetField(GFF_MMH_NODE_AGE_MAP_ELEMENT_SCALE_Y)->asFloat32();
}

void AMEL::set_nodeAgeMapElementScaleY(float value) {
  impl->GetField(GFF_MMH_NODE_AGE_MAP_ELEMENT_SCALE_Y)->assign(value);
}

Color4 AMEL::get_nodeAgeMapElementColor() const {
  return impl->GetField(GFF_MMH_NODE_AGE_MAP_ELEMENT_COLOR)->asColor4();
}

void AMEL::set_nodeAgeMapElementColor(Color4 value) {
  impl->GetField(GFF_MMH_NODE_AGE_MAP_ELEMENT_COLOR)->assign(value);
}

