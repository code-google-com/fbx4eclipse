
/**********************************************************************
*<
FILE: BOXS.h

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
#include "PHY/BOXS.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString BOXS::type("BOXS");BOXS::BOXS(GFFStructRef owner) : impl(owner) {
}

Vector3f BOXS::get_shapeBoxDim() const {
  return impl->GetField(GFF_MMH_SHAPE_BOX_DIM)->asVector3f();
}

void BOXS::set_shapeBoxDim(Vector3f value) {
  impl->GetField(GFF_MMH_SHAPE_BOX_DIM)->assign(value);
}

