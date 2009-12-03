
/**********************************************************************
*<
FILE: FXDJ.h

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
#include "PHY/FXDJ.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString FXDJ::type("FXDJ");FXDJ::FXDJ(GFFStructRef owner) : impl(owner) {
}

