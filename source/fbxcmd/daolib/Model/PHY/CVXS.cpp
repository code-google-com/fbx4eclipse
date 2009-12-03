
/**********************************************************************
*<
FILE: CVXS.h

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
#include "PHY/CVXS.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString CVXS::type("CVXS");CVXS::CVXS(GFFStructRef owner) : impl(owner) {
}

GFFListRef CVXS::get_shapeCookedDataStream() const {
  return impl->GetField(GFF_MMH_SHAPE_COOKED_DATA_STREAM)->asList();
}

