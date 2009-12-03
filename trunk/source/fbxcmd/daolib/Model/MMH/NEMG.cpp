
/**********************************************************************
*<
FILE: NEMG.h

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
#include "MMH/NEMG.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NEMG::type("NEMG");NEMG::NEMG(GFFStructRef owner) : impl(owner) {
}

Text NEMG::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void NEMG::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

Text NEMG::get_materialObject() const {
  return impl->GetField(GFF_MMH_MATERIAL_OBJECT)->asECString();
}

void NEMG::set_materialObject(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_OBJECT)->assign(value);
}

GFFListRef NEMG::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

