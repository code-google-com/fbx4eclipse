
/**********************************************************************
*<
FILE: BNDS.h

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
#include "MMH/BNDS.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString BNDS::type("BNDS");BNDS::BNDS(GFFStructRef owner) : impl(owner) {
}

Vector4f BNDS::get_boundsBoxmin() const {
  return impl->GetField(GFF_MESH_BOUNDS_BOXMIN)->asVector4f();
}

void BNDS::set_boundsBoxmin(Vector4f value) {
  impl->GetField(GFF_MESH_BOUNDS_BOXMIN)->assign(value);
}

Vector4f BNDS::get_boundsBoxmax() const {
  return impl->GetField(GFF_MESH_BOUNDS_BOXMAX)->asVector4f();
}

void BNDS::set_boundsBoxmax(Vector4f value) {
  impl->GetField(GFF_MESH_BOUNDS_BOXMAX)->assign(value);
}

Vector4f BNDS::get_boundsSphere() const {
  return impl->GetField(GFF_MESH_BOUNDS_SPHERE)->asVector4f();
}

void BNDS::set_boundsSphere(Vector4f value) {
  impl->GetField(GFF_MESH_BOUNDS_SPHERE)->assign(value);
}

