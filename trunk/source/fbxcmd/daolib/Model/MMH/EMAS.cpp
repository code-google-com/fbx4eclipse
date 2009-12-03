
/**********************************************************************
*<
FILE: EMAS.h

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
#include "MMH/EMAS.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString EMAS::type("EMAS");EMAS::EMAS(GFFStructRef owner) : impl(owner) {
}

GFFListRef EMAS::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

