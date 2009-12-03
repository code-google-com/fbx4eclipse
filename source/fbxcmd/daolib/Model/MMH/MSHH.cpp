
/**********************************************************************
*<
FILE: MSHH.h

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
#include "MMH/MSHH.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString MSHH::type("MSHH");MSHH::MSHH(GFFStructRef owner) : impl(owner) {
}

Text MSHH::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void MSHH::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

Text MSHH::get_materialObject() const {
  return impl->GetField(GFF_MMH_MATERIAL_OBJECT)->asECString();
}

void MSHH::set_materialObject(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_OBJECT)->assign(value);
}

Text MSHH::get_materialLibrary() const {
  return impl->GetField(GFF_MMH_MATERIAL_LIBRARY)->asECString();
}

void MSHH::set_materialLibrary(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_LIBRARY)->assign(value);
}

Text MSHH::get_id() const {
  return impl->GetField(GFF_MMH_ID)->asECString();
}

void MSHH::set_id(const Text& value) {
  impl->GetField(GFF_MMH_ID)->assign(value);
}

Text MSHH::get_meshGroupName() const {
  return impl->GetField(GFF_MMH_MESH_GROUP_NAME)->asECString();
}

void MSHH::set_meshGroupName(const Text& value) {
  impl->GetField(GFF_MMH_MESH_GROUP_NAME)->assign(value);
}

unsigned char MSHH::get_meshCastRuntimeShadow() const {
  return impl->GetField(GFF_MMH_MESH_CAST_RUNTIME_SHADOW)->asUInt8();
}

void MSHH::set_meshCastRuntimeShadow(unsigned char value) {
  impl->GetField(GFF_MMH_MESH_CAST_RUNTIME_SHADOW)->assign(value);
}

unsigned char MSHH::get_meshCastBakedShadow() const {
  return impl->GetField(GFF_MMH_MESH_CAST_BAKED_SHADOW)->asUInt8();
}

void MSHH::set_meshCastBakedShadow(unsigned char value) {
  impl->GetField(GFF_MMH_MESH_CAST_BAKED_SHADOW)->assign(value);
}

unsigned char MSHH::get_meshCutAway() const {
  return impl->GetField(GFF_MMH_MESH_CUT_AWAY)->asUInt8();
}

void MSHH::set_meshCutAway(unsigned char value) {
  impl->GetField(GFF_MMH_MESH_CUT_AWAY)->assign(value);
}

unsigned char MSHH::get_meshPunchThrough() const {
  return impl->GetField(GFF_MMH_MESH_PUNCH_THROUGH)->asUInt8();
}

void MSHH::set_meshPunchThrough(unsigned char value) {
  impl->GetField(GFF_MMH_MESH_PUNCH_THROUGH)->assign(value);
}

GFFListRef MSHH::get_meshBonesUsed() const {
  return impl->GetField(GFF_MMH_MESH_BONES_USED)->asList();
}

unsigned char MSHH::get_meshReceiveBakedShadow() const {
  return impl->GetField(GFF_MMH_MESH_RECEIVE_BAKED_SHADOW)->asUInt8();
}

void MSHH::set_meshReceiveBakedShadow(unsigned char value) {
  impl->GetField(GFF_MMH_MESH_RECEIVE_BAKED_SHADOW)->assign(value);
}

unsigned char MSHH::get_meshReceiveRuntimeShadow() const {
  return impl->GetField(GFF_MMH_MESH_RECEIVE_RUNTIME_SHADOW)->asUInt8();
}

void MSHH::set_meshReceiveRuntimeShadow(unsigned char value) {
  impl->GetField(GFF_MMH_MESH_RECEIVE_RUNTIME_SHADOW)->assign(value);
}

Text MSHH::get_nodeMeshName() const {
  return impl->GetField(GFF_MMH_NODE_MESH_NAME)->asECString();
}

void MSHH::set_nodeMeshName(const Text& value) {
  impl->GetField(GFF_MMH_NODE_MESH_NAME)->assign(value);
}

unsigned char MSHH::get_meshIsVfxMesh() const {
  return impl->GetField(GFF_MMH_MESH_IS_VFX_MESH)->asUInt8();
}

void MSHH::set_meshIsVfxMesh(unsigned char value) {
  impl->GetField(GFF_MMH_MESH_IS_VFX_MESH)->assign(value);
}

Color4 MSHH::get_meshMaterialColor() const {
  return impl->GetField(GFF_MMH_MESH_MATERIAL_COLOR)->asColor4();
}

void MSHH::set_meshMaterialColor(Color4 value) {
  impl->GetField(GFF_MMH_MESH_MATERIAL_COLOR)->assign(value);
}

unsigned char MSHH::get_useVariationTint() const {
  return impl->GetField(GFF_MMH_USE_VARIATION_TINT)->asUInt8();
}

void MSHH::set_useVariationTint(unsigned char value) {
  impl->GetField(GFF_MMH_USE_VARIATION_TINT)->assign(value);
}

GFFListRef MSHH::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

