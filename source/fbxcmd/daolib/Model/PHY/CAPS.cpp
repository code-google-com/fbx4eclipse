
/**********************************************************************
*<
FILE: CAPS.h

DESCRIPTION:	PHY File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "PHY/PHYCommon.h"
#include "PHY/CAPS.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString CAPS::type("CAPS");CAPS::CAPS(GFFStructRef owner) : impl(owner) {
}

float CAPS::get_shapeRadius() const {
  return impl->GetField(GFF_MMH_SHAPE_RADIUS)->asFloat32();
}

void CAPS::set_shapeRadius(float value) {
  impl->GetField(GFF_MMH_SHAPE_RADIUS)->assign(value);
}

float CAPS::get_shapeHeight() const {
  return impl->GetField(GFF_MMH_SHAPE_HEIGHT)->asFloat32();
}

void CAPS::set_shapeHeight(float value) {
  impl->GetField(GFF_MMH_SHAPE_HEIGHT)->assign(value);
}

