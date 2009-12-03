
/**********************************************************************
*<
FILE: NALT.h

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
#include "MMH/NALT.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NALT::type("NALT");NALT::NALT(GFFStructRef owner) : impl(owner) {
}

Text NALT::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void NALT::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

Color4 NALT::get_nodeAmbientLightColor() const {
  return impl->GetField(GFF_MMH_NODE_AMBIENT_LIGHT_COLOR)->asColor4();
}

void NALT::set_nodeAmbientLightColor(Color4 value) {
  impl->GetField(GFF_MMH_NODE_AMBIENT_LIGHT_COLOR)->assign(value);
}

unsigned char NALT::get_useVariationTint() const {
  return impl->GetField(GFF_MMH_USE_VARIATION_TINT)->asUInt8();
}

void NALT::set_useVariationTint(unsigned char value) {
  impl->GetField(GFF_MMH_USE_VARIATION_TINT)->assign(value);
}

GFFListRef NALT::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

