
/**********************************************************************
*<
FILE: USRP.h

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
#include "MMH/USRP.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString USRP::type("USRP");USRP::USRP(GFFStructRef owner) : impl(owner) {
}

Text USRP::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void USRP::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

GFFListRef USRP::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

