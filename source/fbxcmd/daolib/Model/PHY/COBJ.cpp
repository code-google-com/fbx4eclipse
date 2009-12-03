
/**********************************************************************
*<
FILE: COBJ.h

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
#include "PHY/COBJ.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString COBJ::type("COBJ");COBJ::COBJ(GFFStructRef owner) : impl(owner) {
}

float COBJ::get_nodeCollisionObjDensity() const {
  return impl->GetField(GFF_MMH_NODE_COLLISION_OBJ_DENSITY)->asFloat32();
}

void COBJ::set_nodeCollisionObjDensity(float value) {
  impl->GetField(GFF_MMH_NODE_COLLISION_OBJ_DENSITY)->assign(value);
}

unsigned long COBJ::get_nodeCollisionObjType() const {
  return impl->GetField(GFF_MMH_NODE_COLLISION_OBJ_TYPE)->asUInt32();
}

void COBJ::set_nodeCollisionObjType(unsigned long value) {
  impl->GetField(GFF_MMH_NODE_COLLISION_OBJ_TYPE)->assign(value);
}

unsigned long COBJ::get_collisionGroup() const {
  return impl->GetField(GFF_MMH_COLLISION_GROUP)->asUInt32();
}

void COBJ::set_collisionGroup(unsigned long value) {
  impl->GetField(GFF_MMH_COLLISION_GROUP)->assign(value);
}

GFFListRef COBJ::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

