
/**********************************************************************
*<
FILE: LSTF.h

DESCRIPTION:	FDL File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "FDL/FDLCommon.h"
#include "FDL/LSTF.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::FDL;
///////////////////////////////////////////////////////////////////

ShortString LSTF::type("LSTF");LSTF::LSTF(GFFStructRef owner) : impl(owner) {
}

GFFListRef LSTF::get_fileList() const {
  return impl->GetField(GFF_DEP_FILE_LIST)->asList();
}

