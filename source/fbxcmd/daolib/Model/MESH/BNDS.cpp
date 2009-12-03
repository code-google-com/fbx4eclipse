
/**********************************************************************
*<
FILE: BNDS.h

DESCRIPTION:	MESH File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "MESH/MESHCommon.h"
#include "MESH/BNDS.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MESH;
///////////////////////////////////////////////////////////////////

ShortString BNDS::type("BNDS");BNDS::BNDS(GFFStructRef owner) : impl(owner) {
}

Vector4f BNDS::get_Boxmin() const {
  return impl->GetField(GFF_MESH_BOUNDS_BOXMIN)->asVector4f();
}

void BNDS::set_Boxmin(Vector4f value) {
  impl->GetField(GFF_MESH_BOUNDS_BOXMIN)->assign(value);
}

Vector4f BNDS::get_Boxmax() const {
  return impl->GetField(GFF_MESH_BOUNDS_BOXMAX)->asVector4f();
}

void BNDS::set_Boxmax(Vector4f value) {
  impl->GetField(GFF_MESH_BOUNDS_BOXMAX)->assign(value);
}

Vector4f BNDS::get_Sphere() const {
  return impl->GetField(GFF_MESH_BOUNDS_SPHERE)->asVector4f();
}

void BNDS::set_Sphere(Vector4f value) {
  impl->GetField(GFF_MESH_BOUNDS_SPHERE)->assign(value);
}

