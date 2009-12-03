
/**********************************************************************
*<
FILE: MSHS.h

DESCRIPTION:	PHY File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "PHY/PHYCommon.h"
#include "PHY/MSHS.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString MSHS::type("MSHS");MSHS::MSHS(GFFStructRef owner) : impl(owner) {
}

unsigned long MSHS::get_shapeMeshShapeFlags() const {
  return impl->GetField(GFF_MMH_SHAPE_MESH_SHAPE_FLAGS)->asUInt32();
}

void MSHS::set_shapeMeshShapeFlags(unsigned long value) {
  impl->GetField(GFF_MMH_SHAPE_MESH_SHAPE_FLAGS)->assign(value);
}

unsigned long MSHS::get_shapeMeshHeightFieldAxis() const {
  return impl->GetField(GFF_MMH_SHAPE_MESH_HEIGHT_FIELD_AXIS)->asUInt32();
}

void MSHS::set_shapeMeshHeightFieldAxis(unsigned long value) {
  impl->GetField(GFF_MMH_SHAPE_MESH_HEIGHT_FIELD_AXIS)->assign(value);
}

float MSHS::get_shapeMeshHeightFieldExtent() const {
  return impl->GetField(GFF_MMH_SHAPE_MESH_HEIGHT_FIELD_EXTENT)->asFloat32();
}

void MSHS::set_shapeMeshHeightFieldExtent(float value) {
  impl->GetField(GFF_MMH_SHAPE_MESH_HEIGHT_FIELD_EXTENT)->assign(value);
}

GFFListRef MSHS::get_shapeCookedDataStream() const {
  return impl->GetField(GFF_MMH_SHAPE_COOKED_DATA_STREAM)->asList();
}

