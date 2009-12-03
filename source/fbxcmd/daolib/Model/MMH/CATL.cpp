
/**********************************************************************
*<
FILE: CATL.h

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
#include "MMH/CATL.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString CATL::type("CATL");CATL::CATL(GFFStructRef owner) : impl(owner) {
}

GFFListRef CATL::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

