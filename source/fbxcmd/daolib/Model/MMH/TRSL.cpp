
/**********************************************************************
*<
FILE: TRSL.h

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
#include "MMH/TRSL.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString TRSL::type("TRSL");TRSL::TRSL(GFFStructRef owner) : impl(owner) {
}

Vector4f TRSL::get_translation() const {
  return impl->GetField(GFF_MMH_TRANSLATION)->asVector4f();
}

void TRSL::set_translation(Vector4f value) {
  impl->GetField(GFF_MMH_TRANSLATION)->assign(value);
}

