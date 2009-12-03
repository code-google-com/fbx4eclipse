
/**********************************************************************
*<
FILE: BBOX.h

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
#include "MMH/BBOX.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString BBOX::type("BBOX");BBOX::BBOX(GFFStructRef owner) : impl(owner) {
}

Vector4f BBOX::get_boundingBoxMin() const {
  return impl->GetField(GFF_MMH_BOUNDING_BOX_MIN)->asVector4f();
}

void BBOX::set_boundingBoxMin(Vector4f value) {
  impl->GetField(GFF_MMH_BOUNDING_BOX_MIN)->assign(value);
}

Vector4f BBOX::get_boundingBoxMax() const {
  return impl->GetField(GFF_MMH_BOUNDING_BOX_MAX)->asVector4f();
}

void BBOX::set_boundingBoxMax(Vector4f value) {
  impl->GetField(GFF_MMH_BOUNDING_BOX_MAX)->assign(value);
}

