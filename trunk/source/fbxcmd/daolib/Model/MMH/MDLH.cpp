
/**********************************************************************
*<
FILE: MDLH.h

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
#include "MMH/MDLH.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString MDLH::type("MDLH");MDLH::MDLH(GFFStructRef owner) : impl(owner) {
}

Text MDLH::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void MDLH::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

Text MDLH::get_modelHierarchyModelDataName() const {
  return impl->GetField(GFF_MMH_MODEL_HIERARCHY_MODEL_DATA_NAME)->asECString();
}

void MDLH::set_modelHierarchyModelDataName(const Text& value) {
  impl->GetField(GFF_MMH_MODEL_HIERARCHY_MODEL_DATA_NAME)->assign(value);
}

Text MDLH::get_facialAnimationBlueprintName() const {
  return impl->GetField(GFF_MMH_FACIAL_ANIMATION_BLUEPRINT_NAME)->asECString();
}

void MDLH::set_facialAnimationBlueprintName(const Text& value) {
  impl->GetField(GFF_MMH_FACIAL_ANIMATION_BLUEPRINT_NAME)->assign(value);
}

unsigned long MDLH::get_totalBones() const {
  return impl->GetField(GFF_MMH_TOTAL_BONES)->asUInt32();
}

void MDLH::set_totalBones(unsigned long value) {
  impl->GetField(GFF_MMH_TOTAL_BONES)->assign(value);
}

unsigned long MDLH::get_totalExports() const {
  return impl->GetField(GFF_MMH_TOTAL_EXPORTS)->asUInt32();
}

void MDLH::set_totalExports(unsigned long value) {
  impl->GetField(GFF_MMH_TOTAL_EXPORTS)->assign(value);
}

GFFListRef MDLH::get_modelMeshNameList() const {
  return impl->GetField(GFF_MMH_MODEL_MESH_NAME_LIST)->asList();
}

GFFListRef MDLH::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}
