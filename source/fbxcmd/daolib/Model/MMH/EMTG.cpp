
/**********************************************************************
*<
FILE: EMTG.h

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
#include "MMH/EMTG.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString EMTG::type("EMTG");EMTG::EMTG(GFFStructRef owner) : impl(owner) {
}

Text EMTG::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void EMTG::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

GFFListRef EMTG::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

