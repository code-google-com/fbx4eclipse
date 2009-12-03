
/**********************************************************************
*<
FILE: SPHS.h

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
#include "PHY/SPHS.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString SPHS::type("SPHS");SPHS::SPHS(GFFStructRef owner) : impl(owner) {
}

float SPHS::get_shapeRadius() const {
  return impl->GetField(GFF_MMH_SHAPE_RADIUS)->asFloat32();
}

void SPHS::set_shapeRadius(float value) {
  impl->GetField(GFF_MMH_SHAPE_RADIUS)->assign(value);
}

