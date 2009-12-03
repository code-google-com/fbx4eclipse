
/**********************************************************************
*<
FILE: SCAL.h

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
#include "MMH/SCAL.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString SCAL::type("SCAL");SCAL::SCAL(GFFStructRef owner) : impl(owner) {
}

float SCAL::get_scale() const {
  return impl->GetField(GFF_MMH_SCALE)->asFloat32();
}

void SCAL::set_scale(float value) {
  impl->GetField(GFF_MMH_SCALE)->assign(value);
}

