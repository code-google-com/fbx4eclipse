
/**********************************************************************
*<
FILE: SNAP.h

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
#include "MMH/SNAP.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString SNAP::type("SNAP");SNAP::SNAP(GFFStructRef owner) : impl(owner) {
}

Text SNAP::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void SNAP::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

GFFListRef SNAP::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

