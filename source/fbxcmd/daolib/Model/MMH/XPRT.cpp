
/**********************************************************************
*<
FILE: XPRT.h

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
#include "MMH/XPRT.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString XPRT::type("XPRT");XPRT::XPRT(GFFStructRef owner) : impl(owner) {
}

Text XPRT::get_exportExportName() const {
  return impl->GetField(GFF_MMH_EXPORT_EXPORT_NAME)->asECString();
}

void XPRT::set_exportExportName(const Text& value) {
  impl->GetField(GFF_MMH_EXPORT_EXPORT_NAME)->assign(value);
}

unsigned long XPRT::get_exportControllerType() const {
  return impl->GetField(GFF_MMH_EXPORT_CONTROLLER_TYPE)->asUInt32();
}

void XPRT::set_exportControllerType(unsigned long value) {
  impl->GetField(GFF_MMH_EXPORT_CONTROLLER_TYPE)->assign(value);
}

unsigned long XPRT::get_exportTagVariableType() const {
  return impl->GetField(GFF_MMH_EXPORT_TAG_VARIABLE_TYPE)->asUInt32();
}

void XPRT::set_exportTagVariableType(unsigned long value) {
  impl->GetField(GFF_MMH_EXPORT_TAG_VARIABLE_TYPE)->assign(value);
}

unsigned long XPRT::get_exportControllerIndex() const {
  return impl->GetField(GFF_MMH_EXPORT_CONTROLLER_INDEX)->asUInt32();
}

void XPRT::set_exportControllerIndex(unsigned long value) {
  impl->GetField(GFF_MMH_EXPORT_CONTROLLER_INDEX)->assign(value);
}

