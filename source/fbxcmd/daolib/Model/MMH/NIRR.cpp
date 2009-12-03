
/**********************************************************************
*<
FILE: NIRR.h

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
#include "MMH/NIRR.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NIRR::type("NIRR");NIRR::NIRR(GFFStructRef owner) : impl(owner) {
}

Text NIRR::get_lightprobeIrradianceRes() const {
  return impl->GetField(GFF_MMH_LIGHTPROBE_IRRADIANCE_RES)->asECString();
}

void NIRR::set_lightprobeIrradianceRes(const Text& value) {
  impl->GetField(GFF_MMH_LIGHTPROBE_IRRADIANCE_RES)->assign(value);
}

Matrix44 NIRR::get_lightprobeIrradianceRed() const {
  return impl->GetField(GFF_MMH_LIGHTPROBE_IRRADIANCE_RED)->asMatrix4();
}

void NIRR::set_lightprobeIrradianceRed(Matrix44 value) {
  impl->GetField(GFF_MMH_LIGHTPROBE_IRRADIANCE_RED)->assign(value);
}

Matrix44 NIRR::get_lightprobeIrradianceGreen() const {
  return impl->GetField(GFF_MMH_LIGHTPROBE_IRRADIANCE_GREEN)->asMatrix4();
}

void NIRR::set_lightprobeIrradianceGreen(Matrix44 value) {
  impl->GetField(GFF_MMH_LIGHTPROBE_IRRADIANCE_GREEN)->assign(value);
}

Matrix44 NIRR::get_lightprobeIrradianceBlue() const {
  return impl->GetField(GFF_MMH_LIGHTPROBE_IRRADIANCE_BLUE)->asMatrix4();
}

void NIRR::set_lightprobeIrradianceBlue(Matrix44 value) {
  impl->GetField(GFF_MMH_LIGHTPROBE_IRRADIANCE_BLUE)->assign(value);
}

