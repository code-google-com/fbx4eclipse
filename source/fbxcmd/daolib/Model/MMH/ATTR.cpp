
/**********************************************************************
*<
FILE: ATTR.h

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
#include "MMH/ATTR.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString ATTR::type("ATTR");ATTR::ATTR(GFFStructRef owner) : impl(owner) {
}

Text ATTR::get_attributeName() const {
  return impl->GetField(GFF_MMH_ATTRIBUTE_NAME)->asECString();
}

void ATTR::set_attributeName(const Text& value) {
  impl->GetField(GFF_MMH_ATTRIBUTE_NAME)->assign(value);
}

Text ATTR::get_attributeSourceName() const {
  return impl->GetField(GFF_MMH_ATTRIBUTE_SOURCE_NAME)->asECString();
}

void ATTR::set_attributeSourceName(const Text& value) {
  impl->GetField(GFF_MMH_ATTRIBUTE_SOURCE_NAME)->assign(value);
}

