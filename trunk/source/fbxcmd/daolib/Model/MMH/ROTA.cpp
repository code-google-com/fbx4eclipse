
/**********************************************************************
*<
FILE: ROTA.h

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
#include "MMH/ROTA.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString ROTA::type("ROTA");ROTA::ROTA(GFFStructRef owner) : impl(owner) {
}

Quaternion ROTA::get_rotation() const {
  return impl->GetField(GFF_MMH_ROTATION)->asQuat();
}

void ROTA::set_rotation(Quaternion value) {
  impl->GetField(GFF_MMH_ROTATION)->assign(value);
}

