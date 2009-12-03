
/**********************************************************************
*<
FILE: WTRL.h

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
#include "MMH/WTRL.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString WTRL::type("WTRL");WTRL::WTRL(GFFStructRef owner) : impl(owner) {
}

Text WTRL::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void WTRL::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

Text WTRL::get_materialObject() const {
  return impl->GetField(GFF_MMH_MATERIAL_OBJECT)->asECString();
}

void WTRL::set_materialObject(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_OBJECT)->assign(value);
}

float WTRL::get_weapontrailSegmentLength() const {
  return impl->GetField(GFF_MMH_WEAPONTRAIL_SEGMENT_LENGTH)->asFloat32();
}

void WTRL::set_weapontrailSegmentLength(float value) {
  impl->GetField(GFF_MMH_WEAPONTRAIL_SEGMENT_LENGTH)->assign(value);
}

float WTRL::get_weapontrailDuration() const {
  return impl->GetField(GFF_MMH_WEAPONTRAIL_DURATION)->asFloat32();
}

void WTRL::set_weapontrailDuration(float value) {
  impl->GetField(GFF_MMH_WEAPONTRAIL_DURATION)->assign(value);
}

unsigned char WTRL::get_useVariationTint() const {
  return impl->GetField(GFF_MMH_USE_VARIATION_TINT)->asUInt8();
}

void WTRL::set_useVariationTint(unsigned char value) {
  impl->GetField(GFF_MMH_USE_VARIATION_TINT)->assign(value);
}

GFFListRef WTRL::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

