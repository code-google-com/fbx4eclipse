
/**********************************************************************
*<
FILE: NPLT.h

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
#include "MMH/NPLT.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NPLT::type("NPLT");NPLT::NPLT(GFFStructRef owner) : impl(owner) {
}

Text NPLT::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void NPLT::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

long NPLT::get_id() const {
  return impl->GetField(GFF_MMH_ID)->asInt32();
}

void NPLT::set_id(long value) {
  impl->GetField(GFF_MMH_ID)->assign(value);
}

Color4 NPLT::get_nodePointLightColor() const {
  return impl->GetField(GFF_MMH_NODE_POINT_LIGHT_COLOR)->asColor4();
}

void NPLT::set_nodePointLightColor(Color4 value) {
  impl->GetField(GFF_MMH_NODE_POINT_LIGHT_COLOR)->assign(value);
}

float NPLT::get_nodePointLightRadius() const {
  return impl->GetField(GFF_MMH_NODE_POINT_LIGHT_RADIUS)->asFloat32();
}

void NPLT::set_nodePointLightRadius(float value) {
  impl->GetField(GFF_MMH_NODE_POINT_LIGHT_RADIUS)->assign(value);
}

unsigned char NPLT::get_nodePointLightIsStatic() const {
  return impl->GetField(GFF_MMH_NODE_POINT_LIGHT_IS_STATIC)->asUInt8();
}

void NPLT::set_nodePointLightIsStatic(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_POINT_LIGHT_IS_STATIC)->assign(value);
}

float NPLT::get_nodePointLightIntensityVariation() const {
  return impl->GetField(GFF_MMH_NODE_POINT_LIGHT_INTENSITY_VARIATION)->asFloat32();
}

void NPLT::set_nodePointLightIntensityVariation(float value) {
  impl->GetField(GFF_MMH_NODE_POINT_LIGHT_INTENSITY_VARIATION)->assign(value);
}

float NPLT::get_nodePointLightIntensityPeriod() const {
  return impl->GetField(GFF_MMH_NODE_POINT_LIGHT_INTENSITY_PERIOD)->asFloat32();
}

void NPLT::set_nodePointLightIntensityPeriod(float value) {
  impl->GetField(GFF_MMH_NODE_POINT_LIGHT_INTENSITY_PERIOD)->assign(value);
}

float NPLT::get_nodePointLightIntensityPeriodDelta() const {
  return impl->GetField(GFF_MMH_NODE_POINT_LIGHT_INTENSITY_PERIOD_DELTA)->asFloat32();
}

void NPLT::set_nodePointLightIntensityPeriodDelta(float value) {
  impl->GetField(GFF_MMH_NODE_POINT_LIGHT_INTENSITY_PERIOD_DELTA)->assign(value);
}

unsigned char NPLT::get_nodeLightAffectDomain() const {
  return impl->GetField(GFF_MMH_NODE_LIGHT_AFFECT_DOMAIN)->asUInt8();
}

void NPLT::set_nodeLightAffectDomain(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_LIGHT_AFFECT_DOMAIN)->assign(value);
}

unsigned char NPLT::get_lightCanBeOccluded() const {
  return impl->GetField(GFF_MMH_LIGHT_CAN_BE_OCCLUDED)->asUInt8();
}

void NPLT::set_lightCanBeOccluded(unsigned char value) {
  impl->GetField(GFF_MMH_LIGHT_CAN_BE_OCCLUDED)->assign(value);
}

unsigned char NPLT::get_useVariationTint() const {
  return impl->GetField(GFF_MMH_USE_VARIATION_TINT)->asUInt8();
}

void NPLT::set_useVariationTint(unsigned char value) {
  impl->GetField(GFF_MMH_USE_VARIATION_TINT)->assign(value);
}

unsigned char NPLT::get_nodeLightVersion() const {
  return impl->GetField(GFF_MMH_NODE_LIGHT_VERSION)->asUInt8();
}

void NPLT::set_nodeLightVersion(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_LIGHT_VERSION)->assign(value);
}

GFFListRef NPLT::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

