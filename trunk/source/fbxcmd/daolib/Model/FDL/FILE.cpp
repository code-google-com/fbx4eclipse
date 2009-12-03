
/**********************************************************************
*<
FILE: FILE.h

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
#include "FDL/FILE.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::FDL;
///////////////////////////////////////////////////////////////////

ShortString DAO::FDL::FILE::type("FILE");

DAO::FDL::FILE::FILE(GFFStructRef owner) : impl(owner) {
}

Text DAO::FDL::FILE::get_resref() const {
  return impl->GetField(GFF_DEP_RESREF)->asECString();
}

void DAO::FDL::FILE::set_resref(const Text& value) {
  impl->GetField(GFF_DEP_RESREF)->assign(value);
}

GFFListRef DAO::FDL::FILE::get_dependencyList() const {
  return impl->GetField(GFF_DEP_DEPENDENCY_LIST)->asList();
}

