
/**********************************************************************
*<
FILE: NTRN.h

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
#include "MMH/NTRN.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NTRN::type("NTRN");NTRN::NTRN(GFFStructRef owner) : impl(owner) {
}

Text NTRN::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void NTRN::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

Text NTRN::get_materialObject() const {
  return impl->GetField(GFF_MMH_MATERIAL_OBJECT)->asECString();
}

void NTRN::set_materialObject(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_OBJECT)->assign(value);
}

Text NTRN::get_materialLibrary() const {
  return impl->GetField(GFF_MMH_MATERIAL_LIBRARY)->asECString();
}

void NTRN::set_materialLibrary(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_LIBRARY)->assign(value);
}

Text NTRN::get_id() const {
  return impl->GetField(GFF_MMH_ID)->asECString();
}

void NTRN::set_id(const Text& value) {
  impl->GetField(GFF_MMH_ID)->assign(value);
}

GFFListRef NTRN::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

