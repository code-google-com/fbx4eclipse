
/**********************************************************************
*<
FILE: CRST.h

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
#include "MMH/CRST.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString CRST::type("CRST");CRST::CRST(GFFStructRef owner) : impl(owner) {
}

Text CRST::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void CRST::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

unsigned char CRST::get_nodeCrustHookId() const {
  return impl->GetField(GFF_MMH_NODE_CRUST_HOOK_ID)->asUInt8();
}

void CRST::set_nodeCrustHookId(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_CRUST_HOOK_ID)->assign(value);
}

GFFListRef CRST::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

