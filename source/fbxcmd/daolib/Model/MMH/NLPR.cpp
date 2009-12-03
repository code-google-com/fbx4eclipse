
/**********************************************************************
*<
FILE: NLPR.h

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
#include "MMH/NLPR.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NLPR::type("NLPR");NLPR::NLPR(GFFStructRef owner) : impl(owner) {
}

Text NLPR::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void NLPR::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

GFFListRef NLPR::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

