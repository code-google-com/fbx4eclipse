
/**********************************************************************
*<
FILE: AMAP.h

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
#include "MMH/AMAP.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString AMAP::type("AMAP");AMAP::AMAP(GFFStructRef owner) : impl(owner) {
}

unsigned long AMAP::get_nodeAgeMapCount() const {
  return impl->GetField(GFF_MMH_NODE_AGE_MAP_COUNT)->asUInt32();
}

void AMAP::set_nodeAgeMapCount(unsigned long value) {
  impl->GetField(GFF_MMH_NODE_AGE_MAP_COUNT)->assign(value);
}

GFFListRef AMAP::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

